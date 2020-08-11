import pandas as pd
threadnum=4
for i in range(threadnum):
    
    print(i)
    n=999
    
    if i<9:
        df = pd.read_csv("./2020-0"+str(i),sep="   ")
        if len(df)<999:
            n=len(df)
        
        df = df[:n]
        df.to_excel("2020-0"+str(i)+".xlsx",index=0)
    else:
        df = pd.read_csv("./2020-"+str(i),sep="   ")
        if len(df)<999:
            n=len(df)
        df = df[:n]
        df.to_excel("2020-"++str(i)+".xlsx",index=0)

print("Generate Finish")