//寫一些程式進入點與解析command line
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
#include <string.h>
#include "../include/network.h"

#define STACK_SIZE (1024 * 1024)   // Stack size for cloned child
static char child_stack[STACK_SIZE];
int container_main(void *arg);

// int container_main(void *arg){
//     printf(">>>> [child] Starting the container setup <<<<\n");
    
//     //setup namespace
//     setup_namespace();
//     //setup rooft filesystem
//     setup_rootfs((const char *)arg);

//     //執行使用者指定的程式
//     char *const child_args[] = {"/bin/sh", NULL};
//     if(execv(child_args[0], child_args) == -1){
//         perror("execv");
//         exit(EXIT_FAILURE);
//     }
//     return 1;
    
// }

int main(int argc, char *argv[]){
    printf(">>>> Startint the mini container <<<<\n");

    //檢查參數
    if(argc < 2 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        printf("Usage: %s <rootfs_path>\n", argv[0]);
        printf("Example: %s ./rootfs\n", argv[0]);
        printf("Options:\n");
        printf(" -h, --help    Show this help message\n");
        return 0;
    }

    //建立新的porcess設定其一些UTS namespace、PID namespace、Mount namespace
    pid_t child_pid = clone(container_main, child_stack + STACK_SIZE,
                            SIGCHLD | CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWNET, (void *)argv[1]);
    
    
    if(child_pid == -1){
        perror("clone");
        exit(EXIT_FAILURE);
    }

    //parent call setup_cgroup to limit resource
    printf("[parent] child pid = %d\n", child_pid);
    setup_cgroup(child_pid);
    setup_container_eth(child_pid, "br0");

    //wait for child process to finish
    waitpid(child_pid, NULL, 0);
    printf(">>>> [parent] Exiting the mini container <<<<\n");
    return 0;
    
}