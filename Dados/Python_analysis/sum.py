num_aux = int(input(""))
num = [] 
Ma_CPU = 10 
while num_aux != -1:
    num_aux = int(input(""))  
    if(num_aux != -1):
        num.append(num_aux)
voltage = 3 
sum = 0  
count = 0 
for i in num: 
    print(i)
    sum += i  
    count +=1  

avg_current_mA  = (sum/count)*Ma_CPU 
state_power_mW = avg_current_mA * voltage 

print("Soma ",sum,"S") 
print("Media:",sum/count) 
print("Gasto em mA:",avg_current_mA) 
print("Custo energetico medio mW: ",state_power_mW )