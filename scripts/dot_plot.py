#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Jul  3 23:42:00 2024

@author: marco
"""

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
directory = 'vel'
 
# iterate over files in
# that directory
for filename in os_sorted(os.listdir(directory))[:]:
    f = os.path.join(directory, filename)
    # checking if it is a file
    if os.path.isfile(f):
        print(f)
        
        df = pd.read_csv(f,delimiter="\t")
        DOT = np.arccos(df["x"]*df["xs"] + df["y"]*df["ys"] + df["z"]*df["zs"]) * 180 / np.pi
        plt.plot(df["t"] / 86400, DOT,linewidth=0.5,c="k")
        plt.xlabel("Time (days)")
        plt.ylabel("Solar zenith distance (deg)")