import os
import numpy as np
import matplotlib.pyplot as plt
from utils.utils_matplotlib import set_x_ticks, set_y_ticks, bar_chart

input_file = 'd:/repositories/Simulator/experiments/embedded_synchains/final_ro/exp_2000n_1000_chains_gaussian_from_min_no_limit_new/stdout.txt'
output_folder = 'd:/repositories/Simulator/experiments/embedded_synchains/final_ro/exp_2000n_1000_chains_gaussian_from_min_no_limit_new'


with open(input_file, 'r') as fp:
    lines = fp.readlines()
    lines = [line.strip() for line in lines]


chains_n_layers = []
n_layers = 0

for i in range(len(lines)):
    line = lines[i]

    # check if start of new layer
    if 'Building chain' in line:
        # every chain has at least 1 layer
        if n_layers != 0:
            chains_n_layers.append(n_layers)
        n_layers = 0
    # check if the chain continues
    elif 'Build layer with' in line:
        n_layers += 1
    else:
        if n_layers != 0:
            chains_n_layers.append(n_layers)
        break

chains_n_layers = np.array(chains_n_layers)

n_layers_values = []
n_layers_count = []

n_layers_min = chains_n_layers.min()
n_layers_max = chains_n_layers.max()

for i in range(n_layers_min, n_layers_max + 1):
    n_layers_values.append(i)
    n_layers_count.append((chains_n_layers == i).sum())

n_layers_values = np.array(n_layers_values)
n_layers_count = np.array(n_layers_count)

print(n_layers_values)
print(n_layers_count)
print(np.sum(n_layers_count))

# make percentages
percentages = []
total_chains = np.sum(n_layers_count)
for i in range(n_layers_count.shape[0]):
    percentages.append(str(n_layers_count[i] / total_chains) + '%')

bar_chart(os.path.join(output_folder, 'layers_distribution.png'), n_layers_values, n_layers_count, percentages, x_label='Numar nivele', y_label='Numar aparitii')
with open(os.path.join(output_folder, 'layers_distribution.txt'), 'w') as fp:
    fp.write('N_layers - Occurances - Percentage\n')
    for i in range(n_layers_values.shape[0]):
        fp.write(str(n_layers_values[i]) + ' - ' + str(n_layers_count[i]) + ' - ' + percentages[i] + '\n')
    print('Total Lenghts: ' + str(n_layers_values.shape[0]) + '\n')
    print('Total Chains: ' + str(total_chains) + '\n')