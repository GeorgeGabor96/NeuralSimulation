import argparse
import os
from numba import njit
import numpy as np
import sys
sys.path.append(os.path.join(os.path.realpath(__file__).split('python')[0], 'python'))
from tqdm import tqdm


from utils.utils_simulator import parse_array_file
from utils.utils_simulator import get_binaries_for_layers
from utils.utils_config import parse_yaml_config
from utils.utils_matplotlib import fill_plot


'''
Assume that the folders that contain the experiments have the name 'variable1'_'value'_'variable2'_'value', value is float
'''


def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--config_file', type=str, required=True,
                        help='Path to the config file for the plot')
    # TODO add description for the config file
    return parser.parse_args()


def get_variables_values(experiment_dir, variable1_name, variable2_name):
    experiment_dir_name = experiment_dir.split(os.path.sep)[-1]
    tokens = experiment_dir_name.split('_')
    # should have 4 values
    assert len(tokens) == 4, 'expect 2 tokens for the variable names and 2 for the values'
    assert tokens[0] == variable1_name, 'first token should be {} not {}'.format(variable1_name, tokens[0])
    assert tokens[2] == variable2_name, 'first token should be {} not {}'.format(variable2_name, tokens[2])

    return { variable1_name: float(tokens[1]), variable2_name: float(tokens[3]) }


@njit
def get_first_pulse_duration(spike_data, max_duration):
    for i in range(spike_data.shape[0]):
        # check if pulse begins
        if spike_data[i] == 1:
            # find last spike in pulse
            last_spike = i
            for j in range(i, i + max_duration):
                if spike_data[j] == 1:
                    last_spike = j
            return last_spike - i
    return 0


def get_layer_first_pulse_mean_duration(layers_dir, layer_data, max_duration):
    durations = []
    for spike_binary in layer_data['binaries']:
        spike_binary_file = os.path.join(layers_dir, layer_data['layer_name'], spike_binary)
        spike_data = parse_array_file(spike_binary_file)
        duration = get_first_pulse_duration(spike_data['data'], max_duration)
        durations.append(duration)

    mean_duration = np.mean(durations)
    return mean_duration


def get_synfire_state_for_experiment(experiment_dir, spike_binaries, max_duration):
    # receives a list
    second_l_mean_pulse_duration = get_layer_first_pulse_mean_duration(layers_dir=experiment_dir,
                                                                       layer_data=spike_binaries[1],
                                                                       max_duration=max_duration)
    last_l_mean_pulse_duration = get_layer_first_pulse_mean_duration(layers_dir=experiment_dir,
                                                                     layer_data=spike_binaries[1],
                                                                     max_duration=max_duration)

    # if the ratio between the second and last layer is between [0.5, 1.5] then stable
    second_last_duration_ratio = second_l_mean_pulse_duration / last_l_mean_pulse_duration
    # probably not the best way

    # TODO the constant given in config
    if 0.5 <= second_last_duration_ratio <= 1.5:
        return 'STABLE'
    elif second_last_duration_ratio > 1.5:
        return 'EPILEPSY'
    elif second_last_duration_ratio < 0.5:
        return 'NO_ACTIVITY'

    # we can find if the activity died when probagating
    # daca a fost activitate la layer2 si la final nu este, inseamna ca a murit pe parcurs

    return 'STABLE'


if __name__ == '__main__':
    args = get_args()
    config = parse_yaml_config(args.config_file)

    # get the folders for the experiments
    experiments_dirs = [os.path.join(config['experiments_folder'], f) for f in os.listdir(config['experiments_folder'])]
    experiments_dirs = [f for f in experiments_dirs if os.path.isdir(f) is True]
    print('[INFO] Found', len(experiments_dirs), 'experiments')

    points_for_state = {
        'NO_ACTIVITY': dict(x=[], y=[]),
        'STABLE': dict(x=[], y=[]),
        'EPILEPSY': dict(x=[], y=[])
    }

    for experiment_dir in tqdm(experiments_dirs):
        # get the variables values
        variable_values = get_variables_values(experiment_dir=experiment_dir,
                                               variable1_name=config['variable1'],
                                               variable2_name=config['variable2'])

        # get the spike binaries
        spike_binaries_for_experiment = get_binaries_for_layers(layers_folder=experiment_dir,
                                                                layers=config['layers'],
                                                                prefix='spikes',
                                                                log=False)
        state = get_synfire_state_for_experiment(experiment_dir=experiment_dir,
                                                 spike_binaries=spike_binaries_for_experiment,
                                                 max_duration=config['epileptic_spikes'])

        points_for_state[state]['x'].append(variable_values[config['variable1']])
        points_for_state[state]['y'].append(variable_values[config['variable2']])

    print(points_for_state)
    for state in points_for_state.keys():
        points_for_state[state]['x'] = np.array(points_for_state[state]['x'])
        points_for_state[state]['y'] = np.array(points_for_state[state]['y'])

    fill_plot(output_file=os.path.join(config['experiments_folder'], '2Dplot.png'),
              groups=points_for_state,
              title=config['title'],
              x_label=config['variable1'],
              y_label=config['variable2'])



