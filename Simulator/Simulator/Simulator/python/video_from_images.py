import os
import cv2 as cv


path_images = ['D:/repositories/Simulator\experiments/rerun_probability_plots/exp_5_60_40_refract/stable_p.png',
               'D:/repositories/Simulator\experiments/rerun_probability_plots/exp_6_70_30_refract/stable_p.png',
               'D:/repositories/Simulator/experiments/rerun_probability_plots/exp_5_75_25_refract/stable_p.png',
               'D:/repositories/Simulator/experiments/rerun_probability_plots/exp_1_80_20_refract/stable_p.png',
               'D:/repositories/Simulator/experiments/rerun_probability_plots/exp_5_85_15_refract/stable_p.png',
               'D:/repositories/Simulator/experiments/rerun_probability_plots/exp_3_90_10_refract/stable_p.png',
               'D:/repositories/Simulator/experiments/rerun_probability_plots/exp_5_95_5_refract/stable_p.png',
               'D:/repositories/Simulator/experiments/rerun_probability_plots/exp_4_100_0_refract/stable_p.png']

'''
path_images = ['D:/repositories/Simulator\experiments/rerun_probability_plots/exp_5_60_40_refract/epilepsy_p.png',
               'D:/repositories/Simulator\experiments/rerun_probability_plots/exp_6_70_30_refract/epilepsy_p.png',
               'D:/repositories/Simulator/experiments/rerun_probability_plots/exp_5_75_25_refract/epilepsy_p.png',
               'D:/repositories/Simulator/experiments/rerun_probability_plots/exp_1_80_20_refract/epilepsy_p.png',
               'D:/repositories/Simulator/experiments/rerun_probability_plots/exp_5_85_15_refract/epilepsy_p.png',
               'D:/repositories/Simulator/experiments/rerun_probability_plots/exp_3_90_10_refract/epilepsy_p.png',
               'D:/repositories/Simulator/experiments/rerun_probability_plots/exp_5_95_5_refract/epilepsy_p.png',
               'D:/repositories/Simulator/experiments/rerun_probability_plots/exp_4_100_0_refract/epilepsy_p.png']
'''

images = []
for path_image in path_images:
    images.append(cv.imread(path_image))

video = cv.VideoWriter('D:/repositories/Simulator/experiments/rerun_probability_plots/stable_transition.avi', cv.VideoWriter_fourcc(*'mp4v'), 0.3, (images[0].shape[1], images[0].shape[0]))

for image in images:
    video.write(image)

cv.destroyAllWindows()
video.release()