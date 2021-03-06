#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include  <error.h>


#include "owlzilla_hdr.h"
#define BUFFER_SIZE 2000

#define PORT_MODE 0
#define ADDR_MODE 1
#define HOST_MODE 2
#define HELP_MODE 3


void HelpFunct(void);
void ClientHelp(void);
//Program overview
/********************
* This program makes requests to the server.
*   It can order the server to change directories
*   and print its current directory.
*   The user can also request a help screen
*   if they need it by passing -h argument when
*   calling the program.
*   Next Phase: Will recieve files from server.
*
*********************/
int main(int argc, char * argv[])
{
//extern char * optarg;
    char command[BUFFER_SIZE*2] = {0}, dirBuf[BUFFER_SIZE*2 ] ={0};
    int opt, verbFlag=0, helpFlag= 0, mode = 0, port =10001, sockFD;
    char ipAddr[50]="66.191.25.220";
    char * token;
    while((opt=getopt(argc, argv, "p:i:hv"))!=-1)
    {
        switch(opt)
        {
        case 'p':
            port= atoi(optarg);
            mode++;
            break;

        case 'i':
            strcpy(ipAddr, optarg);
            mode++;
            break;

        case 'h':
            helpFlag=1;
            break;

        case 'v':
            verbFlag=1;
            if (verbFlag==1)
                printf("\nVerbose mode selected\n");
            break;

        default:
            fprintf(stderr, "Invalid option");
            break;

        }
    }
    if(helpFlag == 1)
        HelpFunct();

    else
    {
        while(1)
        {
            printf(PROMPT);
            fgets(command, BUFFER_SIZE, stdin);
            command[strlen(command)-1] = 0;

            token = strtok(command, " ");


            if ((token==NULL) || (strcmp(token, "exit")==0))
                break;

            else if(strcmp(token, "cd")==0)
            {
                token = strtok(NULL, " ");
                if (token != NULL)
                {
                    if (chdir(token)==-1)
                        fprintf(stderr, "\nInvalid directory Name\n");
                }
                else
                    if (chdir(getenv("HOME"))==-1)
                        fprintf(stderr, "\nUnable to change directory\n");
            }
            else if (strcmp(token,"pwd")==0)
            {
                memset((void*)&command, 0 , sizeof(command));
                printf("\n%s\n", (char*)getcwd(command, sizeof(command)));
            }
            else if (strcmp(token, "help")==0)
                ClientHelp();

            else //if ((strcmp(token, "lcd")==0) || (strcmp(token, "lpwd")==0))
            {
                struct sockaddr_in clientSock;
                memset(&clientSock, 0, sizeof(clientSock));

                if (verbFlag==1)
                    printf("\nsetting mode to INET\n");

                clientSock.sin_family = AF_INET;
                if (verbFlag == 1)
                    printf("\nhtons config occurring\n");
                clientSock.sin_port = htons((short)port);
                if (verbFlag==1)
                 inet_pton(AF_INET, ipAddr, &clientSock.sin_addr.s_addr);


                sockFD = socket(AF_INET, SOCK_STREAM, 0);
                if (verbFlag==1)
                    printf("\nSocket set, ready to Connecting to socket\n");
                connect(sockFD, (struct sockaddr*)&clientSock,sizeof(clientSock));
                memset(dirBuf, 0, sizeof(BUFFER_SIZE));

                strncpy(dirBuf, token, strlen(token));
                token= strtok(NULL, " ");
                if (token!= NULL)
                   sprintf(dirBuf,"%s %s", dirBuf, token);
                //printf("Client dir difference: %d", strcmp(dirBuf, "dir"));
                printf("\nHere goes msg: %s \n", dirBuf);
                write(sockFD, dirBuf, strlen(dirBuf));

                sleep(1);       //sleep!

                memset((void*)dirBuf, 0, strlen(dirBuf));
                while((read(sockFD, dirBuf, sizeof(dirBuf)))>0)
                {
                   write(STDOUT_FILENO, dirBuf, strlen(dirBuf));
                   memset((void*)dirBuf, 0, sizeof(dirBuf));
                }
            }
                //close(sockFD);
            memset((void*)&command, 0 , strlen(command));
        }
    }
return 0;
}



void HelpFunct(void)
{
    printf("\n\t---ClientCommands---\n");
    printf("\t'-p <port>' -- Specify <port> for server to use\n");
    printf("\t'-i <ip-address>'  -- Specify <ip-address> for server to use\n");
    printf("\t'-h'-- Help menu\n");
    printf("\t'-v' -- Enable verbose mode\n");
    printf("\t'-H' <hostname>' -- Specify <hostname> for server to use\n");
}


void ClientHelp(void)
{
    printf("\n---Client Commands---");
    printf("\n'pwd' -- Print working directory");
    printf("\n'cd <directory>' -- Change working server directory");
    printf("\n'lcd <directory>' -- Change working client directory");
    printf("\n'help' -- Help menu\n");
}

