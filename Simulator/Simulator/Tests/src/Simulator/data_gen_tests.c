#include "data_gen_tests.h"
#include "data/data_gen_constant_current.h"
#include "data/data_gen_random_spikes.h"


Network* create_basic_network() {
	const char* neuron_class_name = "TEST NEURON";
	const char* synapse_class_name = "TEST SYNAPSE";
	
	Network* net = network_create();
	network_add_neuron_class(net, neuron_class_create(neuron_class_name, LIF_NEURON));
	network_add_synapse_class(net, synapse_class_create_default(synapse_class_name));
	NeuronClass* n_class = network_get_neuron_class_by_name(net, neuron_class_name);
	SynapseClass* s_class = network_get_synapse_class_by_name(net, synapse_class_name);

	// make a network with 2 input layers and one output layer
	Layer* layer_i1 = layer_create_fully_connected(10, n_class, "layer_i1");
	Layer* layer_i2 = layer_create_fully_connected(20, n_class, "layer_i2");
	Layer* layer_out = layer_create_fully_connected(1, n_class, "layer_out");
	layer_add_input_layer(layer_out, layer_i1, s_class);
	layer_add_input_layer(layer_out, layer_i2, s_class);

	network_add_layer(net, layer_i1, TRUE, FALSE);
	network_add_layer(net, layer_i2, TRUE, FALSE);
	network_add_layer(net, layer_out, FALSE, TRUE);
	network_compile(net);
	network_summary(net);

	return net;
}


TestStatus data_generator_constant_current_test() {
	Network* net = NULL;
	Layer* layer = NULL;
	DataGenerator* generator = NULL;
	DataElement* element = NULL;
	NetworkInputs* inputs = NULL;
	NetworkValues* values = NULL;
	uint32_t i = 0;
	uint32_t j = 0;
	uint32_t k = 0;
	uint32_t u = 0;
	uint32_t length = 10;
	uint32_t duration = 20;
	float current_value = 2.0f;
	float current_from_data_gen = 0.0f;

	net = create_basic_network();

	// make the generator
	generator = data_generator_constant_current_create(length, net, current_value, duration);
	assert(data_generator_is_valid(generator) == TRUE, invalid_argument("generator"));
	assert(generator->length == length, "@generator->length should be %u not %u", length, generator->length);

	for (i = 0; i < length; ++i) {
		element = data_generator_get_element(generator, i);
		assert(data_element_is_valid(element) == TRUE, invalid_argument("element"));
		assert(element->duration == duration, "element->duration should be %u not %u", duration, element->duration);

		for (j = 0; j < element->duration; ++j) {
			inputs = data_element_get_values(element, j);
			assert(inputs != NULL, null_argument("inputs"));
			assert(array_is_valid(inputs) == TRUE, invalid_argument("inputs"));
			assert(inputs->length == net->input_layers.length, "number of inputs should be %u not %u", net->input_layers.length, inputs->length);

			for (k = 0; k < inputs->length; ++k) {
				values = (NetworkValues*)array_get(inputs, k);
				layer = *((Layer**)array_get(&(net->input_layers), k));

				assert(values->type == CURRENT, "@values->tpe != CURRENT");
				assert(array_is_valid(&(values->values)) == TRUE, invalid_argument("values->values"));
				assert(values->values.length == layer->neurons.length, "@values->values.length should be %u not %u", layer->neurons.length, values->values.length);

				for (u = 0; u < values->values.length; ++u) {
					current_from_data_gen = *((float*)array_get(&(values->values), u));
					assert(current_from_data_gen == current_value, "current_from_data_gen should be %f not %f", current_value, current_from_data_gen);
				}
			}
			network_step(net, inputs, j);

			data_element_remove_values(element, inputs);
		}
		data_element_destroy(element);
	}

	data_generator_destroy(generator);
	network_destroy(net);
	assert(memory_leak() == FALSE, "Memory leak");

	return TEST_SUCCESS;
ERROR
	return TEST_FAILED;
}


TestStatus data_generator_random_spikes_test() {
	Network* net = NULL;
	Layer* layer = NULL;
	DataGenerator* generator = NULL;
	DataElement* element = NULL;
	NetworkInputs* inputs = NULL;
	NetworkValues* values = NULL;
	uint32_t i = 0;
	uint32_t j = 0;
	uint32_t k = 0;
	uint32_t u = 0;
	uint32_t length = 10;
	uint32_t duration = 20;
	float spike_percent = 0.5f;
	BOOL spikes_from_data_gen = FALSE;

	net = create_basic_network();

	// make the generator
	generator = data_generator_random_spikes_create(length, net, spike_percent, duration);
	assert(data_generator_is_valid(generator) == TRUE, invalid_argument("generator"));
	assert(generator->length == length, "@generator->length should be %u not %u", length, generator->length);

	for (i = 0; i < length; ++i) {
		element = data_generator_get_element(generator, i);
		assert(data_element_is_valid(element) == TRUE, invalid_argument("element"));
		assert(element->duration == duration, "element->duration should be %u not %u", duration, element->duration);
		
		// need this to reset the left over spikes from previous example
		network_clear_state(net);

		for (j = 0; j < element->duration; ++j) {
			inputs = data_element_get_values(element, j);
			assert(inputs != NULL, null_argument("inputs"));
			assert(array_is_valid(inputs) == TRUE, invalid_argument("inputs"));
			assert(inputs->length == net->input_layers.length, "number of inputs should be %u not %u", net->input_layers.length, inputs->length);

			for (k = 0; k < inputs->length; ++k) {
				values = (NetworkValues*)array_get(inputs, k);
				layer = *((Layer**)array_get(&(net->input_layers), k));

				assert(values->type == SPIKES, "@values->tpe != SPIKES");
				assert(array_is_valid(&(values->values)) == TRUE, invalid_argument("values->values"));
				assert(values->values.length == layer->neurons.length, "@values->values.length should be %u not %u", layer->neurons.length, values->values.length);

				for (u = 0; u < values->values.length; ++u) {
					spikes_from_data_gen = *((BOOL*)array_get(&(values->values), u));
					assert(spikes_from_data_gen == TRUE || spikes_from_data_gen == FALSE, "spikes_from_data_gen should be 1 or 0 not %d", spikes_from_data_gen);
				}
			}
			network_step(net, inputs, j);

			data_element_remove_values(element, inputs);
		}
		data_element_destroy(element);
	}

	data_generator_destroy(generator);
	network_destroy(net);
	assert(memory_leak() == FALSE, "Memory leak");

	return TEST_SUCCESS;
ERROR
	return TEST_FAILED;
}


TestStatus data_generator_with_step_between_neurons() {
	Network* net = NULL;
	Layer* layer = NULL;
	DataGenerator* generator = NULL;
	DataElement* element = NULL;
	NetworkInputs* inputs = NULL;
	NetworkValues* values = NULL;
	uint32_t i = 0;
	uint32_t j = 0;
	uint32_t k = 0;
	uint32_t u = 0;
	uint32_t length = 10;
	uint32_t duration = 20;
	uint32_t time_between_spikes = 20;
	BOOL spikes_from_data_gen = FALSE;

	net = create_basic_network();

	// make the generator
	generator = data_generator_with_step_between_neurons_create(length, net, time_between_spikes, duration);
	assert(data_generator_is_valid(generator) == TRUE, invalid_argument("generator"));
	assert(generator->length == length, "@generator->length should be %u not %u", length, generator->length);

	for (i = 0; i < length; ++i) {
		element = data_generator_get_element(generator, i);
		assert(data_element_is_valid(element) == TRUE, invalid_argument("element"));
		assert(element->duration == duration, "element->duration should be %u not %u", duration, element->duration);

		// need this to reset the left over spikes from previous example
		network_clear_state(net);

		for (j = 0; j < element->duration; ++j) {
			inputs = data_element_get_values(element, j);
			assert(inputs != NULL, null_argument("inputs"));
			assert(array_is_valid(inputs) == TRUE, invalid_argument("inputs"));
			assert(inputs->length == net->input_layers.length, "number of inputs should be %u not %u", net->input_layers.length, inputs->length);

			for (k = 0; k < inputs->length; ++k) {
				values = (NetworkValues*)array_get(inputs, k);
				layer = *((Layer**)array_get(&(net->input_layers), k));

				assert(values->type == SPIKES, "@values->tpe != SPIKES");
				assert(array_is_valid(&(values->values)) == TRUE, invalid_argument("values->values"));
				assert(values->values.length == layer->neurons.length, "@values->values.length should be %u not %u", layer->neurons.length, values->values.length);

				for (u = 0; u < values->values.length; ++u) {
					spikes_from_data_gen = *((BOOL*)array_get(&(values->values), u));
					assert(spikes_from_data_gen == TRUE || spikes_from_data_gen == FALSE, "spikes_from_data_gen should be 1 or 0 not %d", spikes_from_data_gen);
				}
			}
			network_step(net, inputs, j);

			data_element_remove_values(element, inputs);
		}
		data_element_destroy(element);
	}

	data_generator_destroy(generator);
	network_destroy(net);
	assert(memory_leak() == FALSE, "Memory leak");

	return TEST_SUCCESS;
ERROR
	return TEST_FAILED;
}
