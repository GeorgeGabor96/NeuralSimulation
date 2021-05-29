#ifndef __NETWORKS_H__
#define __NETWORKS_H__

#include "Network.h"

Network* network_3_L_3_3_3();
Network* network_synfire_chain_10_layers(float connectivity, float amplitude, float s_strength, BOOL use_refract);


typedef struct network_sequential_n_layers_config {
	uint32_t n_layers;
	uint32_t n_exci_neurons;
	uint32_t n_inhi_neurons;
	float connectivity;
	float synapse_weight;
	NeuronClass* n_class;
	SynapseClass* s_exci_class;
	SynapseClass* s_inhi_class;
} network_sequential_n_layers_config;

Network* network_sequential_n_layers(network_sequential_n_layers_config* config);
Network* network_sequential_ring_n_layers(network_sequential_n_layers_config* config);

#endif // __NETWORKS_H__