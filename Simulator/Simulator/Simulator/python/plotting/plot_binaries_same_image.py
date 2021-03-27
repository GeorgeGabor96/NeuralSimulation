import argparse

import os
import sys
sys.path.append(os.path.join(os.path.realpath(__file__).split('python')[0], 'python'))
from utils.utils_simulator import parse_array_file
from utils.utils_matplotlib import line_plot
from utils.utils_config import parse_yaml_config


def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--config_file', type=str, required=True,
                        help='Path to the config file for the plot')
    parser.add_argument('--output_file', type=str, required=True,
                        help='Path to the file that will contain the plot')
    return parser.parse_args()


def plot_binaries(args, config):
    datas = []
    labels = []

    for i in range(len(config['binaries'])):
        binary_file = os.path.join(config['common_path'], config['binaries'][i])
        sim_array = parse_array_file(binary_file)
        datas.append(sim_array['data'])
        labels.append(config['labels'][i])

    line_plot(output_file=args.output_file,
              y_data=datas,
              line_label=labels,
              x_label=config['x_label'],
              y_label=config['y_label'],
              title=config['title'])


if __name__ == '__main__':
    args = get_args()
    os.makedirs(os.path.split(args.output_file)[0], exist_ok=True)
    config = parse_yaml_config(args.config_file)
    plot_binaries(args, config)
