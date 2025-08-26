#ifdef NETWORK_H
#define NETWORK_H

void setup_host_veth(int child_pid, char *contaienr_ip);
void setup_container_eth(int child_pid);

#endif