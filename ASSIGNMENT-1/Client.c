#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <signal.h>


bool EXIT_FL=true;


void * doRecieving(void * sockID){

	int clientSocket = *((int *) sockID);

	while(1){

		char data[1024];
		int read = recv(clientSocket,data,1024,0);
		data[read] = '\0';
		printf("%s\n",data);

	}

}


void catch(int dummy)
	{
		//EXIT_FL=false;
     	char  c;

     	signal(dummy, SIG_IGN);
     	printf("Did you hit Ctrl-C?\n"
            "Do you really want to quit? [y/n] ");
     	c = getchar();
     	if (c == 'y' || c == 'Y')
        	exit(0);
     	else
        	signal(SIGINT, catch);
     	getchar(); // Get new line character
	}

int main(int argc,char *argv[]){

	int portno;
	char username[100];

	if(argc > 3) {
		printf("too many arguments");
		exit(1);
	}
	portno = atoi(argv[2]);
	strcpy(username,argv[1]);

	int clientSocket = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(portno);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	if(connect(clientSocket, (struct sockaddr*) &serverAddr, sizeof(serverAddr)) == -1) return 0;

	printf("Connection established ............\n");
	
	send(clientSocket,username,1024,0);

	pthread_t thread;
	pthread_create(&thread, NULL, doRecieving, (void *) &clientSocket );

	signal(SIGINT, catch);

	while(EXIT_FL){

		char input[1024];

		//if(!EXIT_FL)
		//{	
		//	printf("%s\n","Bye-Bye");
			//recv(clientSocket,"Bye-Bye",1024,0);	
			//send(clientSocket,"Bye-Bye",1024,0);
		//	break;
		//}

		scanf("%s",input);

		if(strcmp(input,"SEND") == 0){

			send(clientSocket,input,1024,0);
			
			scanf("%s",input);
			send(clientSocket,input,1024,0);
			
			scanf("%[^\n]s",input);
			send(clientSocket,input,1024,0);

		}
		if(strcmp(input,"SENDGROUP") == 0){

			send(clientSocket,input,1024,0);
			
			scanf("%s",input);
			send(clientSocket,input,1024,0);

			scanf("%[^\n]s",input);
			send(clientSocket,input,1024,0);

		}
		if(strcmp(input,"CREATEGROUP") == 0){

			send(clientSocket,input,1024,0);
			
			scanf("%s",input);
			send(clientSocket,input,1024,0);
			
			scanf("%s",input);
			send(clientSocket,input,1024,0);

			int p=atoi(input);
			for( int i=0;i<p;i++)
			{
			scanf("%s",input);
			//printf("n=%d",atoi(input));			
			send(clientSocket,input,1024,0);
			}
		}
	}
}
