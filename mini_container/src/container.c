//實作容器核心邏輯
// fork 子程序 (child process)
// 設定 namespace、cgroup
// 切換 rootfs
// 最後 execve() 進入使用者指定的程式

#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "../include/namespace.h"
#include "../include/filesystem.h"
#include "../include/cgroups.h"
#include "../include/container.h"
#include "../include/network.h"

int container_main(void *arg){
    printf(">>>> [child] Starting the container setup <<<<\n");
    
    //setup namespace
    setup_namespace();
    //setup rooft filesystem
    setup_rootfs((const char *)arg);

    //setup network
    //setup_container_eth(getpid(), NULL);

    //執行使用者指定的程式
    char *const child_args[] = {"/bin/sh", NULL};
    if(execv(child_args[0], child_args) == -1){
        perror("execv");
        exit(EXIT_FAILURE);
    }

    return 0;
}

