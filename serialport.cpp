#include <stdio.h>      // standard input / output functions
#include <stdlib.h>
#include <string.h>     // string function definitions
#include <unistd.h>     // UNIX standard function definitions
#include <fcntl.h>      // File control definitions
#include <errno.h>      // Error number definitions
#include <termios.h>    // POSIX terminal control definitions
#include <iostream>
using namespace std;
#define TIMEOUT_SEC(buflen,baud) (buflen*20/baud+2)  //接收超时
#define TIMEOUT_USEC 0

int flag = 1;

int main()
{

    int readlen, fs_sel;
    fd_set	fs_read;
    struct timeval tv_timeout;

    FD_ZERO(&fs_read);

    tv_timeout.tv_sec = TIMEOUT_SEC(256, 9600);
    tv_timeout.tv_usec = TIMEOUT_USEC;
    int n;

/* Open File Descriptor */
int USB = open( "/dev/ttyUSB0", O_RDWR| O_NONBLOCK | O_NDELAY );

/* Error Handling */
 if ( USB < 0 )
 {
  cout << "Error " << errno << " opening " << "/dev/ttyUSB0" << ": " << strerror (errno) << endl;
 }

/* *** Configure Port *** */
struct termios tty;
memset (&tty, 0, sizeof tty);

/* Error Handling */
if ( tcgetattr ( USB, &tty ) != 0 )
{
cout << "Error " << errno << " from tcgetattr: " << strerror(errno) << endl;
}

/* Set Baud Rate */
cfsetospeed (&tty, B9600);
cfsetispeed (&tty, B9600);

/* Setting other Port Stuff */
tty.c_cflag     &=  ~PARENB;        // Make 8n1
tty.c_cflag     &=  ~CSTOPB;
tty.c_cflag     &=  ~CSIZE;
tty.c_cflag     |=  CS8;
tty.c_cflag     &=  ~CRTSCTS;       // no flow control
tty.c_lflag     =   0;          // no signaling chars, no echo, no canonical processing
tty.c_oflag     =   0;                  // no remapping, no delays
tty.c_cc[VMIN]      =   0;                  // read doesn't block
tty.c_cc[VTIME]     =   5;                  // 0.5 seconds read timeout

tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines
tty.c_iflag     &=  ~(IXON | IXOFF | IXANY);// turn off s/w flow ctrl
tty.c_lflag     &=  ~(ICANON | ECHO | ECHOE | ISIG); // make raw
tty.c_oflag     &=  ~OPOST;              // make raw

/* Flush Port, then applies attributes */
tcflush( USB, TCIFLUSH );

if ( tcsetattr ( USB, TCSANOW, &tty ) != 0)
{
cout << "Error " << errno << " from tcsetattr" << endl;
}

/* *** WRITE *** */

unsigned char cmd[] = {'1', '\r', '\0'};
int n_written = write( USB, cmd, sizeof(cmd) -1 );

/* Allocate memory for read buffer */
char buf [256];
memset (&buf, '\0', sizeof buf);
FD_SET(USB, &fs_read);
fs_sel = select(USB+1, &fs_read, NULL, NULL, &tv_timeout);
/* *** READ *** */
while(1)
{
if(flag == 2)
{
	break;
}
if(fs_sel){
        //readlen = read(fdcom, data, datalen);
        n = read( USB, &buf , sizeof buf );
        //return(readlen);
}


/* Error Handling */
//if (n < 0)
//{
    // cout << "Error reading: " << strerror(errno) << endl;
//}

/* Print what I read... */
if(n>0 && flag == 1)
{
//cout << "Read: " << buf << endl;
    printf("%c\n",buf[0] );
    if(buf[0] == 'k')
    {
	pid_t pid = fork();
	switch(pid) {
   	case 0: // we are the child process
		flag = 2;
      		execl( "BackGroundTest","BackGroundTest","1","30","Video.avi",NULL ); //exact signautre differs
		
      		break;
   	case -1: //An error ocurred
      		cout << "error\n";
		return 0;
		    }
    }
}

}
close(USB);
return 0;
}


/*int main(int argc, char const *argv[])
{
	vector<int> test ;
	test.push_back(22); test.push_back(7); test.push_back(2); test.push_back(15);
	vector<int> result = twoSum(test,9);
	for (int i = 0; i < result.size(); ++i)
	{
		printf("%d\n",result[i] );
	}
	//system("gnome-terminal -e ./BackGroundTest 1 30 Video.avi");
	pid_t pid = fork();
	switch(pid) {
   	case 0: // we are the child process
      		execl( "BackGroundTest","BackGroundTest","1","30","Video.avi",NULL ); //exact signautre differs
      		break;
   	case -1: //An error ocurred
      		cout << "error\n";
}
	return 0;
}*/
