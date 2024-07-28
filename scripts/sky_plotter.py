#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Jul 28 19:14:28 2024

@author: marco
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from natsort import os_sorted

import os
# assign directory
directory = '../'
f="/home/marco/Documents/pineapple_problem/sky_data/2024-07-28_19-37-42.txt"
df = pd.read_csv(f,delimiter="\t")

plt.plot(df["t"], 90 - df["z"]*180/np.pi)
plt.plot(df["t"], df["A"] * 180 / np.pi)

plt.plot(df["t"], df["t"] * 0)
plt.plot(df["t"], df["t"] * 0 + 90)
plt.show()


plt.plot(df["t"])
plt.show()

A = 47000
B = 157000

plt.plot(df["t"][A:B], df["theta"][A:B])
plt.show()

plt.plot(df["A"][A:B]*180/np.pi, 90 - df["z"][A:B]*180/np.pi)
plt.ylabel("Elevation (degree)")
plt.xlabel("Azimuth (degree)")
#plt.ylim(0, 90)