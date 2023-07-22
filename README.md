# minidmx
Minimal dmx-uart server and tools


# restrictions

For now very little in the form of commandline arguments is implemented. 
Thus you have no choice for the name of the universe and it
has to be in the current directory.


# Usage

cd into the directory where you put the binary and want to have
the universe file. The binary can be somewhere else, but then the
following commands need to be modified slightly. 

start the server
```
./minidmx &
```
This starts the server and a dmx stream running. 

then you can modify the dmx universe with
```
./setdmx [<ch>:]<val>*
```
so you set channel ch to the specified val. If you leave out a
channel, it will assume previous channel plus one. Channel numbers
start at 1. (the first byte in the universe file is the "this is 
DMX" byte necessary for the DMX protocol. ) 


## example
```
./setdmx 5:16 17 18 20:44 45 46
```
this will set channel 5, 6, 7 to 16 17 18 respectivly and 20, 21, 22 to 
44 45 46. 



