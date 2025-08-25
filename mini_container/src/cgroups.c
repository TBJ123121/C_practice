// 控制資源限制（CPU、記憶體、process 數量）。
// 使用 /sys/fs/cgroup/ API。
// 建立一個新的 cgroup 目錄（例如 /sys/fs/cgroup/mini_container/）
// 寫入 memory.max、pu.max、pids.max.... 來限制最大記憶體用量
// 透過將容器的process加入倒cgroup來達成資源限制
// 寫入process id 到 cgroup 的 cgroup.procs 檔案中， kernel會自動將該process加入到該cgroup中
// Linux cgroup v2格式

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "../include/cgroups.h"
#include <errno.h>


#define CGROUP_FOLDER "/sys/fs/cgroup/mini_container/"

void setup_cgroup(pid_t pid){
    //建立 cgroup 目錄
    if(mkdir(CGROUP_FOLDER, 0755) != 0 && errno != EEXIST){
        perror("mkdir cgroup");
    }

    //設定記憶體上限
    FILE *mem_limit = fopen(CGROUP_FOLDER "/memory.max", "w");
    if(mem_limit){
        fprintf(mem_limit, "100000000\n"); // 100MB
        fclose(mem_limit);
    } else {
        perror("fopen memory.max");
    }

    //設定CPU限制 假設20%
    FILE *cpu_limit = fopen(CGROUP_FOLDER "/cpu.max", "w");
    if(cpu_limit){
        fprintf(cpu_limit, "20000 100000\n"); // 20% of CPU
        fclose(cpu_limit);
    } else {
        perror("fopen cpu.max");
    }

    //設定process數量上限
    FILE *pid_limit = fopen(CGROUP_FOLDER "/pids.max", "w");
    if(pid_limit){
        fprintf(pid_limit, "20\n"); // 最多20個process
        fclose(pid_limit);
    } else {
        perror("fopen pids.max");
    }

    //將當前process加入到該cgroup
    FILE *cgroup_procs = fopen(CGROUP_FOLDER "/cgroup.procs", "w");
    if(cgroup_procs){
        fprintf(cgroup_procs, "%d\n", pid); // 用 child_pid
        fclose(cgroup_procs);
    } else {
        perror("fopen cgroup.procs");
    }
}