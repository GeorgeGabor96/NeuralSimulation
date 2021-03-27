import argparse
import numpy as np
import yaml
import os
import sys
from tqdm import tqdm
sys.path.append(os.path.join(os.path.realpath(__file__).split('python')[0], 'python'))

from utils.utils_simulator import parse_array_file
from utils.utils_config import parse_yaml_config
from utils.utils_matplotlib import NetworkSpikesPlot


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
    # wnat to show output layers at the bottom (close the y = 0) and input layer at the top (y ~ infinity)
    binaries_for_layer.reverse()
    return binaries_for_layer


def make_data_for_layer_and_lines(binaries_for_layer, config):
    data_for_layer = []
    lines = []

    neuron_y_coord = 1

    # go over each layer
    for layer in tqdm(binaries_for_layer):

        # go over each neuron in the layer
        for binary in layer['binaries']:

            # read data of the neuron
            array_file = os.path.join(config['layers_folder'], layer['layer_name'], binary)
            neuron_data = parse_array_file(array_file)

            # need to extract the times of the spikes
            spike_x_coord = np.where(neuron_data['data'] == 1)[0]
            spike_y_coord = np.ones(spike_x_coord.shape) * neuron_y_coord

            data_for_layer.append(dict(layer_name=layer['layer_name'],
                                       spike_x_coord=spike_x_coord,
                                       spike_y_coord=spike_y_coord))

            neuron_y_coord += 1

        # make the line
        lines.append(dict(x_coord=[0, neuron_data['data'].shape[0]], y_coord=[neuron_y_coord, neuron_y_coord]))
        neuron_y_coord += 1

    return data_for_layer, lines


def plot_data_and_lines(data_for_layer, lines, args):
    # create network plot object
    spikes_plotter = NetworkSpikesPlot(args.output_file)

    # plot the lines
    for line in lines:
        spikes_plotter.plot_line(line['x_coord'], line['y_coord'])

    # plot the layers
    for layer in data_for_layer:
        spikes_plotter.plot_points(layer['layer_name'], layer['spike_x_coord'], layer['spike_y_coord'])

    spikes_plotter.plot()


if __name__ == '__main__':
    args = get_args()
    config = parse_yaml_config(args.config_file)

    # get the binaries for each layer
    binaries_for_layer = get_spikes_binaries_for_layer(config)

    # make data for each layer, also need the separation lines
    data_for_layer, lines = make_data_for_layer_and_lines(binaries_for_layer, config)


    plot_data_and_lines(data_for_layer, lines, args)
