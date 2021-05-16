import os
import numpy as np
from tqdm import tqdm

from plot_network_spike_activity import get_args
from plot_network_spike_activity import get_spikes_binaries_for_layer
from plot_network_spike_activity import plot_data_and_lines
from utils.utils_config import parse_yaml_config
from utils.utils_simulator import parse_array_file


'''
Almost the same code as plot_network_spike_activity.py but it will create a histogram-like of spike for each layer
'''


def make_data_for_layer_and_lines(binaries_for_layer, config):
    '''
    For each layer it will stack the spikes at each timestamps -> histogram effect
    It will also create a set of lines that will split the layer points in the plot
    And it will determine where on the y axis on the plot should the names of the layers be located

    :param binaries_for_layer: list
        output of @et_spikes_binaries_for_layer function

    :param config: dict
        the configuration

    :return: 2 lists
        @data_for_layer: list
            each element keeps the @layer_name, the position o the @layer_name on the y axis, the x coordinates of
            all the spikes of the layer, and the y coordinates for every spike in the layer
        @lines: list
            each element keeps 2 points, describing the ends of a line
    '''
    neuron_y_coord = 1

    # I know that every neuron ran for the same amount of time
    # so take the first one and see
    array_file = os.path.join(config['layers_folder'], binaries_for_layer[0]['layer_name'], binaries_for_layer[0]['binaries'][0])
    neuron_data = parse_array_file(array_file)
    n_times = neuron_data['data'].shape[0]

    data_for_layer = []
    lines = [dict(x_coord=[0, n_times], y_coord=[0, 0])]

    for layer in tqdm(binaries_for_layer):
        n_neurons = len(layer['binaries'])

        spike_histogram = np.zeros((n_times, ), dtype=np.int32)

        for binary in layer['binaries']:

            # read data of the neuron
            array_file = os.path.join(config['layers_folder'], layer['layer_name'], binary)
            neuron_data = parse_array_file(array_file)

            # need to extract the times of the spikes
            spike_times = np.where(neuron_data['data'] == 1)[0]
            spike_histogram[spike_times] += 1

        x_points = []
        y_points = []
        for t in range(n_times):
            n_spikes = spike_histogram[t]
            for i in range(n_spikes):
                x_points.append(t)
                y_points.append(neuron_y_coord + i)

        x_points = np.array(x_points)
        y_points = np.array(y_points)

        neuron_y_coord += len(layer['binaries'])

        data_for_layer.append(dict(layer_name=layer['layer_name'],
                                   label_tick=int(neuron_y_coord - n_neurons / 2),
                                   spike_x_coord=x_points,
                                   spike_y_coord=y_points))

        # make the line
        lines.append(dict(x_coord=[0, n_times], y_coord=[neuron_y_coord, neuron_y_coord]))
        neuron_y_coord += 1

    return data_for_layer, lines


if __name__ == '__main__':
    args = get_args()
    config = parse_yaml_config(args.config_file)

    # get the binaries for each layer
    binaries_for_layer = get_spikes_binaries_for_layer(config)

    # make data for each layer, also need the separation lines
    data_for_layer, lines = make_data_for_layer_and_lines(binaries_for_layer, config)

    plot_data_and_lines(data_for_layer, lines, config, file_name='cummulative_spikes.png')

