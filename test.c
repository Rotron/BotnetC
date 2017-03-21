#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
void execcmd(char *cmd)
{
	  FILE *fp;
 	  char path[1035];

 	  /* Open the command for reading. */
	  fp = popen(cmd, "w");
	  if (fp == NULL) {
	    printf("Failed to run command\n" );
	    exit(1);
	  }

	  /* Read the output a line at a time - output it. */
	  while (fgets(path, sizeof(path)-1, fp) != NULL) {
	    printf("%s", path);
	  }

	  /* close */
	  pclose(fp);
}
int main( int argc, char *argv[] )
{

  char *cmd;
  int exit=1;

	while(exit)
  {
		fflush(stdin);
		printf("Enter a command:\n");
		cmd=(char *) malloc(100);
		fgets(cmd,50,stdin);
		execcmd(cmd);

		if (strcmp(cmd,"exit")==10)
		{
			exit=0;
		}
		free(cmd);
  }

  return 0;
}
