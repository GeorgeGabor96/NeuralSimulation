#ifndef __NETWORKS_H__
#define __NETWORKS_H__

#include "Network.h"

Network* network_3_L_3_3_3();
Network* network_synfire_chain_10_layers(float connectivity, float amplitude, float s_strength, BOOL use_refract);


#endif // __NETWORKS_H__