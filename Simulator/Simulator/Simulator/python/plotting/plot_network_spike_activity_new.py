import argparse
import numpy as np
import yaml
import os
import sys
from tqdm import tqdm
sys.path.append(os.path.join(os.path.realpath(__file__).split('python')[0], 'python'))



from utils.utils_simulator import parse_array_file
from utils.utils_config import parse_yaml_config
from utils.utils_matplotlib import NetworkSpikesPlotImproved


def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--config_file', type=str, required=True,
                        help='Path to the config file for the plot')
    parser.add_argument('--output_file', type=str, required=True,
                        help='Path to the file that will contain the plot')
    return parser.parse_args()


def get_spikes_binaries_for_layer(config):
    binaries_for_layer = list()

    for layer_name in tqdm(config['layers']):
        layer_folder = os.path.join(config['layers_folder'], layer_name)

        # get the files that have the prefix
        binaries = [f for f in os.listdir(layer_folder) if f.endswith('.bin')]
        spikes_binaries = [f for f in binaries if config['binaries_prefix'] in f]
        binaries_for_layer.append(dict(layer_name=layer_name, binaries=spikes_binaries))
    return binaries_for_layer


def make_data_for_layer_and_lines(binaries_for_layer, config):
    data_for_layer = []

    # go over each layer
    for layer in tqdm(binaries_for_layer):
        neuron_y_coord = 0

        n_neurons = len(layer['binaries'])

        # I know that every neuron ran for the same amount of time
        # so take the first one and see
        array_file = os.path.join(config['layers_folder'], layer['layer_name'], layer['binaries'][0])
        neuron_data = parse_array_file(array_file)
        n_times = neuron_data['data'].shape[0]

        layer_plot = np.zeros((n_neurons, n_times), dtype=np.uint8)

        # go over each neuron in the layer
        for i in range(n_neurons):

            # read data of the neuron
            array_file = os.path.join(config['layers_folder'], layer['layer_name'], layer['binaries'][i])
            neuron_data = parse_array_file(array_file)

            # need to extract the times of the spikes
            spike_x_coord = np.where(neuron_data['data'] == 1)[0]

            layer_plot[i, spike_x_coord] = 1

        # make the line
        data_for_layer.append(dict(layer_name=layer['layer_name'], layer_data=layer_plot))

    return data_for_layer


def plot_data_for_layer(data_for_layer, args):
    spikes_plotter = NetworkSpikesPlotImproved(args.output_file)

    for data in data_for_layer:
        spikes_plotter.plot_layer(data['layer_name'], data['layer_data'])

    spikes_plotter.plot()


if __name__ == '__main__':
    args = get_args()
    config = parse_yaml_config(args.config_file)

    # get the binaries for each layer
    binaries_for_layer = get_spikes_binaries_for_layer(config)

    data_for_layer = make_data_for_layer_and_lines(binaries_for_layer, config)

    plot_data_for_layer(data_for_layer, args)