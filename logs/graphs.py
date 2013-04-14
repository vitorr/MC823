#!/usr/bin/env python
# a bar plot with errorbars
import numpy as np
import matplotlib.pyplot as plt

menMeans = []
menStd = []
for i in range(6):
    content = open('client_op' + str(i) + '.log', 'r').read()
    a = content.split('\n')
    a1 = [int(s) for s in a if len(s) > 0]
    a2 = [s for s in a1 if s > 0]
    a3 = [s for s in a2 if abs(s) < 100000]
    b = np.array(a3)
    mean = np.nan_to_num( b.mean() )
    std = np.nan_to_num( b.std(dtype=np.float64) )
    menMeans.append( mean )
    menStd.append( std )

print menMeans
print menStd

womenMeans = []
womenStd = []
for i in range(6):
    content = open('server_op' + str(i) + '.log', 'r').read()
    a = content.split('\n')
    a1 = [int(s) for s in a if len(s) > 0]
    a2 = [s for s in a1 if s > 0]
    a3 = [s for s in a2 if abs(s) < 100000]
    b = np.array(a3)
    mean = np.nan_to_num( b.mean() )
    std = np.nan_to_num( b.std(dtype=np.float64) )
    womenMeans.append( mean )
    womenStd.append( std )

print womenMeans
print womenStd

N = 6

#menMeans = (20, 35, 30, 35, 27)
#menStd =   (2, 3, 4, 1, 2)

ind = np.arange(N)  # the x locations for the groups
width = 0.35       # the width of the bars

fig = plt.figure()
ax = fig.add_subplot(111)
rects1 = ax.bar(ind, menMeans, width, color='r', yerr=menStd)

#womenMeans = (25, 32, 34, 20, 25)
#womenStd =   (3, 5, 2, 3, 3)
rects2 = ax.bar(ind+width, womenMeans, width, color='y', yerr=womenStd)

# add some
ax.set_ylabel('Scores')
ax.set_title('Scores by group and gender')
ax.set_xticks(ind+width)
ax.set_xticklabels( ('OP0', 'OP1', 'OP2', 'OP3', 'OP4', 'OP5') )

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
