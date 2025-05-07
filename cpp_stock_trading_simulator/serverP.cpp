/* This file implements the portfolio server (Server P) to manage user portfolios, which for each user
 * maintains a list of stock information containing the ticker name, the number of shares held, and the average buy price
 * of each stock in the user's portfolio. For approved "buy" and "sell" commands, Server P is responsible for updating
 * the users' portfolios. 
 */

#include "utility.h"
using namespace std;

// Structure to contain the details of a stock owned by a member
struct OneStockInfo {
	string ticker;
	int shares;
	double avgPrice;
};

// Global Variable
unordered_map<string, vector<OneStockInfo>> pf;

/*
 * Load member portfolios from the input "portfolios.txt" to the global variable. 
 */
void loadPortfolios() {
	string line, member, ticker, temp;
	int shares;
	double avgP;

	ifstream file("portfolios.txt");
	if (file.is_open()) {
		while (getline(file, line)) {
			istringstream iss(line);
			// Get the first word of each line
			iss >> temp;
			if (!(iss >> shares)) {
				member = temp;
				pf[member] = vector<OneStockInfo>();
			}
			else {
				ticker = temp;
				iss >> avgP;
				OneStockInfo newStock = {ticker, shares, avgP};
				pf[member].push_back(newStock);
			}
		}
	}
	else {
		perror("Failed to open portfolios.txt");
		exit(1);
	}
	file.close();
}

/*
 * Update the portfolio for an approved buy request.
 * @param uname the username
 * @param bTicker the stock for which the user intends to buy some shares
 * @param bShares the number of shares involved in the purchase
 * @param bPrice the current price of this specific stock
 */
void buyStock(const string& uname, const string& bTicker, const int& bShares, const double& bPrice) {
	for (OneStockInfo& stock : pf[uname]) {
		if (stock.ticker == bTicker) {
			// Calculate the previous total cost for this stock
			int prevCost = stock.shares * stock.avgPrice;
			// Update the number of shares
			stock.shares += bShares;
			// Update the average buy price
			stock.avgPrice = (prevCost + bShares * bPrice) / stock.shares;
			return;
		}
	}

	// bTicker is not in the portfolio
	pf[uname].push_back({bTicker, bShares, bPrice});
}

/*
 * Check the number of shares currently held by the user for a stock of interest to be sold.
 * @param uname the username
 * @param sTicker the stock for which the user intends to sell some shares
 * @param sShares the number of shares involved in the sale
 * @return true, if there are sufficient shares. false, if the number of shares is not sufficient.
 */
bool checkShareNum(const string& uname, const string& sTicker, const int& sShares) {
	for (OneStockInfo& stock : pf[uname]) {
		if (stock.ticker == sTicker) {
			if (stock.shares >= sShares) {
				return true;
			}
			else {
				return false;
			}
		}
	}
	return false;
}

/*
 * Update the portfolio for an approved sell request.
 * @param uname the username
 * @param sTicker the stock for which the user intends to sell some shares
 * @param sShares the number of shares involved in the sale
 */
void sellStock(const string& uname, const string& sTicker, const int& sShares) {
	vector<OneStockInfo>& stocks = pf[uname];
	for (auto it = stocks.begin(); it != stocks.end(); it++) {
		if (it->ticker == sTicker) {
			// Update the share number
			it->shares -= sShares;
			if (it->shares == 0) {
				stocks.erase(it);
			}
			break;
		}
	}
}

int main() {
	int sockfd;
	int numbytes;
	char buf[MAXBUFSIZE];

	struct sockaddr_in serverAddr; // socket address of Server M
	socklen_t addrLen = sizeof(serverAddr);

	// Bootup
	// Load input file
	loadPortfolios();
	// Set up UDP socket
	printf("[Server P] Booting up using UDP on port %s.\n", PORT_P);
	sockfd = setupUDP('P', PORT_P);
	
	while (1) {
		// Receive a new request from Server M
		if ((numbytes = recvfrom(sockfd, buf, MAXBUFSIZE - 1, 0, (struct sockaddr*)&serverAddr, &addrLen)) == -1) {
			perror("Server P: recvfrom");
			continue;
		}
		buf[numbytes] = '\0';

     	// Parse the request
		string request(buf), transID, payload;
		int colon = request.find(':');
		transID = request.substr(0, colon);
		payload = request.substr(colon + 1);

     	// Process the payload
		if (payload[0] == 'b') { // a buy request, format: bUname|ticker,shares_price
			printf("[Server P] Received a buy request from the client.\n");
			string uname, bTicker;
			int bShares;
			double bPrice;

			payload = payload.substr(1);
			int pipe = payload.find('|');
			uname = payload.substr(0, pipe);
			payload = payload.substr(pipe + 1);
			int comma = payload.find(',');
			bTicker = payload.substr(0, comma);
			payload = payload.substr(comma+1);
			int underscore = payload.find('_');
			bShares = stoi(payload.substr(0, underscore));
			bPrice = stod(payload.substr(underscore + 1));

			// Update pf
			buyStock(uname, bTicker, bShares, bPrice);

			// Send a purchase confirmation to Server M
			string response = transID + ":s";
			if (sendto(sockfd, response.c_str(), response.length(), 0, (struct sockaddr*)&serverAddr, addrLen) == -1) {
				perror("Server P: buy sendto");
				continue;
			}
			printf("[Server P] Successfully bought %d shares of %s and updated %s’s portfolio.\n", 
				bShares, bTicker.c_str(), uname.c_str());
		}
		else if (payload[0] == 's') { // a sell check request, format: sUname|ticker,shares
			printf("[Server P] Received a sell request from the main server.\n");
			string uname, sTicker;
			int sShares;

			payload = payload.substr(1);
			int pipe = payload.find('|');
			uname = payload.substr(0, pipe);
			payload = payload.substr(pipe + 1);
			int comma = payload.find(',');
			sTicker = payload.substr(0, comma);
			sShares = stoi(payload.substr(comma + 1));
			
			string stockStatus = transID + ":";
			// Check if there are sufficient shares
			if (checkShareNum(uname, sTicker, sShares)) {
				// Sufficient shares: requesting users' confirmation via Server M
				stockStatus += "SUFF";
				if (sendto(sockfd, stockStatus.c_str(), stockStatus.length(), 0, (struct sockaddr*)&serverAddr, addrLen) == -1) {
					perror("Server P: sell sendto");
					continue;
				}
				printf("[Server P] Stock %s has sufficient shares in %s’s portfolio. Requesting users’ confirmation for selling stock.\n", 
					sTicker.c_str(), uname.c_str());
				
				string response;
				while (1) {
					// Receive users' confirmation responses
					if ((numbytes = recvfrom(sockfd, buf, MAXBUFSIZE - 1, 0, (struct sockaddr*)&serverAddr, &addrLen)) == -1) {
						perror("Server P: sell recvfrom");
						continue;
					}
					buf[numbytes] = '\0';
					// Parse the response
					response = buf;
					int colon = response.find(':');
					if (response.substr(0, colon) == transID) {
						response = response.substr(colon + 1);
						break;
					}
				}
				
				if (response == "S_CONFIRMED") {
					printf("[Server P] User approves selling the stock.\n");
					// Update pf
					sellStock(uname, sTicker, sShares);
					// Send a sell result to Server M
					string sellConfirm = transID + ":s";
					if (sendto(sockfd, sellConfirm.c_str(), sellConfirm.length(), 0, (struct sockaddr*)&serverAddr, addrLen) == -1) {
						perror("Server P: sell sendto");
						continue;
					}
					printf("[Server P] Successfully sold %d shares of %s and updated %s’s portfolio.\n", 
						sShares, sTicker.c_str(), uname.c_str());
				}
				else if (response == "S_DENIED") {
					printf("[Server P] Sell denied.\n");
				}
			}
			else {
				// Not sufficient shares: reporting the issue to Server M
				stockStatus += "NOT_SUFF";
				if (sendto(sockfd, stockStatus.c_str(), stockStatus.length(), 0, (struct sockaddr*)&serverAddr, addrLen) == -1) {
					perror("Server P: sell sendto");
					continue;
				}
				printf("[Server P] Stock %s does not have enough shares in %s’s portfolio. Unable to sell %d shares of %s.\n", 
					sTicker.c_str(), uname.c_str(), sShares, sTicker.c_str());
			}
		}
		else if (payload[0] == 'p') { // a position request, format: pUname
			string uname = request.substr(colon + 2);
			printf("[Server P] Received a position request from the main server for Member: %s\n", uname.c_str());
			// Compose the portfolio message
			ostringstream oss;
			oss << transID << ":";
			for (const auto& stock : pf[uname]) {
				oss << stock.ticker << " ";
				oss << stock.shares << " ";
				oss << fixed << setprecision(2) << stock.avgPrice << "\n";
			}
			string response = oss.str();
			// Send the portfolio to Server M
			if (sendto(sockfd, response.c_str(), response.length(), 0, (struct sockaddr*)&serverAddr, addrLen) == -1) {
				perror("Server P: portfolio sendto");
				continue;
			}
			printf("[Server P] Finished sending the gain and portfolio of %s to the main server.\n", uname.c_str());
		}
	}
	close(sockfd);
	return 0;
}
