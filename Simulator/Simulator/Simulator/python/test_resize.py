import cv2 as cv
import os
import argparse


parser = argparse.ArgumentParser()
parser.add_argument('--common_path', type=str, required=True)
args = parser.parse_args()

out_folder = os.path.join(args.common_path, 'resized')
os.makedirs(out_folder, exist_ok=True)

# get all pngs
pngs = [f for f in os.listdir(args.common_path) if f.endswith('.png')]

for png in pngs:
    png_path = os.path.join(args.common_path, png)
    png_image = cv.imread(png_path, -1)

    png_image = cv.resize(png_image, (png_image.shape[1] // 2, png_image.shape[0] // 2))

    dest_image = os.path.join(out_folder, png)
    cv.imwrite(dest_image, png_image)
