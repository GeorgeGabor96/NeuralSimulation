import numpy as np
import os
import matplotlib.pyplot as plt
import cv2 as cv


def reset_plot():
    plt.clf()
    plt.cla()
    plt.close()


class NetworkSpikesPlot:
    def __init__(self, output_file):
        reset_plot()
        plt.figure(figsize=(15, 10))
        self.output_file = output_file

    def plot_points(self, x_coord, y_coord):
        plt.scatter(x_coord, y_coord, color='black', s=0.1)

    def plot_line(self, x_coord, y_coord):
        plt.plot(x_coord, y_coord, color='black')

    def plot(self):
        plt.axis('scaled')
        os.makedirs(os.path.split(self.output_file)[0], exist_ok=True)
        plt.savefig(self.output_file, dpi=100)
        reset_plot()


class NetworkSpikesPlotImproved:
    def __init__(self, output_file):
        self.output_file = output_file
        self.subplots = []

    def plot_layer(self, layer_name, layer_data):
        self.subplots.append(dict(subplot_name=layer_name, subplot_data=layer_data))

    def plot(self):
        text_space_width = 100
        line = np.hstack((np.zeros((1, text_space_width)), np.ones((1, self.subplots[0]['subplot_data'].shape[1]))))
        plot = line
        for subplot in self.subplots:
            n_rows = subplot['subplot_data'].shape[0]
            subplot_np = np.hstack((np.zeros((n_rows, text_space_width)), subplot['subplot_data']))
            cv.putText(subplot_np, subplot['subplot_name'], (0, int(n_rows/2)), cv.FONT_HERSHEY_SIMPLEX, 0.5, (1, 1, 1), 1)

            plot = np.vstack((plot, subplot_np, line))
        plot[:, text_space_width - 1] = 1
        plot = np.hstack((plot, np.zeros((plot.shape[0], 50))))
        plot[:, -49] = 1

        # add x axis numbers
        x_step = self.subplots[0]['subplot_data'].shape[1] / 10.0
        x_values = np.arange(start=0, stop=self.subplots[0]['subplot_data'].shape[1] + x_step, step=x_step)
        text_start_y = plot.shape[0] + 20
        text_start_x = text_space_width
        plot = np.vstack((plot, np.zeros((50, plot.shape[1]))))
        for i in range(x_values.shape[0]):
            cv.putText(plot, str(int(x_values[i])), (text_start_x, text_start_y), cv.FONT_HERSHEY_SIMPLEX, 0.5, (1, 1, 1), 1)
            text_start_x += int(x_step)

        plot_c = plot.copy()
        plot_c[plot == 1] = 0
        plot_c[plot == 0] = 1

        cv.imwrite(self.output_file, plot_c * 255)


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
