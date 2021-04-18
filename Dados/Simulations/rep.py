count = 0 
def first(source): 
    for i,w in enumerate(source): 
        if w == "Interval_conf_TASA_PDR:" or w == "Interval_conf_LBV_PDR:": 
            return float(source[i+2]) 
        if w == "Interval_conf_THLBV:" or w == "Interval_conf_THTASA:": 
              return float(source[i+2])
def second(source): 
    for i,w in enumerate(source): 
        if w == "Interval_conf_TASA_PDR:" or w == "Interval_conf_LBV_PDR:": 
            return float(source[i+4]) 
        if w == "Interval_conf_THLBV:" or w == "Interval_conf_THTASA:": 
              return float(source[i+4])

print("entrou") 
with open('teste.txt','r') as f:
    for line in f: 
        words = line.split() 
        if "Interval_conf_TASA_PDR:" in words:  
            total = second(words) - first(words) 
            print(total)
        if "Interval_conf_LBV_PDR:" in words:   
            total = second(words) - first(words) 
            print(total)
        if "Interval_conf_TASA_confiabilidade:" in words: 
            total = second(words) - first(words) 
            print(total)
        if "Interval_conf_LBV_confiabilidade:" in words:    
            total = second(words) - first(words) 
            print(total)       
        if "Interval_conf_LBV_confiabilidade:" in words:  
            total = second(words) - first(words) 
            print(total)  
        if "Interval_conf_THTASA:" in words:    
            total = second(words) - first(words) 
            print(total) 
        if "Interval_conf_THLBV:" in words: 
            total = second(words) - first(words) 
            print(total)