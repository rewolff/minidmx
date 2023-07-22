// (C) BitWizard B.V.
// written by R.E.Wolff@BitWizard.nl
// GPL v2.0 applies
//
// ** minidmx.c **  
// compile with 
// gcc -Wall -O2 minidmx.c -o minidmx
// 
// run with 
// ./minidmx 
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stropts.h>
//  sys/ioctl clashes with asm/termios. That one is required, this just generates a warning. 
//#include <sys/ioctl.h>
#include <asm/termbits.h>
#include <sys/mman.h>
#include <errno.h>
#include <asm/termios.h>
#include <unistd.h>
#include <fcntl.h>


#define DMXSIZE 512

unsigned char *dmx;
int dbg=1;

int led=1; 

void update_dmx_universe (unsigned char *dmx)
{
  static int t;

  t++;
#if 1
  if (t < 30) dmx[led] = 15 + t * 5;
  else        dmx[led] = 15 + (60-t) * 5;
#endif
//  printf ("%d\n", dmx[led]);

  if ((t != 30) && (t != 60)) return;
  if (t >= 60) {
     t = 0;
//     led++;
 //    printf ("led=%d\n", led);
  }
  static int p;

#define M 4
  p = (p+1) % M;

  for (int i =1;i<DMXSIZE;i++) 
    if (i != led)
      dmx[i] = ((i % M) == p)?255:0 ;

}


void fatal (char *msg)
{
  perror (msg);
  exit (1);
}


int setup_dmx_uart (char *port) 
{
  static const int rate = 250000;
  struct termios2 tio;  // linux-specific terminal stuff
  int fd;
 
  if (!port) {
    port = "/dev/ttyAMA0";
  }
  fd = open (port, O_RDWR);
  if (fd < 0) fatal (port);

  if (ioctl(fd, TCGETS2, &tio) < 0) fatal ("ioctl tcgets");
  if (dbg > 0) printf ("Port speeds before change %d/%d\n" , tio.c_ispeed,  tio.c_ospeed );

  tio.c_cflag |= CLOCAL;    // port is local, no flow control
  tio.c_cflag &= ~CSIZE;
  tio.c_cflag |= CS8;       // 8 bit chars
  tio.c_cflag &= ~PARENB;   // no parity
  tio.c_cflag |= CSTOPB;    // 2 stop bit for DMX
  tio.c_cflag &= ~CRTSCTS;  // no CTS/RTS flow control

  tio.c_cflag &= ~CBAUD;
  tio.c_cflag |= BOTHER;
  tio.c_ispeed = rate;
  tio.c_ospeed = rate;  // set custom speed directly

  if (ioctl(fd, TCSETS2, &tio) < 0) fatal ("ioctl tcsets");
  if (ioctl(fd, TCGETS2, &tio) < 0) fatal ("ioctl tcgets2");
  if (dbg > 0) printf ("Port speeds are %d/%d\n" , tio.c_ispeed,  tio.c_ospeed );
  return fd;
}



void send_dmx_frame (int fd, unsigned char *dmx)
{
  if (ioctl(fd, TIOCSBRK, NULL) < 0) fatal ("ioctl(SBRK) failed");
  usleep (100);
  if (ioctl(fd, TIOCCBRK, NULL) < 0) fatal ("ioctl(CBRK) failed");
  usleep (8);
  if (write (fd, dmx, DMXSIZE+1) < DMXSIZE) perror ("write");
}



unsigned char *open_universe_file (char *fname)
{
  int fd; 
  unsigned char *tmpdmx;

  fd = open(fname, O_RDWR);
  if (fd < 0) { // help the user: create the universe file.
     fd = open(fname, O_RDWR  | O_CREAT, 0666);
     if (fd < 0) fatal (fname);
     tmpdmx = calloc (4096, 1);
     write (fd, tmpdmx, 4096);
     free (tmpdmx);
     tmpdmx = NULL;
  }

  tmpdmx = mmap (NULL, 513, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
  if (tmpdmx ==  (void *)-1) fatal ("mmap"); 
  tmpdmx[0] = 0;
  return tmpdmx;
}






int main (int argc, char **argv)
{
  int fd = 0;
  int test = 0;
//  char *port = argv[1];
//  char *port = NULL;

  if (argc > 1) led = atoi (argv[1]);

  if (test) {
     dmx = malloc (DMXSIZE+1); 
  } else {
     dmx = open_universe_file ("dmx_universe"); 
  }

  fd = setup_dmx_uart (NULL);

  while (1) {
    send_dmx_frame (fd, dmx);
    if (test)
       update_dmx_universe (dmx);
    usleep (25000);
  }
  exit (0); 
}
