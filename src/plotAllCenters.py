import matplotlib.pyplot as plt



ff = open("centersAll.txt")

lines = ff.readlines()

firstLine = lines[0]
k = int(lines[0])
i=1

while(i<len(lines)):
    col = '$'+str(int(lines[i]))+'$'
    i+=1
    for j in range(k):
        line = lines[i].split(',')
        plt.scatter(float(line[0]),float(line[1]),marker=col)
        i+=1
#    plt.plot(dim1,dim2, marker=co[c])

fo = open("centersTrue.txt")
lines = fo.readlines()
for j in range(k):
    line = lines[j].strip().split('       ')
    plt.scatter(float(line[0]),float(line[1]))

plt.title('Plot of centers given by minibatch(n=1,250,000, k=10, batchSize = 3000, iterations = 2000)\n Numbers represent the iteration in which that center was generated. \n The point '
          'represents the center given by lloyd algorithm')
plt.xlabel('dimension 1')
plt.ylabel('dimension 2')

plt.savefig('centersAll.png')
plt.show()
