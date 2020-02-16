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

int clientCount = 0;
int groupCount = 0;


static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct client{

	int index;
	char username[100];
	int sockID;
	struct sockaddr_in clientAddr;
	int len;

};

struct group{

	int index;
	int no_people;
	char username_g[100];
	int people[1024];

};


struct client Client[1024];
struct group Group[1024];

pthread_t thread[1024];

void * doNetworking(void * ClientDetail){

	struct client* clientDetail = (struct client*) ClientDetail;
	int index = clientDetail -> index;
	int clientSocket = clientDetail -> sockID;
	char username[100];
		
	recv(clientSocket,username,1024,0);
	//printf("Clinet %s connected", username);

	//strcpy(username,clientDetail -> username);

	printf("%s with ID : %d, connected.\n",username,index + 1);
	
	
	while(1){

		char data[1024];
		int read = recv(clientSocket,data,1024,0);
		data[read] = '\0';

		char output[1024];

/*
		if(strcmp(data,"LIST") == 0){

			int l = 0;

			for(int i = 0 ; i < clientCount ; i ++){

				if(i != index)
					l += snprintf(output + l,1024,"Client %d is at socket %d.\n",i + 1,Client[i].sockID);

			}

			send(clientSocket,output,1024,0);
			continue;

		}
*/
		if(strcmp(data,"SEND") == 0){
			read = recv(clientSocket,data,1024,0);
			data[read] = '\0';

			int id = atoi(data) - 1;
			
			read = recv(clientSocket,data,1024,0);
			data[read] = '\0';

			send(Client[id].sockID,data,1024,0);			

		}

		if(strcmp(data,"SENDGROUP") == 0){			
			read = recv(clientSocket,data,1024,0);
			data[read] = '\0';

			int id = atoi(data) - 1;
					
			read = recv(clientSocket,data,1024,0);
			data[read] = '\0';
			
			int k=0;
			for(int i = 0 ; i <= Group[id].no_people; i ++){
				if (index == (Group[id].people)[i])
				{
					k ++;
				}
			}
			
			
			if (k>=1)
			{
				char tem[100];
				//snprintf( tem, 100, "%s", Client[index].username);
				strcpy(tem,username);
				strcat(tem,"(");
				strcat(tem,Group[id].username_g);
				strcat(tem,")");				
				strcat(tem,":");
				strcat(tem,data);
				for(int i = 0 ; i < Group[id].no_people; i ++){
					send(Client[(Group[id].people)[i]-1].sockID,tem,1024,0);
				}
			}
			else	
			{
				char data1[18]={'Y','o','u',' ','h','a','v','e',' ','n','o',' ','a','c','c','e','s','s'};
				send(Client[index].sockID,data1,1024,0);
			}			

		}

		if(strcmp(data,"CREATEGROUP") == 0){
			//printf("here");
			read = recv(clientSocket,data,1024,0);
			data[read] = '\0';
			strcpy(Group[groupCount].username_g,data);			
			
			read = recv(clientSocket,data,1024,0);
			data[read] = '\0';


			int num_of_group = atoi(data);
			Group[groupCount].index=groupCount;
			Group[groupCount].no_people=num_of_group;


			for(int i = 0 ; i < num_of_group ; i ++){
				read = recv(clientSocket,data,1024,0);
				data[read] = '\0';
				(Group[groupCount].people)[i]=atoi(data);
			}						
			groupCount ++;
			//printf("Groupcreated");
			//printf("details=%d , %d %d %d",Group[groupCount-1].index,Group[groupCount-1].no_people,(Group[groupCount-1].people)[0],(Group[groupCount-1].people)[1]);
			//pthread_create(&thread[clientCount+groupCount], NULL, doNetworking, (void *) &Group[GroupCount]);
		}



	}

	return NULL;

}

int main(int argc,char *argv[]){

	int portno;

	if(argc > 3) {
		printf("too many arguments");
		exit(1);
	}
	portno = atoi(argv[1]);

	int serverSocket = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serverAddr;

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(portno);
	serverAddr.sin_addr.s_addr = htons(INADDR_ANY);


	if(bind(serverSocket,(struct sockaddr *) &serverAddr , sizeof(serverAddr)) == -1) return 0;

	if(listen(serverSocket,1024) == -1) return 0;

	printf("Server started listenting on port %d ...........\n",portno);

	while(1){

		Client[clientCount].sockID = accept(serverSocket, (struct sockaddr*) &Client[clientCount].clientAddr, &Client[clientCount].len);
		Client[clientCount].index = clientCount;


		pthread_create(&thread[clientCount+groupCount], NULL, doNetworking, (void *) &Client[clientCount]);

		clientCount ++;
 
	}

	for(int i = 0 ; i < clientCount ; i ++)
		pthread_join(thread[i],NULL);

}
