# minidmx
Minimal dmx-uart server and tools


# restrictions

For now very little in the form of commandline arguments is implemented. 
Thus you have no choice for the name of the universe and it
has to be in the current directory.

# compiling

just type `make` and when it is succesful, `sudo make install`

# Usage

cd into the directory where you want to have the universe file. 

start the server
```
minidmx &
```
This starts the server and a dmx stream running. 

then you can modify the dmx universe with
```
setdmx [<ch>:]<val>*
```
so you set channel ch to the specified val. If you leave out a
channel, it will assume previous channel plus one. Channel numbers
start at 1. (the first byte in the universe file is the "this is 
DMX" byte necessary for the DMX protocol. ) 

## example
```
setdmx 5:16 17 18 20:44 45 46
```
this will set channel 5, 6, 7 to 16 17 18 respectivly and 20, 21, 22 to 
44 45 46. 

Note that for now you have to change directory into where the
universe file lives. Or you could create a symlink to that file 
in the current directory.



