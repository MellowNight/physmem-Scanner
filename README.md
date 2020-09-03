# hwid spoofer/physical memory tool
 scans through memory and places some hooks around to spoof hwids. physicalMemory is kernel mode driver, spooferTool is usermode.
 
 ## how it works:
 
1. sends commands through CMD to retrieve serial number and place in text file
2. reads from text file
3. sends content to driver
4. driver scans physical memory for serial numbers
5. spoof one by one in memory

## current issue:

sometimes it doesn't successfully spoof everything


## NOTE:
- for s.m.a.r.t serials you NEED to place hooks
- for GPU uuid you MAY need to place hooks
