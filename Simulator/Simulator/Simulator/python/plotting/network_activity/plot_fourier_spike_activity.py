import os
import numpy as np
from tqdm import tqdm

from plotting.network_activity.plot_network_spike_activity import get_args
from plotting.network_activity.plot_network_spike_activity import get_spikes_binaries_for_layer
from plotting.network_activity.plot_network_spike_activity import plot_data_and_lines
from plotting.network_activity.plot_network_cummulative_spike_activity import get_histogram_for_layer
from utils.utils_config import parse_yaml_config
from utils.utils_simulator import parse_array_file
from utils.utils_matplotlib import line_plot


'''
Computes the fast fourier transform on the signal defined as the population coding of the layer. It will do it for each
layer and create a network line plot and individual line plots for each layer where the frequency x amplitude domain is
showed
'''


def get_fourier(signal):
    '''
    https://www.youtube.com/watch?v=spUNpyF58BY
    https://www.youtube.com/watch?v=su9YSmwZmPg&t=189s&ab_channel=FluidicColoursFluidicColours

    Computes Fast Fourier Transform

    :param signal: 1D np array

    :return:
        the fourier transform and the corresponding frequencies
    '''
    fourier = np.fft.fft(signal)
    freq = np.fft.fftfreq(len(signal)) * len(signal)
    mask = freq > 0
    # get the magnitude of the fft
    fourier = 2.0 * np.abs(fourier / len(signal))

    return fourier[mask], freq[mask]


def make_data_for_layer_and_lines(binaries_for_layer, config):
    '''
    For each layer it will compute the Fast Fourier Transform
    It will also create a set of lines that will split the layer points in the space
    And it will determine where on the y axis on the plot should the names of the layers be located

    :param binaries_for_layer: list
        output of @get_spikes_binaries_for_layer function

    :param config: dict
        the configuration

    :return: 2 lists
        @data_for_layer: list
            each element keeps the @layer_name, the position o the @layer_name on the y axis, the x coordinates of
            all the points of the layer, and the y coordinates for every point in the layer,
            the @frequency's for the layer and the @fourier values for them
        @lines: list
            each element keeps 2 points, describing the ends of a line
    '''
    # start from 0 because fourier uses real numbers and it will ruin the alignment of the network plot
    neuron_y_coord = 0

    # I know that every neuron ran for the same amount of time
    # so take the first one and see
    array_file = os.path.join(config['layers_folder'], binaries_for_layer[0]['layer_name'], binaries_for_layer[0]['binaries'][0])
    neuron_data = parse_array_file(array_file)
    n_times = neuron_data['data'].shape[0]

    data_for_layer = []
    lines = [dict(x_coord=[0, n_times // 2], y_coord=[0, 0])]

    for layer in tqdm(binaries_for_layer):
        spike_histogram = get_histogram_for_layer(n_times, layer, config)

        fourier, freq = get_fourier(spike_histogram)
        x_points = []
        y_points = []
        fourier_for_big_plot = fourier.copy() / (fourier.max() + 1e-10)
        for f in range(freq.shape[0]):
            f_amplitude = fourier_for_big_plot[f]
            x_points.append(f)
            y_points.append(neuron_y_coord + f_amplitude)

        x_points = np.array(x_points)
        y_points = np.array(y_points)

        # for romanian
        layer['layer_name'] = layer['layer_name'].replace('layer', 'nivel')

        new_neuron_y_coord = y_points.max()
        data_for_layer.append(dict(layer_name=layer['layer_name'],
                                   label_tick=neuron_y_coord + (new_neuron_y_coord - neuron_y_coord) / 2,
                                   spike_x_coord=x_points,
                                   spike_y_coord=y_points,
                                   frequency=freq,
                                   fourier=fourier))
        neuron_y_coord = new_neuron_y_coord
        # make the line
        lines.append(dict(x_coord=[0, freq.shape[0]], y_coord=[neuron_y_coord, neuron_y_coord]))

    return data_for_layer, lines


def print_frequencyes_for_layers(data_for_layer, config):
    '''
    Crates a frequency x amplitude plot for each layer to be easier to see the whole Fourier transform
    The resulted plots are an alternative from the generated network plot

    It will create a folder @frequencies in the @config['layers_folder'] folder

    :param data_for_layer: list of dict
        the first output of @make_data_for_layer_and_lines

    :param config: dict
        the configuration, should have the @layers_folder key
    '''
    layers_frequency_dir = os.path.join(config['layers_folder'], 'frequencies')
    for layer_data in data_for_layer:
        output_file = os.path.join(layers_frequency_dir, layer_data['layer_name'] + '.png')
        os.makedirs(os.path.dirname(output_file), exist_ok=True)
        line_plot(output_file=output_file,
                  x_data=layer_data['frequency'],
                  y_data=layer_data['fourier'],
                  x_label='Frecventa',  #'Frequency',
                  y_label='Magnitudine',  #'Amplitude',
                  title=layer_data['layer_name'])


def plot_network_fourier(config_file):
    config = parse_yaml_config(config_file)

    # get the binaries for each layer
    binaries_for_layer = get_spikes_binaries_for_layer(config)

    # make data for each layer, also need the separation lines
    data_for_layer, lines = make_data_for_layer_and_lines(binaries_for_layer, config)

    # create individual plots for each layer
    print_frequencyes_for_layers(data_for_layer, config)

    # create the network activity plot
    if 'title' not in config.keys():
        config['title'] = None
    plot_data_and_lines(data_for_layer, lines, config, file_name='fourier.png', x_label='Frecventa', y_label='Nivel', scatter=False)


if __name__ == '__main__':
    args = get_args()
    plot_network_fourier(args.config_file)
