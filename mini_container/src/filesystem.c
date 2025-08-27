// 處理 rootfs 掛載：
// chroot() 或 pivot_root() 切換根目錄
// 並掛載必要的目錄（/proc, /sys, /dev）

#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/filesystem.h"
#include <sys/mount.h>
#include <sys/syscall.h>
#include <string.h>
#include <errno.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <sys/sysmacros.h>

void setup_rootfs(const char *rooft_path);
void setup_mounts();

void setup_rootfs(const char *rooft_path){
    printf("[child] Setting up root filesystem at %s\n", rooft_path);

    //1. bind mount rootfs to itself to make it a mount point
    if(mount(rooft_path, rooft_path, "bind", MS_BIND | MS_REC, NULL) != 0){
        perror("mount --bind rootfs");
        exit(EXIT_FAILURE);
    }
    //2. 建立put_old 目錄
    char put_old[PATH_MAX];
    snprintf(put_old, sizeof(put_old), "%s/old_root", rooft_path);
    if(mkdir(put_old, 0755) != 0 && errno != EEXIST){
        perror("mkdir put_old");
        exit(EXIT_FAILURE);
    }

    //3. pivot_root
    if(syscall(SYS_pivot_root, rooft_path, put_old) != 0){
        perror("pivot_root");
        exit(EXIT_FAILURE);
    }

    //4. chroot 到新的 root
    if(chdir("/") != 0){
        perror("chdir");
        exit(EXIT_FAILURE);
    }

    //5. 卸載 put_old 並刪除該目錄
    // if(umount2("/old_root", MNT_DETACH) != 0){
    //     perror("unmount old_root");
    //     exit(EXIT_FAILURE);
    // }
    // rmdir("/old_root");

    setup_mounts();

    printf("[child] Root filesystem set done!\n");
}

void setup_mounts(){
    // 這裡可以添加其他需要的掛載點
    // 例如 /dev, /dev/pts, /proc, /sys 等
    mkdir("/dev", 0755);
    mkdir("/etc", 0755);
    mkdir("/proc", 0755);
    mkdir("/sys", 0755);

    if(mount("proc", "/proc", "proc", 0, NULL) != 0){
        perror("mount /proc");
        exit(EXIT_FAILURE);
    }

    if(mount("sysfs", "/sys", "sysfs", 0, NULL) != 0){
        perror("mount /sys");
        exit(EXIT_FAILURE);
    }

    if(mount("tmpfs", "/dev", "tmpfs", MS_NOSUID | MS_STRICTATIME, "mode=755") != 0){
        perror("mount /dev");
        exit(EXIT_FAILURE);
    }

    mkdir("/dev/pts", 0755);

    if(mount("devpts", "/dev/pts", "devpts", MS_NOSUID | MS_NOEXEC, "gid=0,mode=620,ptmxmode=666,newinstance") != 0){
        perror("mount /dev/pts");
        exit(EXIT_FAILURE);
    }

    // 建立基本裝置節點
    mknod("/dev/null", S_IFCHR | 0666, makedev(1, 3));
    mknod("/dev/zero", S_IFCHR | 0666, makedev(1, 5));
    mknod("/dev/tty",  S_IFCHR | 0666, makedev(5, 0));
    // 建立 ptmx symlink
    unlink("/dev/ptmx");
    if(symlink("pts/ptmx", "/dev/ptmx") != 0) {
        perror("symlink /dev/ptmx");
        exit(EXIT_FAILURE);
    }

    // 寫入基本 DNS 設定，確保容器可解析域名
    FILE *resolv = fopen("/etc/resolv.conf", "w");
    if(resolv) {
        fputs("nameserver 8.8.8.8\n", resolv);
        fclose(resolv);
    } else {
        perror("write /etc/resolv.conf");
    }
}

void cleanup_mounts(){
    umount2("/proc", MNT_DETACH);
    umount2("/sys", MNT_DETACH);
    umount2("/dev/pts", MNT_DETACH);
    umount2("/dev", MNT_DETACH);
}
