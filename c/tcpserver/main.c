/****************** SERVER CODE ****************/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <arpa/inet.h>

char *gettimestr(char *s)
{
  //char s[24];
  char tstr[20];
  struct timeval tv;
  gettimeofday(&tv, NULL);
  struct tm *p = localtime(&tv.tv_sec);
  strftime(tstr, 32, "%FT%T", p);
  snprintf(s, 24, "%s.%03d", tstr, (int)(tv.tv_usec/1000.0));
  return s;
}

ssize_t myrecv(int fd)
{
  int i;
  char tstr[32];
  char buffer[1025];
  ssize_t ss;
  ssize_t ttl = 0;

  for (i = 0; i < 10; i++) {
    ss = recv(fd, buffer, 1024, MSG_DONTWAIT);
    if (ss <= 0) { 
      if (ttl <= 0) {
        usleep(200000); 
      }
      usleep(100000); 
      continue; 
    }
    i = 0;
    buffer[ss] = '\0';
    ttl += ss;

    printf("%s %04d-%s(%d) size %04d total %5d\n", gettimestr(tstr), __LINE__, __FUNCTION__, fd, ss, ttl);
    //printf("%s\n", buffer);
    fflush(stdout);
  }
  return ttl;
}

int main(){
  int rc;
  int newSocket;
  socklen_t addr_size;
  struct sockaddr_storage serverStorage;
  int welcomeSocket;
  struct sockaddr_in serverAddr;
  unsigned short port = 8080;
  char ipaddr[] = "0.0.0.0";
    char tstr[32];
  char buffer[1024];
  int pid;

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
  int enable = 1;
  if (setsockopt(welcomeSocket, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    fprintf(stderr, "setsockopt(SO_REUSEADDR) failed\n");
  
  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(port);
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr(ipaddr);
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*---- Bind the address struct to the socket ----*/
  rc = bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
  if (rc != 0) {
    fprintf(stderr, "%04d-%s() rc(%d) %s\n", __LINE__, __FUNCTION__, rc, strerror(errno));
    return 0;
  }

  /*---- Listen on the socket, with 5 max connection requests queued ----*/
  if(listen(welcomeSocket,5)==0)
    printf("%s Listening(%s:%d)\n", gettimestr(tstr), ipaddr, port);
  else
    printf("Error\n");
  fflush(stdout);
 for (;;) {

  /*---- Accept call creates a new socket for the incoming connection ----*/
  addr_size = sizeof serverStorage;
  newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);

  {
  /*---- Send message to the socket of the incoming connection ----*/
  pid = fork();
  if (pid == -1) {
    fprintf(stderr, "%04d-%s() pid(%d)\n", __LINE__, __FUNCTION__, pid);
    close(newSocket);
    continue;
  } else if (pid > 0) {
    //fprintf(stderr, "%s %04d-%s() pid(%d) fd(%d)\n", __TIME__, __LINE__, __FUNCTION__, pid, newSocket);
    continue;
  } else {

  ssize_t ttl;
  printf("%s %04d-%s() pid(%d) fd(%d)\n", gettimestr(tstr), __LINE__, __FUNCTION__, pid, newSocket);
  fflush(stdout);
  ttl = myrecv(newSocket);

  strncpy(buffer,"HTTP/1.1 200 OK\r\nConnetion: close\r\nContent-Length: 0\r\n\r\n", sizeof(buffer));
  //strncpy(buffer,"HTTP/1.1 200 OK\r\n\r\n", sizeof(buffer));
  send(newSocket, buffer, strlen(buffer), 0);
  close(newSocket);
    printf("%s %04d-%s() pid(%d) fd(%d) total %d closed()\n", gettimestr(tstr), __LINE__, __FUNCTION__, pid, newSocket, ttl);
    fflush(stdout);
    break;
  }
  }
 }
  return 0;
}
