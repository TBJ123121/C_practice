#ifdef FILESYSTEM_H
#define FILESYSTEM_H

void setup_rootfs(const char *rootfs_path);
void cleanup_rootfs(pid_t pid);
#endif
