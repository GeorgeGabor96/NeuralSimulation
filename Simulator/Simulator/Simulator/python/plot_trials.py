import os
from plotting.network_activity.plot_multiple_network_spike_activity import plot_multiple

TRIALS_DIR = 'd:/repositories/Simulator/experiments/rerun_probability_plots/exp_normal_transition_1'
trials = [os.path.join(TRIALS_DIR, f) for f in os.listdir(TRIALS_DIR)]
trials = [f for f in trials if os.path.isdir(f)]


for trial in trials:
    print('For', trial)
    plot_multiple(trial)
