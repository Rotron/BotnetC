#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<string.h>
#include<netinet/in.h>

#define PORT 9999
#define BUF_SIZE 2000
#define CLADDR_LEN 100
/*
void execcmd(char *cmd)
{
	  FILE *fp;
 	  char path[1035];

 	  //Open the command for reading.
	  fp = popen(cmd, "w");
	  if (fp == NULL) {
	    printf("Failed to run command\n" );
	    exit(1);
	  }

	  //Read the output a line at a time - output it.
	  while (fgets(path, sizeof(path)-1, fp) != NULL) {
	    printf("%s", path);
	  }

	  //close/
	  pclose(fp);
}


int main( int argc, char *argv[] )
{

  char *cmd;
  int exit=1;
	int i;
	while(exit)
  {
		fflush(stdin);

		printf("Enter a command:\n");
		cmd=(char *) malloc(100);
		fgets(cmd,100,stdin);
		cmd[strcspn(cmd,"\r\n")]='\0';
		execcmd(cmd);
		printf("%s",cmd);
		if (strcmp(cmd,"exit")==0)
		{
			free(cmd);
			exit=0;
		}
		free(cmd);
  }

  return 0;
}
*/
void main()
{
	 struct sockaddr_in addr, cl_addr;
	 int sockfd, len, ret, newsockfd;
	 char buffer[BUF_SIZE];
	 pid_t childpid;
	 char clientAddr[CLADDR_LEN];

	 sockfd = socket(AF_INET, SOCK_STREAM, 0);
	 if (sockfd < 0)
	 {
		  printf("Error creating socket!\n");
		  exit(1);
	 }
	 printf("Socket created...\n");

	 memset(&addr, 0, sizeof(addr));
	 addr.sin_family = AF_INET;
	 addr.sin_addr.s_addr = INADDR_ANY;
	 addr.sin_port = PORT;

	 ret = bind(sockfd, (struct sockaddr *) &addr, sizeof(addr));
	 if (ret < 0)
	 {
		  printf("Error binding!\n");
		  exit(1);
	 }
	 printf("Binding done...\n");

	 printf("Waiting for a connection...\n");
	 listen(sockfd, 5);

	 for (;;) { //infinite loop
	  len = sizeof(cl_addr);
	  newsockfd = accept(sockfd, (struct sockaddr *) &cl_addr, &len);
	  if (newsockfd < 0)
		{
		   printf("Error accepting connection!\n");
		   exit(1);
	  }
	  printf("Connection accepted...\n");
		inet_ntop(AF_INET, &(cl_addr.sin_addr), clientAddr, CLADDR_LEN);
	  if ((childpid = fork()) == 0)
		{ //creating a child process
	   close(sockfd);
		 //stop listening for new connections by the main process.
		 //the child will continue to listen.
		 //the main process now handles the connected client.

	   for (;;) {
	    memset(buffer, 0, BUF_SIZE);
	    ret = recvfrom(newsockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, &len);
	    if(ret < 0) {
	     printf("Error receiving data!\n");
	     exit(1);
	    }
	    printf("Received data from %s: %s\n", clientAddr, buffer);

	    ret = sendto(newsockfd, buffer, BUF_SIZE, 0, (struct sockaddr *) &cl_addr, len);
	    if (ret < 0) {
	     printf("Error sending data!\n");
	     exit(1);
	    }
	    printf("Sent data to %s: %s\n", clientAddr, buffer);
	   }
	  }
	  close(newsockfd);
	 }
}
