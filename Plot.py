import csv
import matplotlib.pyplot as plt

d = {}
d['bytes'] = []
d['mmh3'] = []
d['city'] = []
d['xxhash'] = []

dictReader = csv.DictReader(open('out.csv', 'r'), fieldnames = ['bytes', 'mmh3', 'city', 'xxhash'], delimiter = ',', quotechar = '"')

t = True
for row in dictReader:
    if t:
        t = False
        continue
    for key in row:
        d[key].append(float(row[key]))

plt.plot(d['bytes'], d['mmh3'], label='mmh3')
plt.plot(d['bytes'], d['city'], label='sity')
plt.plot(d['bytes'], d['xxhash'], label='xxhash')

plt.legend()

plt.xlabel('Key size in bytes')
plt.ylabel('cycles per hash')
plt.title('Cycles per hash (less is better) (g++ -O3) (step = 50)')

plt.show()