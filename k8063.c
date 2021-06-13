#include <errno.h>
#include <fcntl.h> 
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>

int
set_interface_attribs (int fd, int speed, int parity)
{
        struct termios tty;
        if (tcgetattr (fd, &tty) != 0)
        {
                fprintf(stderr, "error %d from tcgetattr", errno);
                return -1;
        }

        cfsetospeed (&tty, speed);
        cfsetispeed (&tty, speed);

        tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;     // 8-bit chars
        // disable IGNBRK for mismatched speed tests; otherwise receive break
        // as \000 chars
        tty.c_iflag &= ~IGNBRK;         // disable break processing
        tty.c_lflag = 0;                // no signaling chars, no echo,
                                        // no canonical processing
        tty.c_oflag = 0;                // no remapping, no delays
        tty.c_cc[VMIN]  = 0;            // read doesn't block
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        tty.c_iflag &= ~(IXON | IXOFF | IXANY); // shut off xon/xoff ctrl

        tty.c_cflag |= (CLOCAL | CREAD);// ignore modem controls,
                                        // enable reading
        tty.c_cflag &= ~(PARENB | PARODD);      // shut off parity
        tty.c_cflag |= parity;
        tty.c_cflag &= ~CSTOPB;
        tty.c_cflag &= ~CRTSCTS;

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
        {
                fprintf(stderr, "error %d from tcsetattr", errno);
                return -1;
        }
        return 0;
}

void
set_blocking (int fd, int should_block)
{
        struct termios tty;
        memset (&tty, 0, sizeof tty);
        if (tcgetattr (fd, &tty) != 0)
        {
                fprintf(stderr, "error %d from tggetattr", errno);
                return;
        }

        tty.c_cc[VMIN]  = should_block ? 1 : 0;
        tty.c_cc[VTIME] = 5;            // 0.5 seconds read timeout

        if (tcsetattr (fd, TCSANOW, &tty) != 0)
                fprintf(stderr, "error %d setting term attributes", errno);
}

int main()
{
    const char *portname = "/dev/ttyS0";

    int fd = open (portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0)
    {
            fprintf(stderr, "error %d opening %s: %s", errno, portname, strerror (errno));
            return -1;
    }

    set_interface_attribs (fd, B2400, 0);  // set speed to 2400 bps, 8n1 (no parity)
    set_blocking (fd, 0);                // set no blocking

    unsigned char buf[100];
    unsigned int i=0;
    unsigned int sum=0;
    sum += buf[i++] = 13;
    sum += buf[i++] = 6;
    sum += buf[i++] = 'B';
    sum += buf[i++] = 79;
    buf[i++] = ~sum + 1;
    sum=0;
    sum += buf[i++] = 13;
    sum += buf[i++] = 5;
    sum += buf[i++] = 'B';
    sum += buf[i++] = 111;
    buf[i++] = ~sum + 1;
    sum=0;
    sum += buf[i++] = 13;
    sum += buf[i++] = 4;
    sum += buf[i++] = 'B';
    sum += buf[i++] = 193;
    buf[i++] = ~sum + 1;
    sum=0;
    sum += buf[i++] = 13;
    sum += buf[i++] = 3;
    sum += buf[i++] = 'B';
    sum += buf[i++] = 193;
    buf[i++] = ~sum + 1;
    sum=0;
    sum += buf[i++] = 13;
    sum += buf[i++] = 2;
    sum += buf[i++] = 'B';
    sum += buf[i++] = 237;
    buf[i++] = ~sum + 1;
    sum=0;
    sum += buf[i++] = 13;
    sum += buf[i++] = 1;
    sum += buf[i++] = 'B';
    sum += buf[i++] = 28;
    buf[i++] = ~sum + 1;
    
    write (fd, buf, i);
    usleep (100*1000);
    
    unsigned char buf2[10];
    i=0, sum=0;
    sum += buf2[i++] = 13;
    sum += buf2[i++] = 8;
    sum += buf2[i++] = 'S';
    sum += buf2[i++] = 1;
    buf2[i++] = ~sum + 1;
    
    write (fd, buf2, i);

    close(fd);
    
    return 0;
}
