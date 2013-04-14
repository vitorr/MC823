#!/usr/bin/env python
# a bar plot with errorbars
import numpy as np
import matplotlib.pyplot as plt

menMeans = []
menStd = []
for i in range(2):
    content = open('client_op' + str(i) + '.log', 'r').read()
    a = content.split('\n')
    a1 = [int(s) for s in a if len(s) > 0]
    a2 = [int(s) for s in a2 if s > 0]
    a3 = [int(s) for s in a3 if abs(s) < 100000]
    b = np.array(a3)
    menMeans.append( b.mean() )
    menStd.append( b.std(dtype=np.float64) )

print menMeans
print menStd

N = 2

#menMeans = (20, 35, 30, 35, 27)
#menStd =   (2, 3, 4, 1, 2)

ind = np.arange(N)  # the x locations for the groups
width = 0.35       # the width of the bars

fig = plt.figure()
ax = fig.add_subplot(111)
rects1 = ax.bar(ind, menMeans, width, color='r', yerr=menStd)

#womenMeans = (25, 32, 34, 20, 25)
#womenStd =   (3, 5, 2, 3, 3)
#rects2 = ax.bar(ind+width, womenMeans, width, color='y', yerr=womenStd)

# add some
ax.set_ylabel('Scores')
ax.set_title('Scores by group and gender')
ax.set_xticks(ind+width)
ax.set_xticklabels( ('G1', 'G2', 'G3', 'G4', 'G5') )

#ax.legend( (rects1[0], rects2[0]), ('Men', 'Women') )

def autolabel(rects):
    # attach some text labels
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x()+rect.get_width()/2., 1.05*height, '%d'%int(height),
                ha='center', va='bottom')

autolabel(rects1)
#autolabel(rects2)

plt.show()
