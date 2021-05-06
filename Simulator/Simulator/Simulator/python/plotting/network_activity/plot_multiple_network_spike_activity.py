import argparse
import os
import yaml


def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--experiments_folder', type=str, required=True,
                        help='Path to a folder containing one or more experiments information dumped'
                             'form the simulator')
    parser.add_argument('--exclude_string', type=str, required=True, default='xxxxxxxxxxxxxxxxxxx',
                        help='If a inner folder in the experiment contain this sequence of characters'
                             'it will not be used')
    return parser.parse_args()


if __name__ == '__main__':
    args = get_args()
    # check the experiments folder exist
    if os.path.isdir(args.experiments_folder) is False:
        exit(0)

    experiments = [os.path.join(args.experiments_folder, f) for f in os.listdir(args.experiments_folder)]
    experiments = [f for f in experiments if os.path.isdir(f) is True]

    for experiment in experiments:
        experiment_name = experiment.split(os.sep)[-1]
        print('Processing experiment', experiment_name)

        # build the configuration
        config = dict()
        config['layers_folder'] = experiment
        config['title'] = experiment_name
        layers = [f for f in os.listdir(experiment) if args.exclude_string not in f]
        layers.sort()
        config['layers'] = layers
        config['binaries_prefix'] = 'spikes'

        # create the config file
        experiment_config = os.path.join(experiment_name + '_config.yml')
        with open(experiment_config, 'w') as fp:
            yaml.dump(config, fp)

        # call the plot builder
        os.system('plot_network_spike_activity.py --config {}'.format(experiment_config))

        # remove config file
        os.remove(experiment_config)
