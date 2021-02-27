import numpy as np
import matplotlib.pyplot as plt


def line_plot(output_file, y_data, x_label, y_label, title):
    x_data = np.arange(y_data.shape[0])

    plt.title(title)
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    plt.plot(x_data, y_data)
    plt.savefig(output_file, dpi=300)
    plt.close()
