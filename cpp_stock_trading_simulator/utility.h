/* This file contains header file inclusions and macro definitions used throughout the project.
 * It also defines a generic function for setting up a UDP socket for each of the four servers.
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
#include <arpa/inet.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <signal.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cctype>
#include <unordered_map>
#include <map>
#include <vector>
using namespace std;

#define STDIN 0
// Static port number allocation
#define PORT_A "41710"
#define PORT_P "42710"
#define PORT_Q "43710"
#define PORT_M_UDP "44710"
#define PORT_M_TCP "45710"

#define LOCALHOST "127.0.0.1"

#define MAXBUFSIZE 8196
#define BACKLOG 10 // number of pending connections at TCP server side 

/*
 * This function is cited from Beej's Guide to Network Programming.
 * Handle dead processes.
 */
void sigchld_handler(int s) {
    int saved_errno = errno;
    while (waitpid(-1, NULL, WNOHANG) > 0);
    errno = saved_errno;
}

/* 
 * This function is cited from Beej's Guide to Network Programming.
 * Set up the UDP socket for either Server M or one of the three backend servers.
 * @param serverName a single letter representing the server that is setting up this socket. (e.g., 'M', 'A', 'P', 'Q')
 * @param portNum the macro static port number of the server
 * @return the UDP socket descriptor for the given server
 */
int setupUDP(char serverName, const char* portNum) {
	int sockfd;
	struct addrinfo hints, *servinfo, *p;
	int yes = 1;
	int rv;
	
	// Set up local address
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	hints.ai_flags = AI_PASSIVE;
	if ((rv = getaddrinfo(LOCALHOST, portNum, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	for(p = servinfo; p != NULL; p = p->ai_next) {
		// Get the socket descriptor
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			fprintf(stderr, "Server %c: UDP socket\n", serverName);
			continue;
		}
		
		// Get rid of "address already in use" problem
		if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
			perror("setsockopt");
			exit(1);
		}
		
		// Bind socket to the local address
		if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			fprintf(stderr, "Server %c: UDP bind\n", serverName);
			continue;
		}
		break;
	}
	freeaddrinfo(servinfo);
	if (p == NULL) {
		fprintf(stderr, "Server %c: UDP failed to bind\n", serverName);
		exit(1);
	}
	
	return sockfd;
}

