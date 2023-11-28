/*
    source: 	www.csl.mtu.edu/cs4411.ck/www/NOTES/process/fork/exec.html
*/

#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX 1024

void  parse(char *line, char **argv)
{
     while (*line != '\0') { 	/* if not the end of line */ 
          while (*line == ' ' || *line == '\t' || *line == '\n')
               *line++ = '\0';  /* replace white spaces with 0*/
          *argv++ = line;       /* save the argument position */
          while (*line != '\0' && *line != ' ' && 
                 *line != '\t' && *line != '\n') 
               line++;      /* skip the argument until ...    */
     }
     *argv = '\0';          /* mark the end of argument list  */
}

void  execute(char **argv)
{
     pid_t  pid;
     int    status;

     if ((pid = fork()) < 0) {    /* fork a child process    */
          printf("*** ERROR: forking child process failed\n");
          exit(1);
     }
     else if (pid == 0) {          /* for the child process: */
          if (execvp(*argv, argv) < 0) {     /* exec command  */
               printf("*** ERROR: exec failed\n");
               exit(1);
          }
     }
     else {                            /* for the parent: */
          while (wait(&status) != pid) /* wait for completion */
               ;
     }
}


void  main(void)
{
     char  line[MAX];      /* the input line                 */
     char  *argv[64];       /* the command line argument      */

     while (1) {            /* repeat until done ....         */
          printf("Shell -> ");   /* display a prompt          */
	    if (fgets(line, MAX, stdin) != 0){
	       line[strcspn(line, "\n")] = '\0';
          	  printf("\n");

	// Place your code here to check the command is "exit"
     //   to terminate the shell.
	   if (strcmp(line, "exit") == 0) {
           	printf("Exiting the shell...\n");
              	exit(0);
          	  } 


          	  parse(line, argv);     /* parse the line      */
		  execute(argv);        /* otherwise, execute cmd  */
         }  
     }
}