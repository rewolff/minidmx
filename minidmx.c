
// testbaud.c  compile with 
// gcc -Wall -O2 testbaud.c -o testbaud
// 
// run with 
// ./testbaud < /dev/ttyAMA0
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <stropts.h>
//  sys/ioctl clashes with asm/termios. That one is required, this just generates a warning. 
//#include <sys/ioctl.h>
#include <asm/termbits.h>
#include <errno.h>
#include <asm/termios.h>
#include <unistd.h>
#include <fcntl.h>


#define DMXSIZE 64

unsigned char buf[DMXSIZE+1];

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

int setup_dmx_uart (char *port) 
{
  static const int rate = 250000;
  struct termios2 tio;  // linux-specific terminal stuff
  int fd;
 
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
    exit (1);
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
    exit (1);
  }

  if (ioctl(fd, TCGETS2, &tio) < 0) {
    printf ("Error getting altered settings from port");
    exit (1);
  } else {
    printf ("Port speeds are %d/%d\n" , tio.c_ispeed,  tio.c_ospeed );
  }
  buf [0] = 0;
  return fd;
}

void send_dmx_frame (int fd, unsigned char *buf)
{
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
}


int main (int argc, char **argv)
{
  int fd = 0;
//  char *port = argv[1];
//  char *port = NULL;

  if (argc > 1) led = atoi (argv[1]);
  fd = setup_dmx_uart (NULL);

  while (1) {
    send_dmx_frame (fd, buf);
    update_dmx_universe (buf);
    usleep (15000);

  }
  exit (0); 
}
