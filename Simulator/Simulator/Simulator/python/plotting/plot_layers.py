import argparse
import os
import sys
sys.path.append(os.path.join(os.path.realpath(__file__).split('python')[0], 'python'))
from utils.utils_os import get_file_with_extension
from utils.utils_simulator import parse_array_file
from utils.utils_matplotlib import line_plot


def get_args():
    parser = argparse.ArgumentParser()
    parser.add_argument('--dump_folder', type=str, required=True,
                        help='Folder that has dumps of arrays')
    return parser.parse_args()


if __name__ == '__main__':
    args = get_args()
    bin_files = get_file_with_extension(args.dump_folder, 'bin')

    for bin_file in bin_files:
        sim_array_aux = parse_array_file(bin_file)

        # for romanian
        sim_array = dict()
        if sim_array_aux['name'] == 'voltage':
            sim_array['name'] = 'u (mV)'
        elif sim_array_aux['name'] == 'spikes':
            sim_array['name'] = 'descarcari'
        elif sim_array_aux['name'] == 'PSC':
            sim_array['name'] = 'CPS (nA)'
        elif sim_array_aux['name'] == 'IPSC':
            sim_array['name'] = 'CPSI (nA)'
        elif sim_array_aux['name'] == 'EPSC':
            sim_array['name'] = 'CPSE (nA)'
        sim_array['data'] = sim_array_aux['data']
        n_times = 201

        output_folder, file_name = os.path.split(bin_file)
        file_name = file_name.replace('bin', 'png')

        line_plot(output_file=os.path.join(output_folder, file_name),
                  y_data=[sim_array['data'][:n_times]],
                  line_label=None,  #[sim_array['name']],
                  x_label='t (ms)',
                  y_label=sim_array['name'],
                  title=None)  #file_name.split('.')[0])
