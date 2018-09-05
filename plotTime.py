import numpy as np
import matplotlib.mlab as mlab
import matplotlib.pyplot as plt
F = open("makeSegmentParalel.txt","r")
x = []
y = []
for line in F:
    i = 0
    for val in line.split():
        i ^= 1
        print float(val)
        if( i == 0):
            x.append(float(val))
        else:
            y.append(float(val))

ax = plt.subplot(111)
 
plt.plot(x,y, color ='r',linestyle='--', marker='o')
 
plt.title("Tempo de execucao da funcao makeSegmentParalel")
plt.ylabel("Tempo")
plt.xlabel("Quantidade de Hits")
plt.grid(alpha=0.8)
plt.savefig('makeSegmentParalel')
plt.show()