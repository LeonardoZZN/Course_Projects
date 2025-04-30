/* This file implements the main server (Server M) connecting to clients over TCP and to the three 
 * backend servers A, P, and Q over UDP. Server M functions as the central controller to handle users' commands
 * sent from clients and then assign different tasks to the three backend servers. This main server is responsible
 * for encrypting users' passwords in the authentication phase and calculating the profit/loss for "position" commands
 * based on the users' portfolios.
 */

#include "utility.h"
using namespace std;

/*
 * This function is cited from Beej's Guide to Network Programming.
 * Set up the TCP server side for Server M.
 * @return the TCP parent socket descriptor
 */
int setupTCP() {
	int sockfd;
	int yes = 1;
	int rv;
	struct addrinfo hints, *servinfo, *p;
	
	// Get the socket descriptor
	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
		perror("Server M: TCP socket");
		exit(1);
	}
	
	// Set up local address
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if ((rv = getaddrinfo(LOCALHOST, PORT_M_TCP, &hints, &servinfo)) != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}

	for(p = servinfo; p != NULL; p = p->ai_next) {
		// Get the socket descriptor
		if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
			perror("Server M: TCP socket\n");
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
			perror("Server M: TCP bind\n");
			continue;
		}
		break;
	}
	freeaddrinfo(servinfo);
	if (p == NULL) {
		perror("Server M: TCP failed to bind\n");
		exit(1);
	}
	
	// Listen to connections from clients
	if (listen(sockfd, BACKLOG) == -1) {
		close(sockfd);
		perror("Server M: listen");
		exit(1);
	}
	
	return sockfd;
}

/* 
 * Encrypts users' passwords which will be sent to Server A for authentication.
 * @param input the original password
 * @return the encrypted password
 */
string encryptPass(const string& input) {
	string res;
	for (char c : input) {
		if (c >= 'A' && c <= 'Z') {
			res += 'A' + (c + 3 - 'A') % 26;
		}
		else if (c >= 'a' && c <= 'z') {
			res += 'a' + (c + 3 - 'a') % 26;
		}
		else if (c >= '0' && c <= '9') {
			res += '0' + (c + 3 - '0') % 10;
		}
		else {
			res += c;
		}
	}
	return res;
}

/*
 * Handle authentication requests from clients.
 * @param sockTCP the TCP child socket connecting to the client
 * @param sockUDP the UDP socket used to communicate with the three backend servers
 * @param clientPort the dynamically assigned client's TCP port number, used as the transaction ID for UDP communication
 * @return the authenticated username for each client
 */
string handleAuth(const int& sockTCP, const int& sockUDP, const string& clientPort) {
	char buf[MAXBUFSIZE];
	string uname, password;
	int numbytes;
	// Set the socket address of Server A
	struct sockaddr_in sockaddrA;
	socklen_t addrLenA = sizeof(sockaddrA);
	sockaddrA.sin_family = AF_INET;
	sockaddrA.sin_port = htons(atoi(PORT_A));
	sockaddrA.sin_addr.s_addr = inet_addr(LOCALHOST);
	memset(sockaddrA.sin_zero, '\0', sizeof sockaddrA.sin_zero);
	while (1) {
		// Get username and password
		numbytes = recv(sockTCP, buf, MAXBUFSIZE - 1, 0);
		buf[numbytes] = '\0';
		if (numbytes == -1) {
			close(sockTCP);
			close(sockUDP);
			perror("Server M: recv");
			exit(1);
		}
		string recvRequest(buf);
		int comma = recvRequest.find(',');
		uname = recvRequest.substr(0, comma);
		password = recvRequest.substr(comma + 1);
		printf("[Server M] Received username %s and password ****.\n", uname.c_str());
		// Encrypt password
		string encrypted;
		encrypted = encryptPass(password);
		// Compose authentication message
		string authRequest = clientPort + ":" + uname + "," + encrypted, authResult;
		while (1) {
			// Send the authentication request to Server A via UDP
			if (sendto(sockUDP, authRequest.c_str(), authRequest.length(), 0, (struct sockaddr*)&sockaddrA, addrLenA) == -1) {
				close(sockTCP);
				close(sockUDP);
				perror("Server M: authentication request");
				exit(1);
			}
			printf("[Server M] Sent the authentication request to Server A.\n");
			// Receive authentication result from Server A
			numbytes = recvfrom(sockUDP, buf, MAXBUFSIZE - 1, 0, (struct sockaddr*)&sockaddrA, &addrLenA);
			if (numbytes == -1) {
				close(sockTCP);
				close(sockUDP);
				perror("Server M: authentication result");
				exit(1);
			}
			buf[numbytes] = '\0';
			authResult = buf; 
			int colon = authResult.find(':');
			if (authResult.substr(0, colon) == clientPort) {
				authResult = authResult.substr(colon+1);
				break;
			}
		}
		printf("[Server M] Received the response from server A using UDP over %s.\n", PORT_M_UDP);
		// Forward authentication result to client
		send(sockTCP, authResult.c_str(), authResult.length(), 0);
		printf("[Server M] Sent the response from server A to the client using TCP over port %s.\n", PORT_M_TCP);
		if (authResult[0] == 's') {break;}
	}
	return uname;
}

/*
 * Handle quote requests from clients
 * @param sockTCP the TCP child socket connecting to the client
 * @param sockUDP the UDP socket used to communicate with the three backend servers
 * @param clientPort the dynamically assigned client's TCP port number, used as the transaction ID for UDP communication
 * @param uname the username on the client side
 * @param buf the buffer used to store incoming messages
 */
void handleQuote(const int& sockTCP, const int& sockUDP, const string& clientPort, const string& uname, char* buf) {
	string request = "";
	string quoteResult;
	int numbytes;
	// Set the socket address of Server Q
	struct sockaddr_in sockaddrQ;
	socklen_t addrLenQ = sizeof(sockaddrQ);
	sockaddrQ.sin_family = AF_INET;
	sockaddrQ.sin_port = htons(atoi(PORT_Q));
	sockaddrQ.sin_addr.s_addr = inet_addr(LOCALHOST);
	memset(sockaddrQ.sin_zero, '\0', sizeof sockaddrQ.sin_zero);
	// For a general quote
	if (strcmp(buf, "qALL_STOCK") == 0) {
		printf("[Server M] Received a quote request from %s, using TCP over port %s.\n", uname.c_str(), PORT_M_TCP);
		// Compose a general quote request
		request += clientPort + ":" + "All_Stock";
		while (1) {
			// Forward the quote request to Server Q via UDP
			if (sendto(sockUDP, request.c_str(), request.length(), 0, (struct sockaddr*)&sockaddrQ, addrLenQ) == -1) {
				close(sockTCP);
				close(sockUDP);
				perror("Server M: quote request");
				exit(1);
			}
			printf("[Server M] Forwarded the quote request to server Q.\n");
			// Get a quote response from Server Q
			if ((numbytes = recvfrom(sockUDP, buf, MAXBUFSIZE - 1, 0, (struct sockaddr*)&sockaddrQ, &addrLenQ)) == -1) {
				close(sockTCP);
				close(sockUDP);
				perror("Server M: quote response");
				exit(1);
			}
			buf[numbytes] = '\0';
			quoteResult = buf; 
			int colon = quoteResult.find(':');
			if (quoteResult.substr(0, colon) == clientPort) {
				quoteResult = quoteResult.substr(colon+1);
				break;
			}
		}
		printf("[Server M] Received the quote response from server Q using UDP over %s.\n", PORT_M_UDP);
	}
	// For a specific stock quote
	else {
		string ticker(buf+1);
		printf("[Server M] Received a quote request from %s for stock %s, using TCP over port %s.\n", uname.c_str(), ticker.c_str(), PORT_M_TCP);
		// Compose a specific quote request
		request += clientPort + ":" + ticker;
		while (1) {
			// Forward the quote request to Server Q via UDP
			if (sendto(sockUDP, request.c_str(), request.length(), 0, (struct sockaddr*)&sockaddrQ, addrLenQ) == -1) {
				close(sockTCP);
				close(sockUDP);
				perror("Server M: quote request");
				exit(1);
			}
			printf("[Server M] Forwarded the quote request to server Q.\n");
			// Get a quote response from Server Q
			if ((numbytes = recvfrom(sockUDP, buf, MAXBUFSIZE - 1, 0, (struct sockaddr*)&sockaddrQ, &addrLenQ)) == -1) {
				close(sockTCP);
				close(sockUDP);
				perror("Server M: quote response");
				exit(1);
			}
			buf[numbytes] = '\0';
			quoteResult = buf; 
			int colon = quoteResult.find(':');
			if (quoteResult.substr(0, colon) == clientPort) {
				quoteResult = quoteResult.substr(colon+1);
				break;
			}
		}
		printf("[Server M] Received the quote response from server Q for stock %s using UDP over %s.\n", ticker.c_str(), PORT_M_UDP);
	}
	// Forward the quote response to client
	send(sockTCP, quoteResult.c_str(), quoteResult.length(), 0);
	printf("[Server M] Forwarded the quote response to the client.\n");
}

/*
 * Handle buy requests from clients
 * @param sockTCP the TCP child socket connecting to the client
 * @param sockUDP the UDP socket used to communicate with the three backend servers
 * @param clientPort the dynamically assigned client's TCP port number, used as the transaction ID for UDP communication
 * @param uname the username on the client side
 * @param buf the buffer used to store incoming messages
 */
void handleBuy(const int& sockTCP, const int& sockUDP, const string& clientPort, const string& uname, char* buf) {
	printf("[Server M] Received a buy request from member %s using TCP over port %s.\n", uname.c_str(), PORT_M_TCP);
	int numbytes;
	string payload(buf+1);
	int comma = payload.find(',');
	string ticker = payload.substr(0, comma);
	string shares = payload.substr(comma+1);

	// Set socket addresses of Server Q and Server P
	struct sockaddr_in sockaddrQ, sockaddrP;
	socklen_t addrLenQ = sizeof(sockaddrQ), addrLenP = sizeof(sockaddrP);
	sockaddrQ.sin_family = AF_INET;
	sockaddrQ.sin_port = htons(atoi(PORT_Q));
	sockaddrQ.sin_addr.s_addr = inet_addr(LOCALHOST);
	memset(sockaddrQ.sin_zero, '\0', sizeof sockaddrQ.sin_zero);
	sockaddrP.sin_family = AF_INET;
	sockaddrP.sin_port = htons(atoi(PORT_P));
	sockaddrP.sin_addr.s_addr = inet_addr(LOCALHOST);
	memset(sockaddrP.sin_zero, '\0', sizeof sockaddrP.sin_zero);

	string quoteRequest = clientPort + ":" + ticker, quoteResult;
	while (1) {
		// Send a quote request to Server Q to get the specific stock's price
		if (sendto(sockUDP, quoteRequest.c_str(), quoteRequest.length(), 0, (struct sockaddr*)&sockaddrQ, addrLenQ) == -1) {
			close(sockTCP);
			close(sockUDP);
			perror("Server M: quote request");
			exit(1);
		}
		printf("[Server M] Sent quote request to server Q.\n");
		// Get a quote response from Server Q
		if ((numbytes = recvfrom(sockUDP, buf, MAXBUFSIZE - 1, 0, (struct sockaddr*)&sockaddrQ, &addrLenQ)) == -1) {
			close(sockTCP);
			close(sockUDP);
			perror("Server M: quote response");
			exit(1);
		}
		buf[numbytes] = '\0';
		quoteResult = buf; 
		int colon = quoteResult.find(':');
		if (quoteResult.substr(0, colon) == clientPort) {
			quoteResult = quoteResult.substr(colon+1);
			break;
		}
	}
	printf("[Server M] Received quote response from server Q.\n");
	if (quoteResult == "NOT_EXIST") {
		// Notify the client that this purchase failed since the stock name does not exist
		send(sockTCP, quoteResult.c_str(), quoteResult.length(), 0);
		printf("[Server M] Forwarded the buy result to the client.\n");
		return;
	}
	// Get the current price of the queried stock
	int space = quoteResult.find(' ');
	string price = quoteResult.substr(space+1);
	// Send a buy confirmation to the client
	send(sockTCP, price.c_str(), price.length(), 0);
	printf("[Server M] Sent the buy confirmation to the client.\n");
	// Receive the decision of the client
	if ((numbytes = recv(sockTCP, buf, MAXBUFSIZE - 1, 0)) == -1) {
		close(sockTCP);
		close(sockUDP);
		perror("Server M: recv decision");
		exit(1);
	}
	buf[numbytes] = '\0';
	if (buf[0] == 'Y') {
		printf("[Server M] Buy approved.\n");

		string buyRequest = clientPort + ":b" + uname + "|" + ticker + "," + shares + "_" + price, buyResult;
		while (1) {
			// Send the purchase detail to Server P, format: bUname|ticker,shares_price
			if (sendto(sockUDP, buyRequest.c_str(), buyRequest.length(), 0, (struct sockaddr*)&sockaddrP, addrLenP) == -1) {
				close(sockTCP);
				close(sockUDP);
				perror("Server M: buy request to P");
				exit(1);
			}
			printf("[Server M] Forwarded the buy confirmation response to Server P.\n");
			// Receive the buy result from Server P
			if ((numbytes = recvfrom(sockUDP, buf, MAXBUFSIZE - 1, 0, (struct sockaddr*)&sockaddrP, &addrLenP)) == -1) {
				close(sockTCP);
				close(sockUDP);
				perror("Server M: buy result from P");
				exit(1);
			}
			buf[numbytes] = '\0';
			buyResult = buf; 
			int colon = buyResult.find(':');
			if (buyResult.substr(0, colon) == clientPort) {
				buyResult = buyResult.substr(colon+1);
				break;
			}
		}
		// Forward the buy result to the client
		send(sockTCP, buyResult.c_str(), buyResult.length(), 0);
		printf("[Server M] Forwarded the buy result to the client.\n");
	}
	else if (buf[0] == 'N') {
		printf("[Server M] Buy denied.\n");
	}
	else {
		return;
	}
	// Send a time shift request to Server Q
	string shiftRequest = clientPort + ":+" + ticker;
	if (sendto(sockUDP, shiftRequest.c_str(), shiftRequest.length(), 0, (struct sockaddr*)&sockaddrQ, addrLenQ) == -1) {
		close(sockTCP);
		close(sockUDP);
		perror("Server M: shift request");
		exit(1);
	}
	printf("[Server M] Sent a time forward request for %s.\n", ticker.c_str());
}

/*
 * Handle sell requests from clients
 * @param sockTCP the TCP child socket connecting to the client
 * @param sockUDP the UDP socket used to communicate with the three backend servers
 * @param clientPort the dynamically assigned client's TCP port number, used as the transaction ID for UDP communication
 * @param uname the username on the client side
 * @param buf the buffer used to store incoming messages
 */
void handleSell(const int& sockTCP, const int& sockUDP, const string& clientPort, const string& uname, char* buf) {
	printf("[Server M] Received a sell request from member %s using TCP over port %s.\n", uname.c_str(), PORT_M_TCP);
	int numbytes;
	string payload(buf+1);
	int comma = payload.find(',');
	string ticker = payload.substr(0, comma);
	string shares = payload.substr(comma+1);

	// Set socket addresses of Server Q and Server P
	struct sockaddr_in sockaddrQ, sockaddrP;
	socklen_t addrLenQ = sizeof(sockaddrQ), addrLenP = sizeof(sockaddrP);
	sockaddrQ.sin_family = AF_INET;
	sockaddrQ.sin_port = htons(atoi(PORT_Q));
	sockaddrQ.sin_addr.s_addr = inet_addr(LOCALHOST);
	memset(sockaddrQ.sin_zero, '\0', sizeof sockaddrQ.sin_zero);
	sockaddrP.sin_family = AF_INET;
	sockaddrP.sin_port = htons(atoi(PORT_P));
	sockaddrP.sin_addr.s_addr = inet_addr(LOCALHOST);
	memset(sockaddrP.sin_zero, '\0', sizeof sockaddrP.sin_zero);

	string quoteRequest = clientPort + ":" + ticker, quoteResult;
	while (1) {
		// Send a quote request to Server Q to get the specific stock's price
		if (sendto(sockUDP, quoteRequest.c_str(), quoteRequest.length(), 0, (struct sockaddr*)&sockaddrQ, addrLenQ) == -1) {
			close(sockTCP);
			close(sockUDP);
			perror("Server M: quote request");
			exit(1);
		}
		printf("[Server M] Sent quote request to server Q.\n");
		// Get a quote response from Server Q
		if ((numbytes = recvfrom(sockUDP, buf, MAXBUFSIZE - 1, 0, (struct sockaddr*)&sockaddrQ, &addrLenQ)) == -1) {
			close(sockTCP);
			close(sockUDP);
			perror("Server M: quote response");
			exit(1);
		}
		buf[numbytes] = '\0';
		quoteResult = buf;
		int colon = quoteResult.find(':');
		if (quoteResult.substr(0, colon) == clientPort) {
			quoteResult = quoteResult.substr(colon+1);
			break;
		}
	}
	printf("[Server M] Received quote response from server Q.\n");
	if (quoteResult == "NOT_EXIST") {
		// Notify the client that this sell failed since the stock name does not exist
		send(sockTCP, quoteResult.c_str(), quoteResult.length(), 0);
		printf("[Server M] Forwarded the sell result to the client.\n");
		return;
	}
	// Get the current price of the queried stock
	int space = quoteResult.find(' ');
	string price = quoteResult.substr(space+1);

	string sellRequest = clientPort + ":s" + uname + "|" + ticker + "," + shares, shareStatus;
	while (1) {
		// Forward the sell request to Server P to check the number of shares
		if (sendto(sockUDP, sellRequest.c_str(), sellRequest.length(), 0, (struct sockaddr*)&sockaddrP, addrLenP) == -1) {
			close(sockTCP);
			close(sockUDP);
			perror("Server M: sell request");
			exit(1);
		}
		printf("[Server M] Forwarded the sell request to server P.\n");
		// Get a response from Server P about the status of shares
		if ((numbytes = recvfrom(sockUDP, buf, MAXBUFSIZE - 1, 0, (struct sockaddr*)&sockaddrP, &addrLenP)) == -1) {
			close(sockTCP);
			close(sockUDP);
			perror("Server M: sell response");
			exit(1);
		}
		buf[numbytes] = '\0';
		shareStatus = buf;
		int colon = shareStatus.find(':');
		if (shareStatus.substr(0, colon) == clientPort) {
			shareStatus = shareStatus.substr(colon+1);
			break;
		}
	}
	
	if (shareStatus == "SUFF") {
		// Forward a sell confirmation to the client
		send(sockTCP, price.c_str(), price.length(), 0);
		printf("[Server M] Forwarded the sell confirmation to the client.\n");
		// Receive the decision of the client
		if ((numbytes = recv(sockTCP, buf, MAXBUFSIZE - 1, 0)) == -1) {
			close(sockTCP);
			close(sockUDP);
			perror("Server M: recv decision");
			exit(1);
		}
		buf[numbytes] = '\0';
		// Forward the confirmation response from the client to Server P
		string confirmResult;
		if (buf[0] == 'Y') {
			confirmResult = clientPort + ":S_CONFIRMED";
			string sellResult;
			while (1) {
				if (sendto(sockUDP, confirmResult.c_str(), confirmResult.length(), 0, (struct sockaddr*)&sockaddrP, addrLenP) == -1) {
					close(sockTCP);
					close(sockUDP);
					perror("Server M: sell confirmation result");
					exit(1);
				}
				printf("[Server M] Forwarded the sell confirmation response to Server P.\n");

				// Get a sell final result from Server P and then forward the result to the client
				if ((numbytes = recvfrom(sockUDP, buf, MAXBUFSIZE - 1, 0, (struct sockaddr*)&sockaddrP, &addrLenP)) == -1) {
					close(sockTCP);
					close(sockUDP);
					perror("Server M: sell final result");
					exit(1);
				}
				buf[numbytes] = '\0';
				sellResult = buf;
				int colon = sellResult.find(':');
				if (sellResult.substr(0, colon) == clientPort) {
					sellResult = sellResult.substr(colon+1);
					break;
				}
			}
			// Forward the sell result to the client
			send(sockTCP, sellResult.c_str(), sellResult.length(), 0);
			printf("[Server M] Forwarded the sell result to the client.\n");
		}
		else if (buf[0] == 'N') {
			confirmResult = clientPort + ":S_DENIED";
			if (sendto(sockUDP, confirmResult.c_str(), confirmResult.length(), 0, (struct sockaddr*)&sockaddrP, addrLenP) == -1) {
				close(sockTCP);
				close(sockUDP);
				perror("Server M: sell confirmation result");
				exit(1);
			}
			printf("[Server M] Forwarded the sell confirmation response to Server P.\n");
		}
	}
	else if (shareStatus == "NOT_SUFF") {
		// Notify the client that this sell failed since there are no sufficient shares to be sold
		send(sockTCP, shareStatus.c_str(), shareStatus.length(), 0);
		printf("[Server M] Forwarded the sell result to the client.\n");
	}

	// Send a time shift request to Server Q
	string shiftRequest = clientPort + ":+" + ticker;
	if (sendto(sockUDP, shiftRequest.c_str(), shiftRequest.length(), 0, (struct sockaddr*)&sockaddrQ, addrLenQ) == -1) {
		close(sockTCP);
		close(sockUDP);
		perror("Server M: shift request");
		exit(1);
	}
	printf("[Server M] Sent a time forward request for %s.\n", ticker.c_str());
}

/*
 * Handle position requests from clients
 * @param sockTCP the TCP child socket connecting to the client
 * @param sockUDP the UDP socket used to communicate with the three backend servers
 * @param clientPort the dynamically assigned client's TCP port number, used as the transaction ID for UDP communication
 * @param uname the username on the client side
 * @param buf the buffer used to store incoming messages
 */
void handlePosition(const int& sockTCP, const int& sockUDP, const string& clientPort, const string& uname, char* buf) {
	int numbytes;
	string tickerList = clientPort + ":p";
	vector<int> sharesList;
	vector<double> avgBuyPriceList, curPriceList;

	// Set socket addresses of Server Q and Server P
	struct sockaddr_in sockaddrQ, sockaddrP;
	socklen_t addrLenQ = sizeof(sockaddrQ), addrLenP = sizeof(sockaddrP);
	sockaddrQ.sin_family = AF_INET;
	sockaddrQ.sin_port = htons(atoi(PORT_Q));
	sockaddrQ.sin_addr.s_addr = inet_addr(LOCALHOST);
	memset(sockaddrQ.sin_zero, '\0', sizeof sockaddrQ.sin_zero);
	sockaddrP.sin_family = AF_INET;
	sockaddrP.sin_port = htons(atoi(PORT_P));
	sockaddrP.sin_addr.s_addr = inet_addr(LOCALHOST);
	memset(sockaddrP.sin_zero, '\0', sizeof sockaddrP.sin_zero);

	printf("[Server M] Received a position request from Member to check %s’s gain using TCP over port %s.\n", uname.c_str(), PORT_M_TCP);
	string posRequest = clientPort + ":p" + uname, portfolio;
	while (1) {
		// Forward the position request to server P
		if (sendto(sockUDP, posRequest.c_str(), posRequest.length(), 0, (struct sockaddr*)&sockaddrP, addrLenP) == -1) {
			close(sockTCP);
			close(sockUDP);
			perror("Server M: position request");
			exit(1);
		}
		printf("[Server M] Forwarded the position request to server P.\n");

		// Receive the portfolio from Server P
		if ((numbytes = recvfrom(sockUDP, buf, MAXBUFSIZE - 1, 0, (struct sockaddr*)&sockaddrP, &addrLenP)) == -1) {
			close(sockTCP);
			close(sockUDP);
			perror("Server M: portfolio");
			exit(1);
		}
		buf[numbytes] = '\0';
		portfolio = buf;
		int colon = portfolio.find(':');
		if (portfolio.substr(0, colon) == clientPort) {
			portfolio = portfolio.substr(colon+1);
			break;
		}
	}
	printf("[Server M] Received user’s portfolio from server P using UDP over %s.\n", PORT_M_UDP);

	// Read the portfolio and store the stocks' info into lists
	istringstream iss(portfolio);
	string line;
	string ticker;
	int shares;
	double avgPrice;
	while (getline(iss, line)) {
		istringstream liness(line);
		if (liness >> ticker >> shares >> avgPrice) {
			tickerList += ticker + " ";
			sharesList.push_back(shares);
			avgBuyPriceList.push_back(avgPrice);
		}
	}

	string curPrices;
	while (1) {
		// Send a ticker list to Server Q to get current prices of those stocks listed in the portfolio
		if (sendto(sockUDP, tickerList.c_str(), tickerList.length(), 0, (struct sockaddr*)&sockaddrQ, addrLenQ) == -1) {
			close(sockTCP);
			close(sockUDP);
			perror("Server M: current price request");
			exit(1);
		}

		// Receive a list of current prices from Server Q
		if ((numbytes = recvfrom(sockUDP, buf, MAXBUFSIZE - 1, 0, (struct sockaddr*)&sockaddrQ, &addrLenQ)) == -1) {
			close(sockTCP);
			close(sockUDP);
			perror("Server M: current prices");
			exit(1);
		}
		buf[numbytes] = '\0';
		curPrices = buf;
		int colon = curPrices.find(':');
		if (curPrices.substr(0, colon) == clientPort) {
			curPrices = curPrices.substr(colon+1);
			break;
		}
	}

	// Store current prices into curPriceList
	istringstream priceSS(curPrices);
	double price;
	while (priceSS >> price) {
		curPriceList.push_back(price);
	}

	// Calculate the profit
	double profit = 0;
	for (size_t i = 0; i < sharesList.size(); i++) {
		profit += sharesList[i] * (curPriceList[i] - avgBuyPriceList[i]);
	}

	// Send the portfolio and the profit to the client
	string posResponse = to_string(profit) + "|" + portfolio;
	send(sockTCP, posResponse.c_str(), posResponse.length(), 0);
	printf("[Server M] Forwarded the gain to the client.\n");
}

int main() {
	int newSock;
	int numbytes;
	struct sockaddr_in clientAddr;
	socklen_t sin_size = sizeof(struct sockaddr_in);
	
	// Bootup
	int sockUDP = setupUDP('M', PORT_M_UDP);
	int sockTCP = setupTCP();
	printf("[Server M] Booting up using UDP on port %s.\n", PORT_M_UDP);
	
	// Reap all dead processes
	// This block of code is cited from Beej's Guide to Network Programming.
	struct sigaction sa;
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction");
		exit(1);
	}

	while (1) {
		if ((newSock = accept(sockTCP, (struct sockaddr*)&clientAddr, &sin_size)) == -1) {
			perror("Server M: accept");
			continue;
		}
		
		// For child processes
		pid_t pid = fork();
		if (pid == 0) {
			close(sockTCP);
			
			char buf[MAXBUFSIZE];
			string clientPort = to_string(ntohs(clientAddr.sin_port)); // Use the client port number as transaction ID

			/*------Authentication------*/
			string uname = handleAuth(newSock, sockUDP, clientPort);
			
			/*------Trading------*/
			while (1) {
				// Get new request
				if ((numbytes = recv(newSock, buf, MAXBUFSIZE - 1, 0)) == -1) {
					close(newSock);
					perror("Server M: recv");
					continue;
				}
				buf[numbytes] = '\0';
				
				/* Quote */
				if (buf[0] == 'q') {
					handleQuote(newSock, sockUDP, clientPort, uname, buf);
				} 
				/* Buy */
				else if (buf[0] == 'b') {
					handleBuy(newSock, sockUDP, clientPort, uname, buf);
				}
				/* Sell */
				else if (buf[0] == 's') {
					handleSell(newSock, sockUDP, clientPort, uname, buf);
				}
				/* Position */
				else if (buf[0] == 'p') {
					handlePosition(newSock, sockUDP, clientPort, uname, buf);
				}
			}
			close(newSock);
			close(sockUDP);
			exit(0);
		}
		else {
			close(newSock);
		}
	}
	close(sockTCP);
	close(sockUDP);
	return 0;
}
	
	


