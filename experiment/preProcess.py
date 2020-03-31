import matplotlib.pyplot as plt

fo = open("data.txt", "r")
k = 15
dim1=[]
dim2=[]
lines = fo.readlines()
outF = open("input.txt", "w")
for i in range(len(lines)):
    line = lines[i].strip()
    outF.write(line)
    outF.write("\n")
outF.close()
    
