/* 	This file implements the authentication server (Server A) to authenticate users against its stored credentials 
 *	before users can access the other functions of this stock trading system. 
 */

#include "utility.h"
using namespace std;

// Global Variable 
unordered_map<string, string> credentials; // structure storing members' info

/*
 * Convert a string to lowercase.
 * @param input the given string to be converted
 * @return the converted string in lowercase
 */
string strToLower(const string& input) {
	string output = input;
	for (char& c : output) {
		c = tolower(c);
	}
	return output;
}

/*
 * Load credentials of members from the input members.txt to the global variable. 
 */
void loadMembers() {
	string line, uname, password;
	int space;
	ifstream file("./members.txt");
	if (file.is_open()) {
		while (getline(file, line)) {
			space = line.find(' ');
			uname = line.substr(0, space);
			password = line.substr(space + 1);
			credentials[strToLower(uname)] = password;
		}
		file.close();
	}
	else {
		perror("Failed to open members.txt");
		exit(1);
	}
}

/*
 * Check username and encrypted password against the input members.txt.
 * @param uname username passed by the main server
 * @param encrypted the encrypted password passed from the main server
 * @return the result of authentication. 0: Failed. 1: Succeeded.
 */
bool authenticate(const string& uname, const string& encrypted) {
	auto it = credentials.find(strToLower(uname));
	if (it != credentials.end()) {
		return  encrypted == it->second;
    }
    else {
		return false;
    }
}

int main() {
	int sockfd;
	int numbytes;
	char buf[MAXBUFSIZE];
	string uname, encrypted, result;
	struct sockaddr_in serverAddr; // socket address of Server M
	socklen_t addrLen = sizeof(serverAddr);
	
	// Bootup
	// Load input file
	loadMembers();
	// Set up UDP socket
	printf("[Server A] Booting up using UDP on port %s.\n", PORT_A);
	sockfd = setupUDP('A', PORT_A);
	
	while (1) {
		// Receive an authentication request from Server M
		if ((numbytes = recvfrom(sockfd, buf, MAXBUFSIZE - 1, 0, (struct sockaddr*)&serverAddr, &addrLen)) == -1) {
			perror("Server A: recvfrom");
			continue;
		}
		buf[numbytes] = '\0';

		// Parse the request
		string authRequest(buf), transID, payload;
		int colon = authRequest.find(':');
		transID = authRequest.substr(0, colon);
		payload = authRequest.substr(colon + 1);
		int comma = payload.find(',');
		uname = payload.substr(0, comma);
		encrypted = payload.substr(comma + 1);
		printf("[Server A] Received username %s and password ******.\n", uname.c_str());

		// Compose the authentication response
		if (authenticate(uname, encrypted)) { // send "s" to represent success
			result = transID + ":s";
			printf("[Server A] Member %s has been authenticated.\n", uname.c_str());
		}
		else { // send "f" to represent failure
			result = transID + ":f";
			printf("[Server A] The username %s or password ****** is incorrect.\n", uname.c_str());
		}

		// Send the authentication result to Server M via UDP
		sendto(sockfd, result.c_str(), result.length(), 0, (struct sockaddr*)&serverAddr, addrLen);
	}
	
	close(sockfd);
	return 0;
}
		














