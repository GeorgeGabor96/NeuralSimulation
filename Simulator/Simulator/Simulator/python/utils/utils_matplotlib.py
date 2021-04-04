import numpy as np
import os
import matplotlib.pyplot as plt


def reset_plot():
    plt.clf()
    plt.cla()
    plt.close()


class NetworkSpikesPlot:
    def __init__(self, output_file, title):
        reset_plot()
        plt.figure(figsize=(15, 10))
        self.output_file = output_file
        self.title = title
        self.xlabel = 'Time (ms)'
        self.ylabel = 'Layers'

        self.layer_names = []
        self.y_ticks = []

    def plot_points(self, layer_name, label_tick, x_coord, y_coord):
        plt.scatter(x_coord, y_coord, color='black', s=0.5)
        self.y_ticks.append(label_tick)
        self.layer_names.append(layer_name)

    def plot_line(self, x_coord, y_coord):
        plt.plot(x_coord, y_coord, color='black')

    def plot(self):
        plt.xlabel(self.xlabel)

        plt.ylabel(self.ylabel)
        plt.yticks(ticks=self.y_ticks, labels=self.layer_names)

        plt.title(self.title)

        plt.axis('tight')
        os.makedirs(os.path.split(self.output_file)[0], exist_ok=True)
        plt.savefig(self.output_file, dpi=100)
        reset_plot()


def line_plot(output_file, y_data, line_label, x_label, y_label, title):
    plt.figure(figsize=(15, 10))

    if isinstance(y_data, list) is False:
        y_data = [y_data]
    if isinstance(line_label, list) is False:
        line_label = [line_label]

    x_min = 1e+15
    x_max = -1e+15
    y_min = 1e+15
    y_max = -1e+15

    for i in range(len(y_data)):
        x_data = np.arange(y_data[i].shape[0])
        plt.plot(x_data, y_data[i], label=line_label[i])

        x_data_min = x_data.min()
        x_data_max = x_data.max()
        y_data_min = y_data[i].min()
        y_data_max = y_data[i].max()

        x_min = x_data_min if x_data_min < x_min else x_min
        x_max = x_data_max if x_data_max > x_max else x_max
        y_min = y_data_min if y_data_min < y_min else y_min
        y_max = y_data_max if y_data_max > y_max else y_max

    x_step = (x_max - x_min) / 40.0
    if x_step != 0.0:
        plt.xticks(np.arange(start=x_min, stop=x_max + x_step, step=x_step), rotation=90)

    y_step = (y_max - y_min) / 30.0
    if y_step != 0.0:
        plt.yticks(np.arange(start=y_min, stop=y_max + y_step, step=y_step))

    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')
    plt.grid(True, linestyle='--')
    plt.title(title, fontsize=20)
    plt.xlabel(x_label, fontsize=20)
    plt.ylabel(y_label, fontsize=20)
    plt.tight_layout()

    plt.savefig(output_file, dpi=100)

    reset_plot()
