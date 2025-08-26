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
void setup_host_veth(int child_pid, char *contaienr_ip){
    char cmd[256];
    // //建立bridge
    // system("ip link add name br0 type bridge 2>/dev/null");
    // system("ip link set br0 up");
    // system("ip addr add 192.168.1.100/24 dev br0 2>/dev/null"); //固定gateway

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

    //產生ip
    get_container_ip(child_pid, ip, sizeof(ip));

    // 生成唯一名字
    char veth_host[64], veth_child[64];

    snprintf(veth_host, sizeof(veth_host), "veth_host_%d", child_pid);
    snprintf(veth_child, sizeof(veth_child), "veth_child_%d", child_pid);  
    printf(">>>> [network] setup veth pair: %s <-> %s\n", veth_host, veth_child);

    // 1. 建立 veth pair
    snprintf(cmd, sizeof(cmd),
        "ip link add %s type veth peer name %s", veth_host, veth_child);
    system(cmd);

    // 2. 將 child 端移進 container netns
    snprintf(cmd, sizeof(cmd),
        "ip link set %s netns %d", veth_child, child_pid);
    system(cmd);

    // 3. 設定 host 端：綁到 bridge (br0)
    if (bridge_name) {
        snprintf(cmd, sizeof(cmd),
            "ip link set %s master %s", veth_host, bridge_name);
        system(cmd);
    }
    snprintf(cmd, sizeof(cmd), "ip link set %s up", veth_host);
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
        "nsenter -t %d -n ip addr add %s/24 dev eth0", child_pid, ip); // 給個簡單的唯一 IP
    system(cmd);


    snprintf(cmd, sizeof(cmd),
        "nsenter -t %d -n ip route add default via 192.168.1.1", child_pid);
    system(cmd);

    printf(">>>> [network] Container PID %d is already set eth0 and IP\n", child_pid);
    return 0;
}