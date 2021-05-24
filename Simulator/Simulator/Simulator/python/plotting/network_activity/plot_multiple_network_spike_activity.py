import argparse
import os
import yaml

from plotting.network_activity.plot_network_spike_activity import plot_network_activity
from plotting.network_activity.plot_network_cummulative_spike_activity import plot_network_population_coding
from plotting.network_activity.plot_fourier_spike_activity import plot_network_fourier


def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--experiments_folder', type=str, required=True,
                        help='Path to a folder containing one or more experiments information dumped'
                             'form the simulator')
    parser.add_argument('--exclude_string', type=str, required=False, default='xxxxxxxxxxxxxxxxxxx',
                        help='If a inner folder in the experiment contain this sequence of characters'
                             'it will not be used')
    return parser.parse_args()


def plot_multiple(experiments_folder, exclude_string='xxxxxxxxxxxxxxxxxxx'):
    # check the experiments folder exist
    if os.path.isdir(experiments_folder) is False:
        exit(0)

    experiments = [os.path.join(experiments_folder, f) for f in os.listdir(experiments_folder)]
    experiments = [f for f in experiments if os.path.isdir(f) is True]

    for experiment in experiments:
        experiment_name = experiment.split(os.sep)[-1]
        print('Processing experiment', experiment_name)

        # build the configuration
        config = dict()
        config['layers_folder'] = experiment
        config['title'] = experiment_name
        layers = [f for f in os.listdir(experiment) if exclude_string not in f and os.path.isdir(os.path.join(experiment, f)) is True]
        layers.sort()
        config['layers'] = layers
        config['binaries_prefix'] = 'spikes'
        config['color_for_inner_substring'] = dict(inhi='blue')
        config['default_color'] = 'red'

        # create the config file
        experiment_config = os.path.join(experiment_name + '_config.yml')
        with open(experiment_config, 'w') as fp:
            yaml.dump(config, fp)

        # call the plot builder
        plot_network_activity(experiment_config)
        plot_network_population_coding(experiment_config)
        #plot_network_fourier(experiment_config)

        # remove config file
        os.remove(experiment_config)


if __name__ == '__main__':
    args = get_args()
    plot_multiple(args.experiments_folder, args.exclude_string)

