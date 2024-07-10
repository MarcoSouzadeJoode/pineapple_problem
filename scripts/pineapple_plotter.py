#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Jun 30 22:42:52 2024

@author: marco
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from natsort import os_sorted

import os
# assign directory
directory = '../data'


# iterate over files in
# that directory
for filename in os_sorted(os.listdir(directory))[:]:
    f = os.path.join(directory, filename)
    # checking if it is a file
    if os.path.isfile(f) and not f.endswith("metadata"):
        print(f)
        
        df = pd.read_csv(f,delimiter="\t")

        #plt.xlim(-0.02, 0.04)
        fig, (ax2, ax3) = plt.subplots(nrows=2, ncols=1)
        
        #number_part = f.split('_')[1].split('.')[0]

        #ax2.set_title(f"v = {float(number_part):.2f} m/s")
        #plt.plot(df["phi"] * 180 / np.pi, df["theta"] * 180 / np.pi)
        #ax1.scatter(df["t"] / 86400, df["phi"] * 180 / np.pi, s=0.001, c="k")
        
        ax2.plot(df["t"] / 86400 / 365.25, df["theta"] * 180 / np.pi, c="k", label="Walker")
        
        ax2.plot(df["t"] / 86400 / 365.25, 90 - (df["thetas"] * 180 / np.pi), linewidth=1, c="red")
        ax2.plot(df["t"] / 86400 / 365.25, -90 - (df["thetas"] * 180 / np.pi), linewidth=1, c="red", label="p. day")
        ax2.plot(df["t"] / 86400 / 365.25, 90 + (df["thetas"] * 180 / np.pi), linewidth=1, c="blue")
        ax2.plot(df["t"] / 86400 / 365.25, -90 + (df["thetas"] * 180 / np.pi), linewidth=1, c="blue", label="p. night",)
        
        ax2.plot(df["t"] / 86400 / 365.25, df["thetas"] * 180 / np.pi, linewidth=1, c="orange", label="Sun")
        ax2.legend()

        ax3.plot( df["lambda"] * 180 / np.pi,df["theta"] * 180 / np.pi,linewidth=0.1 ,c="k")
        
        ax3.plot( df["lambdas"] * 180 / np.pi,df["thetas"] * 180 / np.pi, linewidth=1,alpha=0.5, c="orange")
        #2.legend()
        """
        ax1.set_xlim(0,12)
        ax2.set_xlim(0,12)
        
        ax1.set_ylim(-170, 0)
        ax2.set_ylim(-2, 90)
        
        ax3.set_xlim(-170, 0)
        ax3.set_ylim(-2, 90)
        """
        
        ax2.set_ylim(-90, 90)
        #ax2.set_xlim(0, 0.2)
        #ax3.set_ylim(-90, 90)

        #ax2.hlines(-23.5, 0, 365, color="r")
        #ax2.hlines(23.5, 0, 365, color="r")
        
        
        ax3.hlines(-23.5, -180, 180, color="r")
        ax3.hlines(23.5, -180, 180, color="r")
        ax3.set_xlim(-180, 180)
        #ax1.set_ylabel("Longitude (deg)")
        ax2.set_ylabel("Latitude (deg)")
        
        #ax1.set_xlabel("Time (days)")
        ax2.set_xlabel("Time (years)")
        
        ax3.set_xlabel("Longitude (deg)")
        ax3.set_ylabel("Latitude (deg)")
        plt.tight_layout()
        plt.savefig(f"../plots/{filename}.plot.png", dpi=200)
        plt.show()
