import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import csv

d = {}
d['filename'] = []
d['lz4'] = []
d['zstd_lvl1'] = []
d['zstd_lvl7'] = []

dictReader = csv.DictReader(open('cmake-build-debug/compression.csv', 'r'), fieldnames = ['filename', 'lz4', 'zstd_lvl1', 'zstd_lvl7'], delimiter = ',', quotechar = '"')

t = True
for row in dictReader:
    if t:
        t = False
        continue
    for key in row:
        if key == 'filename':
            d[key].append(row[key])
        else:
            d[key].append(float(row[key]))

data = {'lz4': d['lz4'],
        'zstd_lvl1': d['zstd_lvl1'],
        'zstd_lvl7': d['zstd_lvl7']}
df = pd.DataFrame(data)
df.plot(kind='bar', title='Compression', xlabel='File name', ylabel='Compression ratio') #, logy=True

index = np.arange(len(d['filename']))
plt.xticks(index, d['filename'], rotation=50)
plt.show()