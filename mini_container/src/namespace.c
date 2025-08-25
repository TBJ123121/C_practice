//使用 Linux clone() / unshare() 建立隔離：
// UTS namespace → 容器有自己的 hostname
// PID namespace → 容器內 ps 看不到外部程序
// Mount namespace → 容器有自己的掛載點
// IPC/NET namespace (進階)

#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/namespace.h"

void setup_namespace(){
    //設定 hostname
    if(sethostname("minicontainer", strlen("minicontainer")) != 0){
        perror("sethostname");
        exit(EXIT_FAILURE);
    }
    printf("[child] Hostname set to 'minicontainer'\n");
}
