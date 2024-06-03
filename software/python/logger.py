import pandas as pd
import json
import time
from datetime import datetime

class Logger:
    def __init__(self):
        _columns = ["tStamp","iCache","dCache","alg","note","tExec"]
        self.table = pd.DataFrame(columns=_columns)

    def logEntry(self, serial):
        doc = json.loads(serial)
        
        entry = pd.DataFrame(doc)
        entry.loc[0,"tStamp"] = datetime.now()
        self.table = pd.concat([self.table,entry],ignore_index=True)

    def loadLog(self, path):
        self.table = pd.concat([self.table,pd.read_csv(path)], ignore_index=True)
            
    def saveLog(self, path):
        self.table.to_csv(path, index=False, mode='w') 

    def get_dtypes(self):
        return self.table.dtypes
    
    def print(self):
        print(self.table)
        


l = Logger()
l.loadLog('31_05_2024.csv')

entry = "{\"tStamp\":[1],\"iCache\":[2],\"dCache\":[3],\"alg\":[4],\"note\":[5],\"tExec\":[8]}"
l.logEntry(entry)
l.print()

l.saveLog("31_05_2024.csv")