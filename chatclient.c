/*Name: Alan Macabuhay
 *Date: 10/18/18
 *Description: A simple socket client for a chat program
 *References: 
 * (1)https://beeg.us/guide/bgnet/html/multi/clientserver.html#simpleclient
 * (2)https://stackoverflow.com/questions/7880141/how-do-i-check-length-of-user-input-in-c
 * (3)https://www.geeksforgeeks.org/signals-c-language/
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <signal.h>

#define SIGINT 2


//function declarations
void chat(int, char*, char*);
void getInput(char*, char*, int);
void handshake(int, char*, char*);
void handle_sigint(int);
int socketHelper(struct addrinfo*);

//main function
int main(int argc, char*argv[]) {
	int sockfd, rv;
	char serverName[11];
	char userName[11];
	struct addrinfo hints, *servinfo, *p;

	//check for correct arguments
	if(argc != 3) {
		fprintf(stderr, "Error: incorrect number of arguments\n");
		exit(1);
	}

	memset(serverName, '\0', sizeof(serverName));
	memset(userName, '\0', sizeof(userName));


	//capture SIGINT. (3)
	signal(SIGINT, handle_sigint);

	//set up server struct. (1)
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	//get address info. (1)
	if((rv = getaddrinfo(argv[1], argv[2], &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		exit(1);
	}


	//connect sockets
	sockfd = socketHelper(servinfo);

	//get client username
	getInput("Enter a username with maximum length of 10 characters", userName, 10);

	//exchange handles with server
	handshake(sockfd, userName, serverName);

	//start chatting
	chat(sockfd, userName, serverName);

	//done with structure. (1)
	freeaddrinfo(servinfo);

	return 0;
}

//function that handles chatting
//parameters: sockfd, userName, serverName
void chat(int sockfd, char *userName, char *serverName) {
	char readBuffer[500];
	char writeBuffer[500];
	int bytes;
	int status;

	memset(readBuffer, '\0', sizeof(readBuffer));
	memset(writeBuffer, '\0', sizeof(writeBuffer));

	bytes = 0;

	//keep chatting until user quits
	while(1) {
		//get msg from user
		getInput(userName, writeBuffer, 500);

		if(strcmp(writeBuffer, "\\quit") == 0) {
			printf("Ending connection with %s\n", serverName);
			break;
		}

		//send message to server
		bytes = send(sockfd, writeBuffer, sizeof(writeBuffer), 0);

		if(bytes == -1){
			fprintf(stderr, "Error sending message\n");
			exit(1);
		}

		//receive message from server
		status = recv(sockfd, readBuffer, sizeof(readBuffer), 0);

		if(status == -1) {
			fprintf(stderr, "Error receiving message\n");
			exit(1);

		}else if(status == 0){
			printf("%s has ended connection\n", serverName);
			break;

		}else {
			//message received, display it
			printf("%s> %s\n", serverName, readBuffer); 
		}

		//clear buffers
		memset(writeBuffer, '\0', sizeof(writeBuffer));
		memset(readBuffer, '\0', sizeof(readBuffer));
	}	

	close(sockfd);
	printf("Connection now closed.\n");
}

//function that gets user input. validates length. (2)
//parameters: question prompt, buffer, size of buffer
void getInput(char *question, char *buffer, int size) {
	
	//prompt
	printf("%s> ", question);
	fgets(buffer, size + 1, stdin);

	if(buffer[strlen(buffer) - 1] != '\n') {
	
		int dropped = 0;
		while(fgetc(stdin) != '\n')
			dropped++;

		//validating length
		if(dropped > 0) {
			getInput(question, buffer, size);
		}

	}else {
		//remove trailing newline
		buffer[strlen(buffer) - 1] = '\0';
	}
}


//function that exchanges handle info between client and server
//parameters: sockfd, userName, serverName buffer
void handshake(int sockfd, char *userName, char *serverName) {
	
	int sendingStatus, recvStatus;

	//client-server handshake
	sendingStatus = send(sockfd, userName, strlen(userName), 0);
	if(sendingStatus < 0) {
		fprintf(stderr, "Client: Error writing to socket\n");
		exit(1);
	}
	
	if(sendingStatus < strlen(userName)) {
		printf("Warning: Not all data written to socket\n");
	}

	//client-server handshake
	recvStatus = recv(sockfd, serverName, sizeof(serverName), 0);
	if(recvStatus < 0) {
		fprintf(stderr, "Client: Error reading from socket");
		exit(1);
	}

	printf("Now connected with %s\n", serverName);


}

//helper function to connect sockets
//parameters: *servinfo
//returns sockfd
int socketHelper(struct addrinfo *servinfo) {

	int sockfd;

	//create socket. (1)
	if((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1 ) {
		fprintf(stderr, "Client error: cannot create socket\n");
		exit(1);
	}

	//connect socket. (1)
	if(connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1) {
		fprintf(stderr, "Client error: cannot connect to socket\n");
		exit(1);
	}

	return sockfd;
}

//function that handles SIGINT. (3)
//parameters: sig
void handle_sigint(int sig) {
	printf("Caught signal %d, exiting program\n", sig);
	exit(0);
}
