#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <sys/wait.h>
#include <string.h>
#include "../include/network.h"
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <sys/socket.h>


//產生container ip
void get_container_ip(int pid, char *ip_buf, size_t size){
    int host_part = 100 + (pid % 100); //簡單計算，利用host端紀錄容器pid然後生成ip
    snprintf(ip_buf, size, "192.168.1.%d", host_part);
}

//host端建立bridge和veth，需要在host下
void setup_host_veth(int child_pid){
    char cmd[256];
    //建立bridge
    system("ip link add name br0 type bridge 2>/dev/null");
    system("ip link set br0 up");
    system("ip addr add 192.168.1.1/24 dev br0 2>/dev/null"); //設置為gateway
    
    // 啟用IP轉發
    system("echo 1 > /proc/sys/net/ipv4/ip_forward");
    
    // 設置NAT規則（自動偵測外網介面）找出 host 的外部網卡，然後確保有一條 iptables NAT 規則，讓容器的子網 (192.168.1.0/24) 可以透過 host 出去
    system("sh -c 'IF=$(ip route show default 0.0.0.0/0 | awk \"NR==1{print $5}\"); iptables -t nat -C POSTROUTING -s 192.168.1.0/24 -o $IF -j MASQUERADE 2>/dev/null || iptables -t nat -A POSTROUTING -s 192.168.1.0/24 -o $IF -j MASQUERADE'");

    //建立veth pair
    snprintf(cmd, sizeof(cmd), "ip link add veth_host_%d type veth peer name veth_child_%d", child_pid, child_pid);
    system(cmd);

    //將host端放到bridge
    snprintf(cmd, sizeof(cmd), "ip link set veth_host_%d master br0", child_pid);
    system(cmd);
    snprintf(cmd, sizeof(cmd), "ip link set veth_host_%d up", child_pid);
    system(cmd);

    //將container端veth移入container namespace
    snprintf(cmd, sizeof(cmd), "ip link set veth_child_%d netns %d", child_pid, child_pid);
    system(cmd);

    printf("[host] veth and bridge configured for container %d\n", child_pid);
}


int setup_container_eth(pid_t child_pid, const char *bridge_name) {
    char cmd[256];
    char ip[32];
    //需要在container下設定，使用nsenter讓其在host下也可以透過child pid操作

    //產生ip
    get_container_ip(child_pid, ip, sizeof(ip));

    // 生成唯一名字
    char veth_host[64], veth_child[64];

    snprintf(veth_host, sizeof(veth_host), "veth_host_%d", child_pid);
    snprintf(veth_child, sizeof(veth_child), "veth_child_%d", child_pid);  
    printf(">>>> [network] setup veth pair: %s <-> %s\n", veth_host, veth_child);

    // veth pair 已在 host 端建立並將 child 端移入 netns
    // 這裡僅進行容器內端與 host 端必要的啟用與配置
    (void)bridge_name; // bridge 綁定已在 host 端完成
    snprintf(cmd, sizeof(cmd), "ip link set %s up 2>/dev/null", veth_host);
    system(cmd);

    // 4. 在 container 內設定 eth0
    // rename veth_child -> eth0, up, 配置 IP
    snprintf(cmd, sizeof(cmd),
        "nsenter -t %d -n ip link set %s name eth0", child_pid, veth_child);
    system(cmd);

    snprintf(cmd, sizeof(cmd),
        "nsenter -t %d -n ip link set eth0 up", child_pid);
    system(cmd);

    snprintf(cmd, sizeof(cmd),
        "nsenter -t %d -n ip addr add %s/24 dev eth0", child_pid, ip);
    system(cmd);

    //設定default route走 192.168.1.1 host bridge
    snprintf(cmd, sizeof(cmd),
        "nsenter -t %d -n ip route add default via 192.168.1.1", child_pid);
    system(cmd);

    // 設置DNS（需進入 mount namespace 才能寫入容器的 /etc）
    snprintf(cmd, sizeof(cmd),
        "nsenter -t %d -n -m sh -c 'printf \"nameserver 8.8.8.8\\n\" > /etc/resolv.conf'", child_pid);
    system(cmd);

    // 容器內不需開啟 IP 轉發；對外通訊依賴 host 的 NAT 與轉發

    printf(">>>> [network] Container PID %d is already set eth0 and IP\n", child_pid);
    return 0;
}