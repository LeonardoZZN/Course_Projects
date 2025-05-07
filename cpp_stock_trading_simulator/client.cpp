/*	This file implements the client interface that users interact with, connecting to Server M via TCP connections. */

#include "utility.h"
using namespace std;

/*
 * Process the quote commands for a specific stock.
 * @param sockfd the TCP socket connecting to Server M
 * @param localPort the dynamically assigned local TCP port number
 * @param buf the buffer to store the incoming messages
 * @param stockname the stock for which the user is requesting a quote
 */
void quoteSpec(const int& sockfd, const int& localPort, char* buf, const string& stockname) {
	int numbytes;
	// Compose a specific quote request
	string quoteRequest = "q" + stockname;
	// Send the quote request to Server M
	if (send(sockfd, quoteRequest.c_str(), quoteRequest.length(), 0) == -1) {
		close(sockfd);
		perror("Client: send quote request");
		exit(1);
	}
	printf("[Client] Sent a quote request to the main server.\n");
	// Receive the quote response
	if ((numbytes = recv(sockfd, buf, MAXBUFSIZE - 1, 0)) == -1) {
		close(sockfd);
		perror("Client: recv quote response");
		exit(1);
	}
	buf[numbytes] = '\0';
	string response(buf);
	if (response == "NOT_EXIST") {
		printf("[Client] Received the response from the main server using TCP over port %d.\n"
			"%s does not exist. Please try again.\n"
			"—Start a new request—\n", localPort, stockname.c_str());
	}
	else {
		printf("[Client] Received the response from the main server using TCP over port %d.\n"
			"%s\n"
			"—Start a new request—\n", localPort, response.c_str());
	}
}

/*
 * Process the general quote commands for all stocks.
 * @param sockfd the TCP socket connecting to Server M
 * @param localPort the dynamically assigned local TCP port number
 * @param buf the buffer to store the incoming messages
 */
void quoteGen(const int& sockfd, const int& localPort, char* buf) {
	int numbytes;
	// Compose a general quote request
	string quoteRequest = "qALL_STOCK";
	// Send the quote request to Server M
	if (send(sockfd, quoteRequest.c_str(), quoteRequest.length(), 0) == -1) {
		close(sockfd);
		perror("Client: send quote request");
		exit(1);
	}
	printf("[Client] Sent a quote request to the main server.\n");
	// Receive the quote response
	if ((numbytes = recv(sockfd, buf, MAXBUFSIZE - 1, 0)) == -1) {
		close(sockfd);
		perror("Client: recv quote response");
		exit(1);
	}
	buf[numbytes] = '\0';
	string response(buf);
	printf("[Client] Received the response from the main server using TCP over port %d.\n"
			"%s"
			"—Start a new request—\n", localPort, response.c_str());
}

/*
 * Process the buy commands for purchasing a specific number of shares of a specific stock.
 * @param sockfd the TCP socket connecting to Server M
 * @param localPort the dynamically assigned local TCP port number
 * @param buf the buffer to store the incoming messages
 * @param uname the username
 * @param stockname the stock for which the user intends to buy some shares
 * @param numShares the number of shares involved in the purchase
 */
void buy(const int& sockfd, const int& localPort, char* buf, const string& uname, const string& stockname, const string& numShares) {
	int numbytes;
	// Compose a buy request
	string buyRequest = "b" + stockname + "," + numShares;
	// Send the buy request to Server M
	if (send(sockfd, buyRequest.c_str(), buyRequest.length(), 0) == -1) {
		close(sockfd);
		perror("Client: send buy request");
		exit(1);
	}
	// Receive a response from Server M
	if ((numbytes = recv(sockfd, buf, MAXBUFSIZE - 1, 0)) == -1) {
		close(sockfd);
		perror("Client: recv buy response");
		exit(1);
	}
	buf[numbytes] = '\0';
	if (strcmp(buf, "NOT_EXIST") == 0) { // unavailable stock name
		printf("[Client] Error: stock name does not exist. Please check again.\n"
			"—Start a new request—\n");
		return;
	}
	else {
		string input;
		while (1) {
			// Ask for buy confirmation
			printf("[Client] %s’s current price is %s. Proceed to buy? (Y/N)\n", stockname.c_str(), buf);
			getline(cin, input);
			if (input == "Y" || input == "y" || input == "N" || input == "n") {
				break;
			}
		}
	
		// Send decision to Server M
		string decision;
		if (input == "Y" || input == "y") {
			decision = "Y";
			if (send(sockfd, decision.c_str(), decision.length(), 0) == -1) {
				close(sockfd);
				perror("Client: send decision");
				exit(1);
			}
			// Receive a purchase result from Server M
			if ((numbytes = recv(sockfd, buf, MAXBUFSIZE - 1, 0)) == -1) {
				close(sockfd);
				perror("Client: recv buy response");
				exit(1);
			}
			buf[numbytes] = '\0';
			if (buf[0] == 's') {
				printf("[Client] Received the response from the main server using TCP over port %d.\n"
					"%s successfully bought %s shares of %s.\n"
					"—Start a new request—\n", localPort, uname.c_str(), numShares.c_str(), stockname.c_str());
			}
		}
		else {
			decision = "N";
			if (send(sockfd, decision.c_str(), decision.length(), 0) == -1) {
				close(sockfd);
				perror("Client: send decision");
				exit(1);
			}
			printf("—Start a new request—\n");
		}
	}
}

/*
 * Process the sell commands for selling a specific number of shares of a specific stock from the user's portfolio.
 * @param sockfd the TCP socket connecting to Server M
 * @param localPort the dynamically assigned local TCP port number
 * @param buf the buffer to store the incoming messages
 * @param uname the username
 * @param stockname the stock for which the user intends to sell some shares
 * @param numShares the number of shares involved in the sale
 */
void sell(const int& sockfd, const int& localPort, char* buf, const string& uname, const string& stockname, const string& numShares) {
	int numbytes;
	// Compose a sell request
	string sellRequest = "s" + stockname + "," + numShares;
	// Send the sell request to Server M
	if (send(sockfd, sellRequest.c_str(), sellRequest.length(), 0) == -1) {
		close(sockfd);
		perror("Client: send sell request");
		exit(1);
	}
	// Receive a response from Server M
	if ((numbytes = recv(sockfd, buf, MAXBUFSIZE - 1, 0)) == -1) {
		close(sockfd);
		perror("Client: recv sell response");
		exit(1);
	}
	buf[numbytes] = '\0';
	if (strcmp(buf, "NOT_EXIST") == 0) { // unavailable stock name
		printf("[Client] Error: stock name does not exist. Please check again.\n"
			"—Start a new request—\n");
		return;
	}
	else if (strcmp(buf, "NOT_SUFF") == 0) { // nut sufficient held shares
		printf("[Client] Error: %s does not have enough shares of %s to sell. Please try again.\n"
			"—Start a new request—\n", uname.c_str(), stockname.c_str());
		return;
	}
	else {
		string input;
		while (1) {
			// Ask for sell confirmation
			printf("[Client] %s’s current price is %s. Proceed to sell? (Y/N)\n", stockname.c_str(), buf);
			getline(cin, input);
			if (input == "Y" || input == "y" || input == "N" || input == "n") {
				break;
			}
		}
	
		// Send decision to Server M
		string decision;
		if (input == "Y" || input == "y") {
			decision = "Y";
			if (send(sockfd, decision.c_str(), decision.length(), 0) == -1) {
				close(sockfd);
				perror("Client: send decision");
				exit(1);
			}
			// Receive a sell result from Server M
			if ((numbytes = recv(sockfd, buf, MAXBUFSIZE - 1, 0)) == -1) {
				close(sockfd);
				perror("Client: recv sell response");
				exit(1);
			}
			buf[numbytes] = '\0';
			if (buf[0] == 's') {
				printf("[Client] %s successfully sold %s shares of %s.\n"
					"—Start a new request—\n", uname.c_str(), numShares.c_str(), stockname.c_str());
			}
		}
		else {
			decision = "N";
			if (send(sockfd, decision.c_str(), decision.length(), 0) == -1) {
				close(sockfd);
				perror("Client: send decision");
				exit(1);
			}
			printf("—Start a new request—\n");
		}
	}
}

/*
 * Process the position commands for checking users' portfolios and the net profit/loss based on the current prices.
 * @param sockfd the TCP socket connecting to Server M
 * @param localPort the dynamically assigned local TCP port number
 * @param buf the buffer to store the incoming messages
 * @param uname the username
 */
void position(const int& sockfd, const int& localPort, char* buf, const string& uname) {
	int numbytes;
	// Compose a position request
	string posRequest = "p";
	// Send the position request to Server M
	if (send(sockfd, posRequest.c_str(), posRequest.length(), 0) == -1) {
		close(sockfd);
		perror("Client: send position request");
		exit(1);
	}
	printf("[Client] %s sent a position request to the main server.\n", uname.c_str());
	// Receive a response from Server M
	if ((numbytes = recv(sockfd, buf, MAXBUFSIZE - 1, 0)) == -1) {
		close(sockfd);
		perror("Client: recv position response");
		exit(1);
	}
	buf[numbytes] = '\0';
	// Parse the position result
	string payload(buf);
	int pipe = payload.find('|');
	double profit = stod(payload.substr(0, pipe));
	string portfolio = payload.substr(pipe + 1);
	// Print out the portfolio with the profit
	printf("[Client] Received the response from the main server using TCP over port %d.\n"
		"stock shares avg_buy_price\n"
		"%s"
		"%s’s current profit is %.2f.\n", localPort, portfolio.c_str(), uname.c_str(), profit);
}


int main() {
	int sockfd, numbytes;
	struct addrinfo hints, *servinfo, *p;
	int rv;
	struct sockaddr_in clientAddr;
	socklen_t clientAddrLen = sizeof(clientAddr);
	string uname, password;
	char buf[MAXBUFSIZE];
	int localPort;
	
	// Bootup
	printf("[Client] Booting up.\n");
	
	/* The following code snippet is cited from Beej's Guide to Network Programming. */
	// Get the server address
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	if ((rv = getaddrinfo(LOCALHOST, PORT_M_TCP, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	for(p = servinfo; p != NULL; p = p->ai_next) {
		// Get the socket descriptor
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("Client: socket\n");
			continue;
		}
		
		// Connect to Server M
		if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
			close(sockfd);
			perror("Client: TCP connect");
			exit(1);
		} 
		break;
	}
	freeaddrinfo(servinfo);
	if (p == NULL) {
		perror("Client: failed to connect\n");
		exit(1);
	}
	
	// Get this client's dynamically assigned port number
	if (getsockname(sockfd, (struct sockaddr*)&clientAddr, &clientAddrLen) == -1) {
		perror("getsockname");
		exit(1);
	}
	localPort = ntohs(clientAddr.sin_port);
	
	/* Authentication */
	printf("[Client] Logging in.\n");
	while (1) {
		// Get the username from stdin
		cout << "Please enter the username: ";
		getline(cin, uname);
		// Get the password from stdin
		cout << "Please enter the password: ";
		getline(cin, password);
		// Compose the message encapsulating credentials
		string authRequest = uname + "," + password;
		// Send an authentication request to Server M via TCP
		if (send(sockfd, authRequest.c_str(), authRequest.length(), 0) == -1) {
			close(sockfd);
			perror("Client: send auth request");
			exit(1);
		}
		// Receive the authentication result
		if ((numbytes = recv(sockfd, buf, MAXBUFSIZE - 1, 0)) == -1) {
			close(sockfd);
			perror("Client: recv auth response");
			exit(1);
		}
		buf[numbytes] = '\0';
		if (buf[0] == 's') {
			printf("[Client] You have been granted access.\n");
			break;
		}
		else {
			printf("[Client] The credentials are incorrect. Please try again.\n");
			continue;
		}
	}
	
	/* Operations */
	string input, command, stockname, numShares;
	while (1) {
		printf("[Client] Please enter the command:\n"
			"<quote>\n"
			"<quote <stock name>>\n"
			"<buy <stock name> <number of shares>>\n"
			"<sell <stock name> <number of shares>>\n"
			"<position>\n"
			"<exit>\n");
		getline(cin, input);
		istringstream iss(input);
		iss >> command;
		
		if (command == "exit") {
			break;
		}
		/* Quote */
		else if (command == "quote") { 
			/* Specific Quote */
			if (iss >> stockname) { 
				quoteSpec(sockfd, localPort, buf, stockname);
			}
			/* General Quote */
			else { 
				quoteGen(sockfd, localPort, buf);
			}
		}
		/* Buy */
		else if (command == "buy") { 
			if (iss >> stockname && iss >> numShares) {
				buy(sockfd, localPort, buf, uname, stockname, numShares);
			}
			else {
				printf("[Client] Error: stock name/shares are required. Please specify a stock name to buy.\n"
					"—Start a new request—\n");
			}
		}
		/* Sell */
		else if (command == "sell") {
			if (iss >> stockname && iss >> numShares) {
				sell(sockfd, localPort, buf, uname, stockname, numShares);
			}
			else {
				printf("[Client] Error: stock name/shares are required. Please specify a stock name to sell.\n"
					"—Start a new request—\n");
			}
		}
		/* Position */
		else if (command == "position") {
			position(sockfd, localPort, buf, uname);
		}
	}
			
	close(sockfd);
	return 0;
}
