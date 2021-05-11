import numpy as np
import argparse
from tqdm import tqdm
import os
import yaml
import sys
sys.path.append(os.path.join(os.path.realpath(__file__).split('python')[0], 'python'))

from plotting.plot_space_synfire_chain import get_variables_values
from plotting.plot_space_synfire_chain import get_synfire_state

from utils.utils_config import parse_yaml_config
from utils.utils_matplotlib import scatter_plot_with_colorscheme


def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--config_file', type=str, required=True,
                        help='Path to the config file for the plot')
    '''
    Config file format: YAML
    Needed keys:
        trials_folder: absolute path to the folder that contains the trials, trials used to compute the probabilities
        variable1: name of the first variable to consider, like connectivity
        variable2: name of the second variable to consider, like strength

    Note: the variable1 and variable2 values need to be encoded in the experiment file name like this
        variable1_value_variable2_value.txt
    '''
    return parser.parse_args()


def run_plot_space_synfire_chain(trial_folder, config):
    # build the configuration
    plot_config = dict()
    plot_config['experiments_folder'] = trial_folder
    plot_config['variable1'] = config['variable1']
    plot_config['variable2'] = config['variable2']
    plot_config['title'] = config['variable1'] + '_vs_' + config['variable2']

    # create the config file
    config_file = os.path.join(trial_folder + '_config.yml')
    with open(config_file, 'w') as fp:
        yaml.dump(plot_config, fp)

    # call the plotter
    os.system('plot_space_synfire_chain.py --config_file {}'.format(config_file))

    os.remove(config_file)


if __name__ == '__main__':
    args = get_args()
    config = parse_yaml_config(args.config_file)

    trials_folder_elements = [os.path.join(config['trials_folder'], f) for f in os.listdir(config['trials_folder'])]
    trials_folder = [f for f in trials_folder_elements if os.path.isdir(f) is True]

    x_values = []
    y_values = []
    state_values = []

    trails_info = {}
    for trial_folder in tqdm(trials_folder):

        # run the space activity for this trial
        run_plot_space_synfire_chain(trial_folder, config)

        # get the txts
        trial_folder_elements = [os.path.join(trial_folder, f) for f in os.listdir(trial_folder)]
        trail_txt_files = [f for f in trial_folder_elements if os.path.isfile(f) is True and f.endswith('.txt')]

        for txt_file in trail_txt_files:
            variable_values = get_variables_values(txt_file=os.path.basename(txt_file),
                                                   variable1_name=config['variable1'],
                                                   variable2_name=config['variable2'])


            state = get_synfire_state(os.path.join(config['trials_folder'], txt_file))
            x_values.append(variable_values[config['variable1']])
            y_values.append(variable_values[config['variable2']])
            state_values.append(state)

            x_value = variable_values[config['variable1']]
            y_value = variable_values[config['variable2']]
            current_key = str(x_value) + '_' + str(y_value)
            if current_key in trails_info.keys():
                if state == 'STABLE':
                    trails_info[current_key]['stable_cnt'] += 1
                elif state == 'NO_ACTIVITY':
                    trails_info[current_key]['no_activity_cnt'] += 1
                elif state == 'EPILEPSY':
                    trails_info[current_key]['epilepsy_cnt'] += 1
                else:
                    trails_info[current_key]['unknown_cnt'] += 1
                trails_info[current_key]['cnt'] += 1
            else:
                trails_info[current_key] = {
                    'x': x_value,
                    'y': y_value,
                    'stable_cnt': 1 if state == 'STABLE' else 0,
                    'no_activity_cnt': 1 if state == 'NO_ACTIVITY' else 0,
                    'epilepsy_cnt': 1 if state == 'EPILEPSY' else 0,
                    'unknown_cnt': 1 if state == 'UNKNOWN' else 0,
                    'cnt': 1
                }

    n_points = len(trails_info.keys())
    x_values = np.zeros((n_points,))
    y_values = np.zeros((n_points,))
    stable_probs = np.zeros((n_points,))
    epilepsy_probs = np.zeros((n_points,))
    no_activity_probs = np.zeros((n_points,))
    unknown_probs = np.zeros((n_points,))

    for i, key in enumerate(trails_info.keys()):
        point_data = trails_info[key]

        x_values[i] = point_data['x']
        y_values[i] = point_data['y']
        stable_probs[i] = point_data['stable_cnt'] / point_data['cnt']
        epilepsy_probs[i] = point_data['epilepsy_cnt'] / point_data['cnt']
        no_activity_probs[i] = point_data['no_activity_cnt'] / point_data['cnt']
        unknown_probs[i] = point_data['unknown_cnt'] / point_data['cnt']

    sep = config['csv_sep']
    with open(os.path.join(config['trials_folder'], 'values.csv'), 'w') as fp:
        fp.write(str(config['variable1']) + sep + str(config['variable2']) + sep + 'No_Activity' + sep + 'Stable' + sep + 'Epilepsy' + sep + 'Unknown\n')
        for i in range(n_points):
            fp.write(str(x_values[i]) + sep +
                     str(y_values[i]) + sep +
                     str(no_activity_probs[i]) + sep +
                     str(stable_probs[i]) + sep +
                     str(epilepsy_probs[i]) + sep +
                     str(unknown_probs[i]) + '\n')

    scatter_plot_with_colorscheme(os.path.join(config['trials_folder'], 'stable_p.png'),
                                  x_values, y_values, stable_probs,
                                  title='Probability of stable activity',
                                  x_label=config['variable1'],
                                  y_label=config['variable2'])

    scatter_plot_with_colorscheme(os.path.join(config['trials_folder'], 'epilepsy_p.png'),
                                  x_values, y_values, epilepsy_probs,
                                  title='Probability of epileptic activity',
                                  x_label=config['variable1'],
                                  y_label=config['variable2'])

    scatter_plot_with_colorscheme(os.path.join(config['trials_folder'], 'no_activity_p.png'),
                                  x_values, y_values, no_activity_probs,
                                  title='Probability of no activity',
                                  x_label=config['variable1'],
                                  y_label=config['variable2'])

    scatter_plot_with_colorscheme(os.path.join(config['trials_folder'], 'unknown_p.png'),
                                  x_values, y_values, unknown_probs,
                                  title='Probability of unknown',
                                  x_label=config['variable1'],
                                  y_label=config['variable2'])
