import numpy as np
import os
import matplotlib.pyplot as plt


def reset_plot():
    """
    Clears the current plot so we can start from a fresh canvas
    """
    plt.clf()
    plt.cla()
    plt.close()


def set_x_ticks(x_min, x_max, n_ticks=40, rotation=90):
    """
    :param x_min: float
        the minimum value for the x axis

    :param x_max: float
        the maximum value for the x axis

    :param n_ticks: int, default 40
        the number of ticks to set between [x_min, x_max]

    :param rotation: int or float, default 90
        the rotation of the x ticks values on the plot
    """
    x_step = (x_max - x_min) / float(n_ticks)
    if x_step != 0.0:
        plt.xticks(np.arange(start=x_min, stop=x_max + x_step, step=x_step), rotation=rotation)


def set_y_ticks(y_min, y_max, n_ticks=30, rotation=0):
    """
    :param y_min: float
        the minimum value for the y axis

    :param y_max: float
        the maximum value for the y axis

    :param n_ticks: int
        the number of ticks to set between [y_min, y_max]

    :param rotation: int or float
        the rotation of the y ticks values on the plot
    """
    y_step = (y_max - y_min) / float(n_ticks)
    if y_step != 0.0:
        plt.yticks(np.arange(start=y_min, stop=y_max + y_step, step=y_step), rotation=rotation)


def set_legend():
    """
    Sets the legend outside the figure in the upper left corner
    """
    plt.legend(bbox_to_anchor=(1.05, 1), loc='upper left')


def get_min_max(c_min, c_max, array):
    """
    Given a current minimum and a current maximum and an array it will return
    the min(c_min, array.min()), max(c_max, array.max())

    :param c_min: float
        current minimum value

    :param c_max: float
        current maximum value

    :param array: np array
        array of values

    :return: min(c_min, array.min()), max(c_max, array.max())
    """
    return min(c_min, array.min()), max(c_max, array.max())


class NetworkSpikesPlot:
    def __init__(self, output_file, title, x_label='Time (ms)', y_label='Layers', scatter=True):
        """
        This class creates a plot where the x axis represents time and the y represents layers
        It knows how to plot a layer and if necessary lines between them

        It works iteratively by plotting a layer at a time

        :param output_file: str
            path to the output file for the plot

        :param title: str
            title of the plot
        """
        reset_plot()
        plt.figure(figsize=(15, 10))
        self.scatter = scatter
        self.output_file = output_file
        self.title = title
        self.xlabel = x_label
        self.ylabel = y_label
        self.min_x_value = 1e+15
        self.max_x_value = -1e+15

        self.layer_names = []
        self.y_ticks = []

    def plot_points(self, layer_name, label_tick, x_coord, y_coord, color='black'):
        """
        This plots a layer

        :param layer_name: str
            name of the layer

        :param label_tick: int or float
            position on the y axis where to put the label of the layer

        :param x_coord: np array
            shape: 1D
            x values for the layer

        :param y_coord: np array
            shape: 1D
            y values for the layer, it expects the exact coordinates, the user is responsible for that
        """
        if self.scatter is True:
            plt.scatter(x_coord, y_coord, color=color, s=0.5)
        else:
            plt.plot(x_coord, y_coord, color=color)
        self.y_ticks.append(label_tick)
        self.layer_names.append(layer_name)

    def plot_line(self, x_coord, y_coord):
        """
        Plots a line

        :param x_coord: np array or list
            shape: (2,)
            the x values for the first and second point of the line

        :param y_coord: np array or list
            shape: (2,)
            the y values for the first and second point of the line
        """
        plt.plot(x_coord, y_coord, color='black')
        min_x_value = min(x_coord) if isinstance(x_coord, list) else x_coord.min()
        max_x_value = max(x_coord) if isinstance(x_coord, list) else x_coord.max()
        if min_x_value < self.min_x_value:
            self.min_x_value = min_x_value
        if max_x_value > self.max_x_value:
            self.max_x_value = max_x_value

    def plot(self):
        """
        Plots the network activity plot based on the info gathered in the for the @plot_line and @plot_points
        """
        plt.xlabel(self.xlabel)
        set_x_ticks(self.min_x_value, self.max_x_value)

        plt.ylabel(self.ylabel)
        plt.yticks(ticks=self.y_ticks, labels=self.layer_names)

        plt.title(self.title)

        plt.axis('tight')
        os.makedirs(os.path.split(self.output_file)[0], exist_ok=True)
        plt.tight_layout()
        plt.savefig(self.output_file, dpi=200)
        reset_plot()


def line_plot(output_file, y_data, x_label, y_label, title, x_data=None, line_label=None):
    """
    Plots multiple lines on the same plot with different colors

    :param output_file: str
        path to the file where to save the plot

    :param y_data: list or np array
        if np array it will be put in a list
        each element represents the y values for one line

    :param x_label: str
        label for the x axis

    :param y_label: str
        label for the y axis

    :param title: str
        title of the plot

    :param x_data: list or np array or None
        if None the x values will be generated using the np.arange function
        if np array it will be put in a list
        each element represents the x values for one line

    :param line_label: list or str or None
        if None the lines will not have any labels
        if str it will be put in a list
        each element represents the label for one line
    """
    plt.figure(figsize=(15, 10))

    if isinstance(y_data, list) is False:
        y_data = [y_data]
    if x_data is None:
        x_data = []
        for i in range(len(y_data)):
            x_data.append(np.arange(y_data[i].shape[0]))
    elif isinstance(x_data, list) is False:
        x_data = [x_data]
    if line_label is not None and isinstance(line_label, list) is False:
        line_label = [line_label]

    x_min = 1e+15
    x_max = -1e+15
    y_min = 1e+15
    y_max = -1e+15

    for i in range(len(y_data)):
        if line_label is not None:
            plt.plot(x_data[i], y_data[i], label=line_label[i])
        else:
            plt.plot(x_data[i], y_data[i])

        if x_data[i].shape[0] != 0:
            x_min, x_max = get_min_max(x_min, x_max, x_data[i])
            y_min, y_max = get_min_max(y_min, y_max, y_data[i])

    set_x_ticks(x_min, x_max)
    set_y_ticks(y_min, y_max)
    if line_label is not None:
        set_legend()

    plt.grid(True, linestyle='--')
    plt.title(title, fontsize=20)
    plt.xlabel(x_label, fontsize=20)
    plt.ylabel(y_label, fontsize=20)
    plt.tight_layout()

    plt.savefig(output_file, dpi=100)

    reset_plot()


def scatter_plot(output_file, groups, title, x_label, y_label):
    """
    This will plot multiple groups of points, each with a different color

    :param output_file: str
        path to the file where to save the plot

    :param groups: dict
        each key represents the label for one group of points
        each values is a dictionary like:
        {
            'x': x coordinates for the x axis
            'y': y coordinates for the y axis
        }

    :param title: str
        title of the plot

    :param x_label: str
        label for the x axis

    :param y_label: str
        label for the y axis
    """
    plt.figure(figsize=(15, 10))

    x_min = 1e+15
    x_max = -1e+15
    y_min = 1e+15
    y_max = -1e+15

    for group_name in groups.keys():
        group_data = groups[group_name]

        x_data = group_data['x']
        y_data = group_data['y']

        if x_data.shape[0] == 0:
            continue

        plt.scatter(x_data, y_data, label=group_name)
        x_min, x_max = get_min_max(x_min, x_max, x_data)
        y_min, y_max = get_min_max(y_min, y_max, y_data)

    set_x_ticks(x_min, x_max)
    set_y_ticks(y_min, y_max)
    set_legend()

    plt.title(title, fontsize=20)
    plt.xlabel(x_label, fontsize=20)
    plt.ylabel(y_label, fontsize=20)
    plt.tight_layout()

    plt.savefig(output_file, dpi=100)

    reset_plot()


def scatter_plot_with_colorscheme(output_file, x_data, y_data, c_data, title, x_label, y_label, cmap='BuGn'):
    """
    This will create a 2D plot where every point will have a color on a colormap
    Usefull to show properties of points like probabilities

    :param output_file: str
        path to the file that will be the image

    :param x_data: np array
        1D array of x values

    :param y_data: np array
        1D array of y values

    :param c_data: np array
        1D array of values, represent 'indexes' of colors for the x, y points
        if 2 (x, y) points have the same c_data they will have the same color

    :param title: str
        title of the plot

    :param x_label: str
        label for the x axis

    :param y_label: str
        label for the y axis

    :param cmap: str
        matplotlib colorma name
    """
    plt.figure(figsize=(15, 10))

    x_min = x_data.min()
    x_max = x_data.max()
    y_min = y_data.min()
    y_max = y_data.max()

    plt.scatter(x_data, y_data, c=c_data, cmap=cmap)
    plt.colorbar()

    set_x_ticks(x_min, x_max)
    set_y_ticks(y_min, y_max)

    plt.title(title, fontsize=20)
    plt.xlabel(x_label, fontsize=20)
    plt.ylabel(y_label, fontsize=20)
    plt.tight_layout()

    plt.savefig(output_file, dpi=100)

    reset_plot()


