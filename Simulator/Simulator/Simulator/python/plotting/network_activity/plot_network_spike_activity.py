import argparse
import numpy as np
import os
import sys
from tqdm import tqdm
sys.path.append(os.path.join(os.path.realpath(__file__).split('python')[0], 'python'))

from utils.utils_simulator import parse_array_file
from utils.utils_simulator import get_binaries_for_layers
from utils.utils_config import parse_yaml_config
from utils.utils_matplotlib import NetworkSpikesPlot


def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--config_file', type=str, required=True,
                        help='Path to the config file for the plot')
    '''
    Config file format: YAML
    Needed keys:
        layers_folder: absolute path to the folder that contains the information for the layers
        layers: the layers to consider, order of plotting respects this order
        title: title of the plot
        binaries_prefix: the prefix of the binaries that store the spikes, usually 'spikes'
    Optional keys:
        color_for_inner_string: dict where each key is a string and values are color (matplotlib style). 
                If a layer has a key in its name it will be plotting with the corresponding color 
        default_color: the default colors for a layer
    '''
    return parser.parse_args()


def get_spikes_binaries_for_layer(config):
    '''
    This will read all the the spike binary files for all the layers in the @layers variable in the config file
    and reverse the order of the layers to keep the plotting order

    :param config: dict
        the configuration

    :return: list
        @binaries_for_layer, each element has the @layer_name and the @binaries associated with it
    '''
    binaries_for_layer = get_binaries_for_layers(
        layers_folder=config['layers_folder'],
        layers=config['layers'],
        prefix=config['binaries_prefix'],
        log=True)

    # to keep the plotting order as in the config, need to start plotting from the last layer to the first one
    binaries_for_layer.reverse()
    return binaries_for_layer


def make_data_for_layer_and_lines(binaries_for_layer, config):
    '''
    For each layer it will determine all the spike coordinates in the plot
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

    # Dirty overwrite
    new_max_times = 201
    n_times = new_max_times - 1  #neuron_data['data'].shape[0]

    data_for_layer = []
    lines = [dict(x_coord=[0, n_times], y_coord=[0, 0])]

    for layer in tqdm(binaries_for_layer):
        n_neurons = len(layer['binaries'])

        # keep the x and y coordinates of a spike in 2 arrays
        spike_x_coord = np.empty((0,))
        spike_y_coord = np.empty((0,))

        for binary in layer['binaries']:

            # read data of the neuron
            array_file = os.path.join(config['layers_folder'], layer['layer_name'], binary)
            neuron_data = parse_array_file(array_file)

            # Dirty filtering
            neuron_data['data'] = neuron_data['data'][:new_max_times]

            # need to extract the times of the spikes
            spike_times = np.where(neuron_data['data'] == 1)[0]
            spike_x_coord = np.hstack((spike_x_coord, spike_times))
            spike_y_coord = np.hstack((spike_y_coord, np.ones(spike_times.shape) * neuron_y_coord))

            neuron_y_coord += 1

        # for romanian
        layer['layer_name'] = layer['layer_name'].replace('nivel', 'layer')

        data_for_layer.append(dict(layer_name=layer['layer_name'],
                                   label_tick=int(neuron_y_coord - n_neurons / 2),
                                   spike_x_coord=spike_x_coord,
                                   spike_y_coord=spike_y_coord))

        # make the line
        lines.append(dict(x_coord=[0, n_times], y_coord=[neuron_y_coord, neuron_y_coord]))
        neuron_y_coord += 1

    return data_for_layer, lines


def get_layer_color(layer_name, config):
    color = None

    if 'color_for_inner_substring' in config.keys():
        for key in config['color_for_inner_substring'].keys():
            if key in layer_name:
                color = config['color_for_inner_substring'][key]
                break
    if color is None:
        if 'default_color' in config.keys():
            color = config['default_color']
        else:
            color = 'black'
    return color


def plot_data_and_lines(data_for_layer, lines, config, file_name='network.png', x_label='Time (ms)', y_label='Layers', scatter=True):
    '''
    It uses @NetworkSpikesPlot to display the spike activity of every layer on a plot

    :param data_for_layer: list
        first output of @make_data_for_layer_and_lines function

    :param lines: list
        second output of @make_data_for_layer_and_lines function

    :param args: script parameters

    '''
    # create network plot object
    spikes_plotter = NetworkSpikesPlot(output_file=os.path.join(config['layers_folder'], file_name),
                                       title=config['title'],
                                       x_label=x_label,
                                       y_label=y_label,
                                       scatter=scatter)

    # plot the lines
    for line in lines:
        spikes_plotter.plot_line(line['x_coord'], line['y_coord'])

    # plot the layers
    for layer in data_for_layer:
        layer_name = layer['layer_name']
        #if layer['layer_name'] == 'input':
        #    layer_name = 'intrare'
        #else:
        #    layer_name = 'activitate'
        spikes_plotter.plot_points(layer_name=layer_name,
                                   label_tick=layer['label_tick'],
                                   x_coord=layer['spike_x_coord'],
                                   y_coord=layer['spike_y_coord'],
                                   color=get_layer_color(layer['layer_name'], config))

    spikes_plotter.plot()


def plot_network_activity(config_file):
    config = parse_yaml_config(config_file)

    # get the binaries for each layer
    binaries_for_layer = get_spikes_binaries_for_layer(config)

    # make data for each layer, also need the separation lines
    data_for_layer, lines = make_data_for_layer_and_lines(binaries_for_layer, config)


    if 'title' not in config.keys():
        config['title'] = None
    plot_data_and_lines(data_for_layer, lines, config, x_label='time (ms)', y_label='Layer')


if __name__ == '__main__':
    args = get_args()
    plot_network_activity(args.config_file)
