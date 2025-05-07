/* This file implements the quote server (Server Q) to manage and report stock price data. 
 * In the bootup phase, a list of stocks with their corresponding prices at different times is uploaded
 * to this server. Server Q is responsible for keeping a record of the current price for each stock. 
 * For every successful and unsuccessful "buy" and "sell" commands issued by the user, Server Q needs to 
 * update the current prices of stocks involved in those commands. 
 */

#include "utility.h"
using namespace std;

// Global Variables
map<string, vector<string>> quotes; // structure storing stock quotes
unordered_map<string, int> timestamp; // structure storing current time stamps of all stocks

/* 
 * Load stock prices from the input "quotes.txt" to the global variable quotes. 
 * Set the time stamp of all stocks to 0, which represents the index of the current price 
 * among the price list of each stock.
 */
void loadQuotes() {
	string line, ticker, price;
	
	ifstream file("quotes.txt");
	if (file.is_open()) {
		while (getline(file, line)) {
			istringstream iss(line);
			// Get the ticker of each stock
			iss >> ticker;
			// Get ten prices
			vector<string> priceList;
			while (iss >> price) {
				priceList.push_back(price);
			}
			
			if (priceList.size() == 10) {
				quotes[ticker] = priceList;
				timestamp[ticker] = 0;
			}
			else {
				perror("Wrong number of prices");
				exit(1);
			}
		}
	}
	else {
		perror("Failed to open quotes.txt");
		exit(1);
	}
	file.close();
}

/*
 * Get the quote of a specific stock. Return the availability of the queried stock in the database.
 * @param ticker the stock for which the user intends to get a quote
 * @param price the returned current price of this quoted stock
 * @return 1 if the queried stock exists in the database; otherwise, return 0
 */
int getQuote(const string& ticker, string& price) {
	auto it = quotes.find(ticker);
	if (it != quotes.end()) {
		auto itTime = timestamp.find(ticker);
		price = it->second[itTime->second];
		return 1;
	}
	else {
		return 0;
	}
}

int main() {
	int sockfd;
	int numbytes;
	char buf[MAXBUFSIZE];
	string response;
	struct sockaddr_in serverAddr; // socket address of Server M
	socklen_t addrLen = sizeof(serverAddr);
	
	// Bootup
	// Load input file
	loadQuotes();
	// Set up UDP socket
	printf("[Server Q] Booting up using UDP on port %s.\n", PORT_Q);
	sockfd = setupUDP('Q', PORT_Q);
	
	while (1) {
        // Receive a quote request from Server M
		if ((numbytes = recvfrom(sockfd, buf, MAXBUFSIZE - 1, 0, (struct sockaddr*)&serverAddr, &addrLen)) == -1) {
			perror("Server Q: recvfrom");
			continue;
		}
		buf[numbytes] = '\0';
		
     	// Parse the request
		string quoteRequest(buf), transID, payload;
		int colon = quoteRequest.find(':');
		transID = quoteRequest.substr(0, colon);
		payload = quoteRequest.substr(colon + 1);

     	// Send a response to Server M based on the request
		response = "";
        if (payload == "All_Stock") { // for a general quote request
			printf("[Server Q] Received a quote request from the main server.\n");
			response += transID + ":";
			for (const auto& pair : quotes) {
				response += pair.first + " " + pair.second[timestamp[pair.first]] + "\n";
			}
			if (sendto(sockfd, response.c_str(), response.length(), 0, (struct sockaddr*)&serverAddr, addrLen) == -1) {
				perror("Server Q: sendto");
				continue;
			}
			printf("[Server Q] Returned all stock quotes.\n");
        }
        else if (payload[0] == '+') { // for a time shift request
			string ticker = payload.substr(1);
			string price;
			auto it = quotes.find(ticker);
			if (it != quotes.end()) {
				int time = timestamp[ticker];
				timestamp[ticker] = (time + 1) % 10;
				price = it->second[timestamp[ticker]];
				printf("[Server Q] Received a time forward request for %s,"
						" the current price of that stock is %s at time %d.\n", ticker.c_str(), price.c_str(), timestamp[ticker]);
			}
			else {
				perror("Stock name does not exist.");
				continue;
			}
        }
		else if (payload[0] == 'p') { // for a position request
			// Read in a list of stock tickers and send a list of the current prices of those stocks back to Server M
			istringstream tickerList(payload.substr(1));
			string ticker, price;
			string priceList = transID + ":";
			while (tickerList >> ticker) {
				priceList += quotes[ticker][timestamp[ticker]] + " ";
			}
			if (sendto(sockfd, priceList.c_str(), priceList.length(), 0, (struct sockaddr*)&serverAddr, addrLen) == -1) {
				perror("Server Q: price list sendto");
				continue;
			}
		}
        else { // for a specific quote request
			string ticker = payload;
			string price;
			printf("[Server Q] Received a quote request from the main server for stock %s.\n", ticker.c_str());
			response += transID + ":";
			if (getQuote(ticker, price)) {
				response += ticker + " " + price;
			}
			else {
				response += "NOT_EXIST";
			}
			if (sendto(sockfd, response.c_str(), response.length(), 0, (struct sockaddr*)&serverAddr, addrLen) == -1) {
				perror("Server Q: sendto");
				continue;
			}
			printf("[Server Q] Returned the stock quote of %s.\n", ticker.c_str());
        }
	}

	close(sockfd);
	return 0;
}
