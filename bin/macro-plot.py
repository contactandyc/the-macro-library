#!/usr/bin/env python3

# Copyright 2019-2025 Andy Curtis
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

import numpy as np
import matplotlib.pyplot as plt
import csv, sys
from pprint import pprint

# Set width of bar
barWidth = 0.25
fig = plt.subplots(figsize=(12, 8))

# Read Data
with open('plot.dat', 'r') as file:
    reader = csv.reader(file)
    xlabel = next(reader)[0]  # Get the first line for xlabel
    rows = [row for row in reader]

# Extract Data
row1 = int(sys.argv[1])
row2 = int(sys.argv[2])

test_names = rows[0][1:]


sort1 = [float(x) for x in rows[row1][1:]]
sort2 = [float(x) for x in rows[row2][1:]]

# Set position of bar on X axis
br1 = np.arange(len(test_names))
br2 = [x + barWidth for x in br1]

custom_colors = [(51/256.0, 116/256.0, 181/256.0),  (185.0/256.0, 42/256.0, 23/256.0)]

# Make the plot
plt.bar(br1, sort1, color=custom_colors[0], width=barWidth, edgecolor='grey', label=rows[row1][0])
plt.bar(br2, sort2, color=custom_colors[1], width=barWidth, edgecolor='grey', label=rows[row2][0])

# Adding Xticks and labels
plt.xlabel(xlabel, fontweight='bold', fontsize=15)
plt.ylabel('Milliseconds', fontweight='bold', fontsize=15)
plt.xticks([r + barWidth for r in range(len(test_names))], test_names, rotation=45, ha='right')

plt.subplots_adjust(left=0.125, right=0.98, bottom=0.3, top=0.98)

order_sum1 = sum(sort1[0:2])
order_sum2 = sum(sort2[0:2])
random_sum1 = sum(sort1[3:])
random_sum2 = sum(sort2[3:])

cell_text = [
    [rows[row1][0], f"{order_sum1:,.0f}", f"{order_sum2 / order_sum1 * 100:.1f}%",
                    f"{sort1[2]:,.0f}", f"{sort2[2] / sort1[2] * 100:.1f}%",
                    f"{random_sum1:,.0f}", f"{random_sum2 / random_sum1 * 100:.1f}%"],
    [rows[row2][0], f"{order_sum2:,.0f}", f"{order_sum1 / order_sum2 * 100:.1f}%",
                    f"{sort2[2]:,.0f}", f"{sort1[2] / sort2[2] * 100:.1f}%",
                    f"{random_sum2:,.0f}", f"{random_sum1 / random_sum2 * 100:.1f}%"]
]

the_table = plt.table(cellText=cell_text,
                      colLabels=['Sort', 'Ordered', '', 'Reversed', '', 'Random', ''],
                      colColours=['lightgray'] * 7,
                      colWidths=[0.22, 0.07, 0.07, 0.07, 0.07, 0.07, 0.07],
                      loc='upper left')

for i, key in enumerate(the_table.get_celld().keys()):
    cell = the_table.get_celld()[key]
    row, col = key
    cell._text.set_weight('bold')
    if row == 1:
        cell._text.set_color(custom_colors[0])
    elif row == 2:
        cell._text.set_color(custom_colors[1])

the_table.auto_set_font_size(False)
the_table.set_fontsize(9)
the_table.scale(1.0, 1.0)

if len(sys.argv) > 3 and sys.argv[-2] == '-o':
    plt.savefig(sys.argv[-1], dpi=300, bbox_inches='tight')
else:
    plt.show()
