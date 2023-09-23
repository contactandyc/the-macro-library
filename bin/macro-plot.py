#!/usr/bin/env python3

import numpy as np
import matplotlib.pyplot as plt
import csv, sys
from pprint import pprint

# Set width of bar
barWidth = 0.25
fig = plt.subplots(figsize=(12, 8))

print('reading plot.dat')

# Read Data
with open('plot.dat', 'r') as file:
    reader = csv.reader(file)
    xlabel = next(reader)[0]  # Get the first line for xlabel
    rows = [row for row in reader]

# Extract Data
test_names = rows[0][1:]
sort1 = [float(x) for x in rows[1][1:]]
sort2 = [float(x) for x in rows[2][1:]]
sort3 = [float(x) for x in rows[3][1:]]

# Set position of bar on X axis
br1 = np.arange(len(test_names))
br2 = [x + barWidth for x in br1]
br3 = [x + barWidth for x in br2]

custom_colors = [(48/256.0, 180/256.0, 29/256.0), (51/256.0, 116/256.0, 181/256.0),  (185.0/256.0, 42/256.0, 23/256.0)]

# Make the plot
plt.bar(br1, sort1, color=custom_colors[0], width=barWidth, edgecolor='grey', label=rows[1][0])
plt.bar(br2, sort2, color=custom_colors[1], width=barWidth, edgecolor='grey', label=rows[2][0])
plt.bar(br3, sort3, color=custom_colors[2], width=barWidth, edgecolor='grey', label=rows[3][0])

# Adding Xticks and labels
plt.xlabel(xlabel, fontweight='bold', fontsize=15)
plt.ylabel('Milliseconds', fontweight='bold', fontsize=15)
plt.xticks([r + barWidth for r in range(len(test_names))], test_names, rotation=45, ha='right')

plt.subplots_adjust(left=0.125, right=0.98, bottom=0.3, top=0.98)

plt.legend()

print('close plot to exit')

if len(sys.argv) > 2 and sys.argv[-2] == '-o':
    plt.savefig(sys.argv[-1], dpi=300, bbox_inches='tight')
else:
    plt.show()
