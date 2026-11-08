#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <wait.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#define BSIZE 100

/***********************************************************************
 * This starter shell implements a simple shell that implements:
 * 
 * (1) An internal EXIT command that exits the shell.
 * (2) Executes a external program without any command line arguments
 * 
 ***********************************************************************/

pid_t runnin_child = 0;

void handle_sigint(int signum) {
    if (runnin_child > 0) kill(runnin_child, SIGINT);
    printf("\n");
}

void split_tokens(char buffer[], char *tokens[]) {
    int i = 1;
    tokens[0] = buffer;
    for (int j = 0; buffer[j] != '\0'; j++) {
        if (buffer[j] == ' ') {
            buffer[j] = '\0';
            tokens[i] = &buffer[j + 1];
            i++;
        }
    }
    tokens[i] = NULL;
}

int main(int argc, char *argv[]){
    char buffer[BSIZE];
    pid_t child;
    int status;
    char *args[]={"",NULL};
    char *tokens[BSIZE];

    signal(SIGINT, handle_sigint);

    while(1){
        printf("shell> ");
        fgets(buffer,BSIZE-1,stdin);
        buffer[(strlen(buffer)-1)]='\0';  //Remove NewLine
        args[0]=buffer;

        split_tokens(buffer, tokens);
        if (tokens[0] == NULL) continue;

        //** Internal Command **
        if(strcmp(buffer,"exit")==0){
            break;
        }

        if (strcmp(buffer, "cd") == 0) {
            chdir(tokens[1]);
            continue;
        }

        //** External Command **
        child = fork();
        if(child==0){
            //*** Child Process ***
            execvp(tokens[0], tokens);
            execvp(args[0],args);
            exit(0);
        }else{
            //*** Parent Process **
            runnin_child = child;
            waitpid(-1,&status,0);
            runnin_child = 0;
        }

    }
    return 0;
}