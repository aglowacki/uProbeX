from socket import AddressFamily
import h5py
import numpy as np
import cv2 as cv
import pandas as pd
from matplotlib import pyplot as plt
import argparse

parser = argparse.ArgumentParser(description='CV KMeans')
parser.add_argument('--file', type=str, default='Y:\\2ide\\2022-1\\XRF-Demo\\img.dat\\2xfm_0008.mda.h50', help='filename to run kmeans on')

cnts_names_loc = '/MAPS/XRF_Analyzed/NNLS/Channel_Names'
nnls_cnts_loc = '/MAPS/XRF_Analyzed/NNLS/Counts_Per_Sec'
matrix_cnts_loc = '/MAPS/XRF_Analyzed/Fitted/Counts_Per_Sec'
scaler_names_loc = '/MAPS/Scalers/Names'
scaler_values_loc = '/MAPS/Scalers/Values'


def get_element_maps(fname, get_scalers=False):
    f = h5py.File(fname, 'r')
    cnt_labels = None
    cnt_vals = None
    scaler_labels = None
    scaler_vals = None

    if cnts_names_loc in f:
        cnt_labels = f[cnts_names_loc]
    else:
        raise Exception("Can not find Channel Names")

    if nnls_cnts_loc in f:
        cnt_vals = f[nnls_cnts_loc]
    elif matrix_cnts_loc in f:
        cnt_vals = f[matrix_cnts_loc]
    else:
        raise Exception("Can not find NNLS or Matrix counts")
    
    cnt_dict = {}
    for i in range(len(cnt_labels)):
        cnt_dict[str(cnt_labels[i], 'UTF-8')] = cnt_vals[i, :, :]

    if get_scalers:
        if scaler_names_loc in f:
            scaler_labels = f[scaler_names_loc]
        else:
            raise Exception("Can not find scaler names")
        
        if scaler_values_loc in f:
            scaler_vals = f[scaler_values_loc]
        else:
            raise Exception("Can not find scaler values")
        
        for i in range(len(scaler_labels)):
            cnt_dict[str(scaler_labels[i], 'UTF-8')] = scaler_vals[:,:,i]
    
    return cnt_dict

def plot_element(el_name, cnt_dict):
    plt.imshow(cnt_dict[el_name])
    plt.show()
    plt.close()

def plot_2(img1, img2):
    fig, axs = plt.subplots(1, 2)
    axs[0].imshow(img1)
    axs[1].imshow(img2)
    plt.show()
    plt.close()

def plot_multi(el_names, cnt_dict):
    fig, axs = plt.subplots(1, len(el_names))
    for i in range(len(el_names)):
        axs[i].set_title(el_names[i])
        axs[i].imshow(cnt_dict[el_names[i]])
    plt.show()
    plt.close()

def apply_kmeans(img, nfeatures=2, plot=False):
    z = img.reshape((-1,1))
    # Define criteria = ( type, max_iter = 10 , epsilon = 1.0 )
    criteria = (cv.TERM_CRITERIA_EPS + cv.TERM_CRITERIA_MAX_ITER, 10, 1.0)
    # Set flags (Just to avoid line break in the code)
    flags = cv.KMEANS_RANDOM_CENTERS
    compactness,labels,centers = cv.kmeans(z, nfeatures, None, criteria, 10, flags)
    center = np.uint8(centers)
    #res = center[labels.flatten()]
    #res2 = res.reshape((img.shape))
    res2 = labels.reshape((img.shape))
    if plot:
        cv.imshow('res',res2)
        cv.waitKey()
        cv.destroyAllWindows()
    return res2
    

def main():
    opt = parser.parse_args()
    img_dict = get_element_maps(opt.file)
    #plot_element('Total_Fluorescence_Yield', img_dict)
    #plot_multi(['Fe', 'Cu', 'Total_Fluorescence_Yield'], img_dict)
    #mask = apply_kmeans(img_dict['Total_Fluorescence_Yield'])
    #plot_2(img_dict['Total_Fluorescence_Yield'], mask)
    mask = apply_kmeans(img_dict['Ca'], 3, False)
    plot_2(img_dict['Ca'], mask)



if __name__ == '__main__':
    main()