// (C) BitWizard B.V.
// written by R.E.Wolff@BitWizard.nl
// GPL v2.0 applies
// setdmx.c  compile with 
// gcc -Wall -O2 setdmx.c -o setdmx
// 
// run with 
// ./setdmx <channel:value> <value>...  
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//  sys/ioctl clashes with asm/termios. That one is required, this just generates a warning. 
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>


#define DMXSIZE 512
unsigned char *dmx;


void fatal (char *msg)
{
  perror (msg);
  exit (1);
}


unsigned char *open_universe_file (char *fname)
{
  int fd; 
  unsigned char *tmpdmx;

  fd = open(fname, O_RDWR);
  if (fd < 0) { // help the user: create the universe file.
     fd = open(fname, O_RDWR  | O_CREAT, 0666);
     if (fd < 0) fatal (fname);
     tmpdmx = malloc (4096);
     write (fd, tmpdmx, 4096);
     free (tmpdmx);
     tmpdmx = NULL;
  }

  tmpdmx = mmap (NULL, DMXSIZE+1, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (tmpdmx ==  (void *)-1) fatal ("mmap"); 
  tmpdmx[0] = 0;
  return tmpdmx;
}



int main (int argc, char **argv)
{
  int port, val;

  dmx = open_universe_file ("dmx_universe"); 
  port = 1;
  for (int i=1;i<argc;i++) {
     if (strchr (argv[i], ':')) sscanf (argv[i], "%d:%d", &port, &val); 
     else                      sscanf (argv[i], "%d", &val);
     if ((port > 0) && (port <= 512)) dmx[port++] = val; 
  }
  exit (0); 
}
