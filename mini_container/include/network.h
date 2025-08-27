#ifndef NETWORK_H
#define NETWORK_H

#include <sys/types.h>

void get_container_ip(int pid, char *ip_buf, size_t size);
void setup_host_veth(int child_pid);
int setup_container_eth(pid_t child_pid, const char *bridge_name);

#endif