#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Jul  1 13:18:01 2024

@author: marco
"""

import pandas as pd
import matplotlib.pyplot as plt
import numpy as np



df = pd.read_csv("/home/marco/Documents/Pineapple/O/outputX1.txt", delimiter="\t")
X, Y, Z = df["x"], df["y"], df["z"]

from mayavi import mlab

# Create a sphere
r = 1.0
pi = np.pi
cos = np.cos
sin = np.sin
phi, theta = np.mgrid[0:pi:101j, 0:2 * pi:101j]

x = r*sin(phi)*cos(theta)
y = r*sin(phi)*sin(theta)
z = r*cos(phi)

mlab.figure(1, bgcolor=(1, 1, 1), fgcolor=(0, 0, 0), size=(400, 300))
mlab.clf()

data = np.genfromtxt('leb.txt')
xx, yy, zz = np.hsplit(data, 3)


mlab.mesh(x , y , z, color=(0.0,0.5,0.5))
mlab.points3d(xx, yy, zz, scale_factor=0.05)


mlab.show()