
// struct if_nameindex
// {
//     unsigned int if_index;
//     char *if_name;
// };

// struct ifaddr
// {
//     struct sockaddr ifa_addr;
//     union
//     {
// 	struct sockaddr	ifu_broadaddr;
// 	struct sockaddr	ifu_dstaddr;
//     } ifa_ifu;
//     struct iface *ifa_ifp;
//     struct ifaddr *ifa_next;
// };

// struct ifmap
// {
//     unsigned long int mem_start;
//     unsigned long int mem_end;
//     unsigned short int base_addr;
//     unsigned char irq;
//     unsigned char dma;
//     unsigned char port;

// };

// struct ifreq
// {

//     union
//     {
// 	char ifrn_name[IFNAMSIZ];
//     } ifr_ifrn;

//     union
//     {
// 	struct sockaddr ifru_addr;
// 	struct sockaddr ifru_dstaddr;
// 	struct sockaddr ifru_broadaddr;
// 	struct sockaddr ifru_netmask;
// 	struct sockaddr ifru_hwaddr;
// 	short int ifru_flags;
// 	int ifru_ivalue;
// 	int ifru_mtu;
// 	struct ifmap ifru_map;
// 	char ifru_slave[IFNAMSIZ];
// 	char ifru_newname[IFNAMSIZ];
// 	__caddr_t ifru_data;
//     } ifr_ifru;
// };

// struct ifconf
// {
//     int	ifc_len;
//     union
//     {
// 	__caddr_t ifcu_buf;
// 	struct ifreq *ifcu_req;
//     } ifc_ifcu;
// };

// *** undocumented, so don't annotate for now ***

// unsigned int if_nametoindex (const char *ifname);
// char *if_indextoname (unsigned int ifindex, char *ifname);

// struct if_nameindex *if_nameindex (void);

// void if_freenameindex (struct if_nameindex *ptr);
