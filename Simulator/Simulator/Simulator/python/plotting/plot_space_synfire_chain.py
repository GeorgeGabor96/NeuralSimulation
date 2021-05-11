import argparse
import os
import sys
sys.path.append(os.path.join(os.path.realpath(__file__).split('python')[0], 'python'))
from tqdm import tqdm
import numpy as np

from utils.utils_config import parse_yaml_config
from utils.utils_matplotlib import scatter_plot


def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--config_file', type=str, required=True,
                        help='Path to the config file for the plot')
    '''
    Config file format: YAML
    Needed keys:
        experiments_folder: absolute path to the folder that contains a txt file for each experiment
        variable1: name of the first variable to consider, like connectivity
        variable2: name of the second variable to consider, like strength
        title: title of the plot

    Note: the variable1 and variable2 values need to be encoded in the experiment file name like this
        variable1_value_variable2_value.txt
    '''
    return parser.parse_args()


def get_variables_values(txt_file, variable1_name, variable2_name):
    name_without_extension = txt_file.replace('.txt', '')
    tokens = name_without_extension.split('_')

    # should have 4 values
    assert len(tokens) == 4, 'expect 2 tokens for the variable names and 2 for the values'
    assert tokens[0] == variable1_name, 'first token should be {} not {}'.format(variable1_name, tokens[0])
    assert tokens[2] == variable2_name, 'first token should be {} not {}'.format(variable2_name, tokens[2])

    return { variable1_name: float(tokens[1]), variable2_name: float(tokens[3]) }


def get_synfire_state(txt_file):
    with open(txt_file, 'r') as fp:
        state = fp.readline().strip()
    return state


if __name__ == '__main__':
    args = get_args()
    config = parse_yaml_config(args.config_file)

    # find all txt files
    txt_files = [f for f in os.listdir(config['experiments_folder'])
                 if os.path.isfile(os.path.join(config['experiments_folder'], f)) is True and f.endswith('.txt')]

    points_for_state = {
        'NO_ACTIVITY': dict(x=[], y=[]),
        'STABLE': dict(x=[], y=[]),
        'EPILEPSY': dict(x=[], y=[]),
        'UNKNOWN': dict(x=[], y=[])
    }

    for txt_file in tqdm(txt_files):
        variable_values = get_variables_values(txt_file=txt_file,
                                               variable1_name=config['variable1'],
                                               variable2_name=config['variable2'])

        state = get_synfire_state(os.path.join(config['experiments_folder'], txt_file))

        points_for_state[state]['x'].append(variable_values[config['variable1']])
        points_for_state[state]['y'].append(variable_values[config['variable2']])

    for state in points_for_state.keys():
        points_for_state[state]['x'] = np.array(points_for_state[state]['x'])
        points_for_state[state]['y'] = np.array(points_for_state[state]['y'])

    scatter_plot(output_file=os.path.join(config['experiments_folder'], 'synfire_chain_space_plot.png'),
                 groups=points_for_state,
                 title=config['title'],
                 x_label=config['variable1'],
                 y_label=config['variable2'])
