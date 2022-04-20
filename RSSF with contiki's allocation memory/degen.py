
arq = open('degen.txt', 'w') 
n = 500


for i in range(0,n): 
    
    if(n - 1*i == 1 ): break
    arq.writelines(str(n - 1*i) + ' ' + str(n - 1*i -1) + '\n')
    #print(n - 1*i, n - 1*i -1) 


arq.close()
