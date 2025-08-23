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


import matplotlib.pyplot as plt
import csv, sys

# Read data from CSV
data = []
with open('plot.dat', 'r') as csvfile:
    csvreader = csv.reader(csvfile)
    xlabel = next(csvreader)[0]  # Get the first line for xlabel
    for row in csvreader:
        data.append(row)

# Extract headers and types
types = data[0][1:]
headers = [row[0] for row in data[1:]]

plots_per_row = 4

if len(types) < plots_per_row:
    plots_per_row = len(types)

rows = (int)(len(types) / plots_per_row)
if len(types) % plots_per_row != 0:
    rows = rows + 1

# plt.xlabel(xlabel, fontweight='bold', fontsize=15, loc='center')
fig, axs = plt.subplots(rows, plots_per_row, figsize=(36, 10))

wedges = None

custom_colors = [(48/256.0, 180/256.0, 29/256.0), (51/256.0, 116/256.0, 181/256.0),  (185.0/256.0, 42/256.0, 23/256.0)]

if len(sys.argv) > 1 and sys.argv[1] == '1':
    labels = [headers[0], headers[2]]
    colors = [custom_colors[0], custom_colors[2]]
    first_values = [float(data[1][1]), float(data[3][1])]
elif len(sys.argv) > 1 and sys.argv[1] == '2':
    labels = [headers[1], headers[2]]
    colors = [custom_colors[1], custom_colors[2]]
    first_values = [float(data[2][1]), float(data[3][1])]
else:
    labels = [headers[0], headers[1]]
    colors = [custom_colors[0], custom_colors[1]]
    first_values = [float(data[1][1]), float(data[2][1])]

# Create pie charts
pos = 0
row = 0
while pos < len(types):
    n = len(types) - pos
    if n > plots_per_row:
        n = plots_per_row
    for i in range(0, n):
        sizes = [float(row[pos + i + 1]) for row in data[1:]]
        if len(sys.argv) > 1 and sys.argv[1] == '1':
            sizes = [sizes[0], sizes[2]]
        elif len(sys.argv) > 1 and sys.argv[1] == '2':
            sizes = [sizes[1], sizes[2]]
        else:
            sizes = [sizes[0], sizes[1]]
        wedges, _, _ = axs[row][i].pie(sizes, labels=None, colors=colors, autopct='%1.1f%%', textprops={'color': 'white', 'weight': 'bold', 'fontsize': 16})
        axs[row][i].set_title(f'{types[pos + i]}')
        # axs[row][i].text(0, 0, f"{(sizes[1]/sizes[0]):.2f}x", color='white', fontweight='bold', fontsize=30, ha='center', va='center')
    pos = pos + n
    row = row + 1

fig.legend(wedges, labels, loc='center', bbox_to_anchor=(0.5, 0.52), fontsize=17)
if len(sys.argv) <= 2 or sys.argv[-2] != '-o':
    plt.suptitle(xlabel, fontweight='bold', fontsize=24)

subtitle = labels[0] + ' is ' + f"{(first_values[1]/first_values[0]):.2f} times faster than " + labels[1] + " for " + types[0]
fig.text(0.5, 0.9, subtitle, ha='center', va='center', fontsize=16)
plt.subplots_adjust(left=0.17, right=0.83, bottom=0.2, top=0.8)

if len(sys.argv) > 2 and sys.argv[-2] == '-o':
    plt.subplots_adjust(wspace=-0.4)  # adjust spacing
    fig.savefig(sys.argv[-1], dpi=300, bbox_inches='tight')
else:
    plt.show()
