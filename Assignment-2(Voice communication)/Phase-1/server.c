#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <errno.h>
#include <fcntl.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#define BUFSIZE 10000

// Main loop
int main(int argc, char const *argv[]) 
{ 
	// PulseAudio details
	static const pa_sample_spec ss = {
		.format = PA_SAMPLE_S16LE,
		.rate = 20000,
		.channels = 1
	};
	pa_simple *s = NULL;
	int ret = 1;
	int error;
	
	/* Creating a playback stream */
        if (!(s = pa_simple_new(NULL, argv[0], PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) {
		fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        }
	
	// Socket creation
	int server_fd, new_socket, valread; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) return -1; 					// Socket file descriptor
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,&opt, sizeof(opt))) return -1; 	// attaching
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons(6000); 									// Port address
	if (bind(server_fd, (struct sockaddr *)&address,sizeof(address))<0) return -1; 				// Binding
	if (listen(server_fd, 3) < 0)  return -1; 								// Start listening
	int ctr = 0;
	// Run forever
	while(1){
		//printf("ggi");
		if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen))>=0){ // Wait
				while(1){
					// New request to read data
					//printf("hhi");
					uint8_t buffer[BUFSIZE]; 
					valread = read( new_socket , buffer, sizeof(buffer));
					printf("Re %s\n", buffer);  
					printf("Recieved %d\n", ctr++); 
					
					
					if (pa_simple_write(s, buffer, sizeof(buffer), &error) < 0) {
					    fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
					} 

				}
		}
	}
	return 0; 
} 

