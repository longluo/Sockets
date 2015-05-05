#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>

// to do
// chance PF_INET to PF_UNIX will have to change address
// no more ip at this point...binding will be different too

#define ERR_CONN_FAILED (2)
#define PORT (5842)
#define OTW_ADDR "69.55.233.89"

void dump_hex(void *buffer, int len);

struct four_ints {
  unsigned int int1;
  unsigned int int2;
  unsigned int int3;
  unsigned int int4;
};


int main(int argc, char *argv[])
{
    int sock;
    sock = socket(PF_INET,SOCK_STREAM,0);
    if (sock < 0) 
    {
        exit(1);
    }
    
    int connection;
    char buffer[256];
    struct sockaddr_in *address = (struct sockaddr_in*) buffer;

    // convert IP to byte rep and put in the sockaddr_in address struct
    if( inet_pton(PF_INET, OTW_ADDR, &address[0].sin_addr) != 1 ) 
    {
        printf("inet_pton failed errno = %i \n", errno);
        exit(3);
    }

  // populate arguments to connect function
  address[0].sin_family = PF_INET;
  address[0].sin_port = htons(PORT);
  
  // establish a connection
  connection = connect(sock,(struct sockaddr*) address,sizeof(struct sockaddr_in));
  if (connection != 0)
  {
    printf("connection failed with errno: %i \n", errno);
    exit(ERR_CONN_FAILED);
  }

  // read ints transform and print
  struct four_ints data;
  memset(&data, 0, sizeof(data));
  int y = 0;
  int index = 0;
  int temp;
  int *idata = (int *) &data; 
  while ( index < 4 )
  {
    y = y + read(sock,idata + index++,sizeof(unsigned int));
  }
  printf("received ints: %u,%u,%u,%u \nbytes received: %i \n", data.int1,data.int2,data.int3,data.int4,y);
  dump_hex(&data,sizeof(data));

  // get ready to add up the ints and send
  int *pdata = (int*) &data;
  unsigned long long sum = 0;
  for (index = 0; index < 4; index++)
  { 
    sum = sum + (unsigned long long) pdata[index];
  }
 
  // send the sum of the ints
  dump_hex(&sum,sizeof(sum));
  write(sock,&sum,sizeof(sum));

  // read the message from the server
  char info[64];
  read(sock,info,sizeof(info));
  printf("%s\n",info);

  return 0;
}


void dump_hex(void * buffer, int len) {
    unsigned char * p = (unsigned char *) buffer;
    int i;
    for(i=0;i<len;i++) 
        printf("%02X ", p[i]);
    printf("\n");
}