
// testbaud.c  compile with 
// gcc -Wall -O2 testbaud.c -o testbaud
// 
// run with 
// ./testbaud < /dev/ttyAMA0
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stropts.h>
//#include <sys/ioctl.h>
#include <asm/termbits.h>
#include <errno.h>
#include <asm/termios.h>
#include <unistd.h>
#include <fcntl.h>


#define DMXSIZE 64

unsigned char buf[DMXSIZE+1];

void update_dmx_universe (unsigned char *dmx)
{
  static int t;
  if (t++ < 15) return;
  static int p;

  t = 0;
#define M 4
  p = (p+1) % M;

  for (int i =1;i<DMXSIZE;i++) 
    dmx[i] = ((i % M) == p)?255:0 ;
}



int main (int argc, char **argv)
{
  static const int rate = 250000;
  int fd = 0;
  struct termios2 tio;  // linux-specific terminal stuff
  char *port = argv[1];

  if (!port) {
    port = "/dev/ttyAMA0";
  }
  fd = open (port, O_RDWR);
  if (fd < 0) {
    perror (port);
    exit (1);
  }

  if (ioctl(fd, TCGETS2, &tio) < 0) {
    printf  ("Failed to get current serial port settings");
    return 1;
  }
  printf ("Port speeds before change %d/%d\n" , tio.c_ispeed,  tio.c_ospeed );


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

  if (ioctl(fd, TCSETS2, &tio) < 0) {
    printf ("Failed to update serial port settings");
    return 1;
  }

  if (ioctl(fd, TCGETS2, &tio) < 0) {
    printf ("Error getting altered settings from port");
  } else {
    printf ("Port speeds are %d/%d\n" , tio.c_ispeed,  tio.c_ospeed );
  }
  buf [0] = 0;

  while (1) {
    if (ioctl(fd, TIOCSBRK, NULL) < 0) {
      printf (" ioctl() failed");
      exit (1);
    }
    usleep (100);
    if (ioctl(fd, TIOCCBRK, NULL) < 0) {
      printf (" ioctl() failed");
      exit (1);
    }
    
    usleep (100);
    if (write (fd, buf, DMXSIZE+1) < DMXSIZE) 
      perror ("write");
    update_dmx_universe (buf);
    usleep (30000);

  }

 
}
