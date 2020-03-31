import matplotlib.pyplot as plt

fo = open("centers-ham.txt", "r")
ff = open("d2.txt")
k = 2500
dim1=[]
dim2=[]
lines = ff.readlines()
for i in range(1,k):
    line = lines[i].strip()
    data = line.split(' ')
    dim1.append(float(data[0]))
    dim2.append(float(data[1]))
plt.plot(dim1,dim2, 'ro')

d1=[]
d2=[]
lines = fo.readlines()
for i in range(5):
    line = lines[i].strip()
    data = line.split(' ')
    d1.append(float(data[0]))
    d2.append(float(data[1]))
    print(data[0], data[1])
plt.plot(d1,d2, 'bo')

plt.title('Plot of centers for minibatch algorithm (100 outer iterations,40 inner iterations)')
#plt.savefig('centersMiniBatch.png')
plt.show()
    
