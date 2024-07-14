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

from datetime import datetime, timedelta

import os
# assign directory
directory = '../data'



start_date = datetime(2024, 9, 21)


def add_seconds_and_format(start_date, T):
    # Add T seconds to the start date
    new_date = start_date + timedelta(seconds=T)
    # Format the new date
    formatted_date = new_date.strftime('%Y-%m-%dT%H:%M:%S')
    return formatted_date


"""
core.setDate("2019-12-22T12:00:00", "UTC");
core.selectObjectByName("Sun", false);
core.setObserverLocation(50.1525, 15.000, 50);

"""

# iterate over files in
# that directory
for filename in os_sorted(os.listdir(directory))[-3:]:
    f = os.path.join(directory, filename)
    if os.path.isfile(f) and not f.endswith("metadata"):
        print(f)
        
        df = pd.read_csv(f,delimiter="\t")
        
        TIMES   = df["t"]
        LAMBDAS = df["lambda"] * 180 / np.pi
        THETAS  = df["theta"] * 180 / np.pi
        # /snap/stellarium-daily/1799/usr/share/stellarium/scripts/A.ssc
        
        with open("../stellarium_scripts/test_A.ssc", "w") as SSC:
            SSC.write('label = LabelMgr.labelScreen("Solar Pilgrimage", 20, 20, false, 50, "#aa0000");\n')
            SSC.write('LabelMgr.setLabelShow(label, true);\n')
            SSC.write('LandscapeMgr.setCurrentLandscapeID("ideal");')
            
            SSC.write(f'label1 = LabelMgr.labelScreen("LATITUDE = {THETAS[0]:.2f}", 20, 100, false, 30, "#aa0000");\n')
            SSC.write(f'label2 = LabelMgr.labelScreen("LONGITUDE = {LAMBDAS[0]:.2f}", 20, 150, false, 30, "#aa0000");\n')
            SSC.write(f'label3 = LabelMgr.labelScreen("TIME ELAPSED = {(TIMES[0]/86400):.2f} DAYS", 20, 200, false, 30, "#aa0000");\n')

            SSC.write('LabelMgr.setLabelShow(label1, true);\n')
            SSC.write('LabelMgr.setLabelShow(label2, true);\n')
            SSC.write('LabelMgr.setLabelShow(label3, true);\n')


            for i, t in enumerate(TIMES[1:1000:3]):
                fmt_date = add_seconds_and_format(start_date, t)
                SSC.write('core.clear("natural");\n')
                SSC.write('LabelMgr.deleteLabel(label1);\n')
                SSC.write('LabelMgr.deleteLabel(label2);\n')
                SSC.write('LabelMgr.deleteLabel(label3);\n')

                SSC.write(f'core.setDate("{fmt_date}", "UTC");\n')
                SSC.write(f'core.setObserverLocation({THETAS[i]}, {LAMBDAS[i]}, 0.0);\n')
                
                SSC.write(f'label1 = LabelMgr.labelScreen("LATITUDE = {THETAS[i]:.2f}", 20, 100, false, 30, "#aa0000");\n')
                SSC.write(f'label2 = LabelMgr.labelScreen("LONGITUDE = {LAMBDAS[i]:.2f}", 20, 150, false, 30, "#aa0000");\n')
                SSC.write(f'label3 = LabelMgr.labelScreen("TIME ELAPSED = {(TIMES[i]/86400):.2f} DAYS", 20, 200, false, 30, "#aa0000");\n')
                SSC.write('GridLinesMgr.setFlagAzimuthalGrid(true);\n')
                SSC.write('GridLinesMgr.setFlagMeridianLine(true);\n')
                SSC.write('LabelMgr.setLabelShow(label1, true);\n')
                SSC.write('LabelMgr.setLabelShow(label2, true);\n')
                SSC.write('LabelMgr.setLabelShow(label3, true);\n')


                SSC.write('core.wait(1)\n')
                
                

                
                
            
        
