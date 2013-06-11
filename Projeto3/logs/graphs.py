#!/usr/bin/env python
# a bar plot with errorbars
import numpy as np
import matplotlib.pyplot as plt

totalMeans = []
totalStd = []

procMeans = []
procStd = []

commMeans = []
commStd = []
comm2Means = []
comm2Std = []

for i in range(6):
    ## Total
    # Read file
    total = open('client_op' + str(i) + '.log', 'r').read()
    total = total.split('\n')
    total = [int(s) for s in total if len(s) > 0]
    
    # Ignore negative values
    total = [s for s in total if s > 0]
    
    # Numpy array
    total_ = np.array(total)
    totalMeans.append( np.nan_to_num( total_.mean() ) )
    totalStd.append( np.nan_to_num( total_.std(dtype=np.float64) ) )
    
    ## Processing
    # Read file
    proc = open('server_op' + str(i) + '.log', 'r').read()
    proc = proc.split('\n')
    proc = [int(s) for s in proc if len(s) > 0]
    
    # Ignore negative values
    proc = [s for s in proc if s > 0]
    
    # Numpy array
    proc_ = np.array(proc)
    procMeans.append( np.nan_to_num( proc_.mean() ) )
    procStd.append( np.nan_to_num( proc_.std(dtype=np.float64) ) )
    
    ## Communication
    comm = []
    comm2 = []
    for i in range(len(total)):
        comm.append( (total[i]-proc[i]) / 2.0)
        comm2.append( total[i]-proc[i] )
    
    # Numpy array
    comm_ = np.array(comm)
    commMeans.append( np.nan_to_num( comm_.mean() ) )
    commStd.append( np.nan_to_num( comm_.std(dtype=np.float64) ) )
    
    # Numpy array
    comm2_ = np.array(comm2)
    comm2Means.append( np.nan_to_num( comm2_.mean() ) )
    comm2Std.append( np.nan_to_num( comm2_.std(dtype=np.float64) ) )


N = 6
ind = np.arange(N)  # the x locations for the groups


## Plot1
# total, proc, comm
print "Plot 1"

width = 0.3       # the width of the bars

fig = plt.figure()
ax = fig.add_subplot(111)
rects1 = ax.bar(0.05+ind, totalMeans, width, color='r')#, yerr=totalStd)
rects2 = ax.bar(0.05+ind+width, procMeans, width, color='y')#, yerr=procStd)
rects3 = ax.bar(0.05+ind+2*width, commMeans, width, color='b')#, yerr=commStd)

# add some
ax.set_ylabel('t (useconds)')
ax.set_title('Time by operation')
ax.set_xticks(ind+width)
ax.set_xticklabels( ('OP0', 'OP1', 'OP2', 'OP3', 'OP4', 'OP5') )

ax.legend( (rects1[0], rects2[0], rects3[0]), ('total', 'proc', 'comm') )

def autolabel(rects):
    # attach some text labels
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x()+rect.get_width()/2., 1.05*height, '%d'%int(height),
                ha='center', va='bottom')

autolabel(rects1)
autolabel(rects2)
autolabel(rects3)

plt.savefig("plot1.png")



## Plot2
# total, proc, comm x 2
print "Plot 2"

fig = plt.figure()
ax = fig.add_subplot(111)
rects1 = ax.bar(0.05+ind, totalMeans, width, color='r')#, yerr=totalStd)
rects2 = ax.bar(0.05+ind+width, procMeans, width, color='y')#, yerr=procStd)
rects3 = ax.bar(0.05+ind+2*width, comm2Means, width, color='b')#, yerr=commStd)

# add some
ax.set_ylabel('t (useconds)')
ax.set_title('Time by operation')
ax.set_xticks(ind+width)
ax.set_xticklabels( ('OP0', 'OP1', 'OP2', 'OP3', 'OP4', 'OP5') )

ax.legend( (rects1[0], rects2[0], rects3[0]), ('total', 'proc', 'comm x 2') )

def autolabel(rects):
    # attach some text labels
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x()+rect.get_width()/2., 1.05*height, '%d'%int(height),
                ha='center', va='bottom')

autolabel(rects1)
autolabel(rects2)
autolabel(rects3)

plt.savefig("plot2.png")


## Plot3
# everything
print "Plot 3"

width = 0.2       # the width of the bars

fig = plt.figure()
ax = fig.add_subplot(111)
rects1 = ax.bar(0.05+ind, totalMeans, width, color='r')#, yerr=totalStd)
rects2 = ax.bar(0.05+ind+width, procMeans, width, color='y')#, yerr=procStd)
rects3 = ax.bar(0.05+ind+2*width, commMeans, width, color='b')#, yerr=commStd)
rects4 = ax.bar(0.05+ind+3*width, comm2Means, width, color='g')#, yerr=comm2Std)

# add some
ax.set_ylabel('t (useconds)')
ax.set_title('Time by operation')
ax.set_xticks(ind+width)
ax.set_xticklabels( ('OP0', 'OP1', 'OP2', 'OP3', 'OP4', 'OP5') )

ax.legend( (rects1[0], rects2[0], rects3[0], rects4[0]), ('total', 'proc', 'comm', 'comm x 2') )

def autolabel(rects):
    # attach some text labels
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x()+rect.get_width()/2., 1.05*height, '%d'%int(height),
                ha='center', va='bottom')

autolabel(rects1)
autolabel(rects2)
autolabel(rects3)
autolabel(rects4)

plt.savefig("plot3.png")


## Plot4
# total, proc, comm
print "Plot 4"

width = 0.3       # the width of the bars

fig = plt.figure()
ax = fig.add_subplot(111)
rects1 = ax.bar(0.05+ind, totalMeans, width, color='r', yerr=totalStd)
rects2 = ax.bar(0.05+ind+width, procMeans, width, color='y', yerr=procStd)
rects3 = ax.bar(0.05+ind+2*width, commMeans, width, color='b', yerr=commStd)

# add some
ax.set_ylabel('t (useconds)')
ax.set_title('Time by operation')
ax.set_xticks(ind+width)
ax.set_xticklabels( ('OP0', 'OP1', 'OP2', 'OP3', 'OP4', 'OP5') )

ax.legend( (rects1[0], rects2[0], rects3[0]), ('total', 'proc', 'comm') )

def autolabel(rects):
    # attach some text labels
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x()+rect.get_width()/2., 1.05*height, '%d'%int(height),
                ha='center', va='bottom')

autolabel(rects1)
autolabel(rects2)
autolabel(rects3)

plt.savefig("plot4.png")



## Plot5
# total, proc, comm x 2
print "Plot 5"

fig = plt.figure()
ax = fig.add_subplot(111)
rects1 = ax.bar(0.05+ind, totalMeans, width, color='r', yerr=totalStd)
rects2 = ax.bar(0.05+ind+width, procMeans, width, color='y', yerr=procStd)
rects3 = ax.bar(0.05+ind+2*width, comm2Means, width, color='b', yerr=commStd)

# add some
ax.set_ylabel('t (useconds)')
ax.set_title('Time by operation')
ax.set_xticks(ind+width)
ax.set_xticklabels( ('OP0', 'OP1', 'OP2', 'OP3', 'OP4', 'OP5') )

ax.legend( (rects1[0], rects2[0], rects3[0]), ('total', 'proc', 'comm x 2') )

def autolabel(rects):
    # attach some text labels
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x()+rect.get_width()/2., 1.05*height, '%d'%int(height),
                ha='center', va='bottom')

autolabel(rects1)
autolabel(rects2)
autolabel(rects3)

plt.savefig("plot5.png")


## Plot6
# everything
print "Plot 6"

width = 0.2       # the width of the bars

fig = plt.figure()
ax = fig.add_subplot(111)
rects1 = ax.bar(0.05+ind, totalMeans, width, color='r', yerr=totalStd)
rects2 = ax.bar(0.05+ind+width, procMeans, width, color='y', yerr=procStd)
rects3 = ax.bar(0.05+ind+2*width, commMeans, width, color='b', yerr=commStd)
rects4 = ax.bar(0.05+ind+3*width, comm2Means, width, color='g', yerr=comm2Std)

# add some
ax.set_ylabel('t (useconds)')
ax.set_title('Time by operation')
ax.set_xticks(ind+width)
ax.set_xticklabels( ('OP0', 'OP1', 'OP2', 'OP3', 'OP4', 'OP5') )

ax.legend( (rects1[0], rects2[0], rects3[0], rects4[0]), ('total', 'proc', 'comm', 'comm x 2') )

def autolabel(rects):
    # attach some text labels
    for rect in rects:
        height = rect.get_height()
        ax.text(rect.get_x()+rect.get_width()/2., 1.05*height, '%d'%int(height),
                ha='center', va='bottom')

autolabel(rects1)
autolabel(rects2)
autolabel(rects3)
autolabel(rects4)

plt.savefig("plot6.png")
