import numpy as np
import matplotlib.pyplot as plt

# Set width of bar
barWidth = 0.2
fig = plt.subplots(figsize=(12, 8))

# Data
test_names = [
    'ascending', 'ascending[gap=0]', 'descending',
    'saw[period=10,max=100]', 'saw[period=10,max=-100]',
    'random', 'rand_max[max=100]', 'rand_max[max=10000]',
    'rand_head[pct=25]', 'rand_tail[pct=25]'
]

macro_sort = [
    0.136391, 0.227794, 0.246166, 0.695768, 0.691319,
    6.480880, 2.505797, 5.956528, 2.789506, 3.011739
]

std_sort = [
    0.346498, 0.359617, 0.683351, 1.219611, 1.274635,
    8.028105, 3.630266, 7.415671, 3.954147, 4.204019
]

qsort = [
    0.490240, 0.660179, 2.450071, 1.582721, 1.572666,
    10.640059, 4.067321, 8.924805, 6.123680, 5.975161
]

# Set position of bar on X axis
br1 = np.arange(len(test_names))
br2 = [x + barWidth for x in br1]
br3 = [x + barWidth for x in br2]

# Make the plot
plt.bar(br1, macro_sort, color='r', width=barWidth, edgecolor='grey', label='macro_sort')
plt.bar(br2, std_sort, color='g', width=barWidth, edgecolor='grey', label='std_sort')
plt.bar(br3, qsort, color='b', width=barWidth, edgecolor='grey', label='qsort')

# Adding Xticks
plt.xlabel('Sorting 100000 elements 1000 times, each 20 byte(s)', fontweight='bold', fontsize=15)
plt.ylabel('Seconds', fontweight='bold', fontsize=15)
plt.xticks([r + barWidth for r in range(len(test_names))], test_names, rotation=45, ha='right')

plt.legend()
plt.show()
