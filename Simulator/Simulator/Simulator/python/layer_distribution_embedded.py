import os
import numpy as np
import matplotlib.pyplot as plt
from utils.utils_matplotlib import set_x_ticks, set_y_ticks

input_file = 'd:/repositories/Simulator/experiments/embedded_synchains/final_ro/exp_2000n_1000_chains_gaussian_from_min_no_limit/stdout.txt'
output_folder = 'd:/repositories/Simulator/experiments/embedded_synchains/final_ro/exp_2000n_1000_chains_gaussian_from_min_no_limit'


def bar_chart(output_file, x_values, y_values, bar_values):
    x_values_min = x_values.min()
    x_values_max = x_values.max()
    plt.figure(figsize=(15, 10))
    set_x_ticks(n_layers_min, n_layers_max, n_ticks=x_values_max - x_values_min)
    set_y_ticks(0, n_layers_count.max())
    plt.xlabel('Numar nivele', fontsize=25)
    plt.ylabel('Numar aparitii', fontsize=25)

    graph = plt.bar(x_values, y_values)

    i = 0
    for p in graph:
        width = p.get_width()
        height = p.get_height()
        x, y = p.get_xy()

        plt.text(x + width/2,
                 y + height * 1.01,
                 str(bar_values[i]),
                 ha='center',
                 weight='bold',
                 fontdict={'size':13})
        i += 1

    plt.tight_layout()
    plt.savefig(output_file, dpi=200)


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

bar_chart(os.path.join(output_folder, 'layers_distribution.png'), n_layers_values, n_layers_count, percentages)
with open(os.path.join(output_folder, 'layers_distribution.txt'), 'w') as fp:
    fp.write('N_layers - Occurances - Percentage\n')
    for i in range(n_layers_values.shape[0]):
        fp.write(str(n_layers_values[i]) + ' - ' + str(n_layers_count[i]) + ' - ' + percentages[i] + '\n')
    print('Total Lenghts: ' + str(n_layers_values.shape[0]) + '\n')
    print('Total Chains: ' + str(total_chains) + '\n')