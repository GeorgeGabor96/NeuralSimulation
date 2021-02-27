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
        sim_array = parse_array_file(bin_file)

        output_folder, file_name = os.path.split(bin_file)
        file_name = file_name.replace('bin', 'png')

        line_plot(output_file=os.path.join(output_folder, file_name),
                  y_data=sim_array['data'],
                  x_label='time',
                  y_label=sim_array['name'],
                  title=file_name.split('.')[0])
