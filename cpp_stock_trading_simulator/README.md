# 💹 Stock Trading System (UNIX Socket Programming)

---

## 📖 About
This project implements a **distributed stock trading system** using **UNIX socket programming** on **Ubuntu 22.04 ARM64**.  
The system simulates client-server interactions for user authentication, portfolio management, and stock quotes.  

- Clients interact with **Server M** (the main server) over **TCP**.  
- Server M communicates with three backend servers — **Server A**, **Server P**, and **Server Q** — over **UDP**.  
- Backend servers handle authentication, portfolios, and stock quotes, respectively.  
- Users can:  
  - Log in securely with encrypted passwords  
  - View real-time stock quotes  
  - Buy and sell stocks  
  - Check their portfolio with profit/loss calculations  

Server M acts as the central controller, routing commands from clients to the appropriate backend servers and ensuring consistent state across the system.  

---

## ✨ Features
- **Secure login** with encrypted password verification (via Server A).  
- **Real-time stock quotes** (via Server Q).  
- **Portfolio management** with buy/sell operations (via Server P).  
- **Profit/loss calculation** for user positions.  
- **Persistent servers** that remain active until terminated.  
- Message-driven architecture with clear protocols for inter-server communication.  

---

## ⚙️ Installation
### Prerequisites
- [GCC / g++](https://gcc.gnu.org/) (for compiling C++ source code)  
- UNIX/Linux environment (tested on **Ubuntu 22.04 ARM64**)  

### Compilation
Clone this repository and compile the project:
```bash
git clone https://github.com/LeonardoZZN/Course_Projects.git
cd ./cpp_stock_trading_simulator
make
```

## 🚀 Usage
Startup Order
1. Start Server M first:
```bash
./serverM
```
2. Start the backend servers:
```bash
./serverA
./serverP
./serverQ
```
Each server loads its corresponding input file:
- serverA → members.txt (user credentials)
- serverP → portfolios.txt (user portfolios)
- serverQ → quotes.txt (stock prices)
3. Start one or more clients:
```bash
./client
```

## 🔧 Configuration
- Static port numbers are predefined in utility.h:
  - PORT_A = 41710
  - PORT_P = 42710
  - PORT_Q = 43710
  - PORT_M_UDP = 44710
  - PORT_M_TCP = 45710
- Localhost (127.0.0.1) is used for all communication.
- Servers must be started before clients, with Server M always first. (The UDP connections between backend servers must be set up before any requests from clients can be served.)

## 📂 Project Structure
```bash
.
├── Makefile		# Instructs the make utility
├── client.cpp      # Client interface connecting to Server M via TCP
├── serverM.cpp     # Main server (TCP with clients, UDP with backend servers)
├── serverA.cpp     # Authentication server (verifies user credentials)
├── serverP.cpp     # Portfolio server (manages user portfolios)
├── serverQ.cpp     # Quote server (manages stock prices)
├── utility.h       # Shared macros, constants, and UDP/TCP setup functions
├── members.txt     # Sample input file for authentication server
├── portfolios.txt  # Sample input file for portfolio server
├── quotes.txt      # Sample input file for quote server
└── README.md       # Project documentation
```

## 📡 Design of Message Formats 
### Authentication
- C→M: <username>,<password>
- M→A: <client port>:<username>,<encrypted password>
- A→M: <client port>:s (success) or <client port>:f (failure)
- M→C: s or f
### Quote (all stocks)
- C→M: qALL_STOCK
- M→Q: <client port>:ALL_STOCK
- Q→M: <client port>:<stock> <price>\n...
- M→C: <stock> <price>\n...
### Quote (specific stock)
- C→M: q<stock>
- M→Q: <client port>:<stock>
- Q→M: <client port>:<stock> <price> or <client port>:NOT_EXIST
- M→C: <stock> <price> or NOT_EXIST
### Buy
- C→M: b<stock>,<shares>
- M→Q: <client port>:<stock>
- Q→M: <client port>:<stock> <price> or NOT_EXIST
- M→C: <price> or NOT_EXIST
- C→M: Y / N (confirmation)
- M→P: <client port>:b<username>|<stock>,<shares>_<price>
- P→M: <client port>:s
- M→C: s
- M→Q: <client port>:+<stock> (advance time)
### Sell
- C→M: s<stock>,<shares>
- M→Q: <client port>:<stock>
- Q→M: <client port>:<stock> <price> or NOT_EXIST
- M→C: NOT_EXIST (if invalid stock)
- M→P: <client port>:s<username>|<stock>,<shares>
- P→M: <client port>:SUFF or NOT_SUFF
- M→C: <price> or NOT_SUFF
- C→M: Y / N
- M→P: <client port>:S_CONFIRMED / S_DENIED
- P→M: <client port>:s (if successful)
- M→C: s
- M→Q: <client port>:+<stock>
### Position
- C→M: p
- M→P: <client port>:p<username>
- P→M: <client port>:<stock> <shares> <avg_price>\n...
- M→Q: <client port>:p<stock1> <stock2> ...
- Q→M: <client port>:<price1> <price2> ...
- M→C: profit|<stock> <shares> <avg_price>\n...

## 📑 Reused Code
Some functions/snippets are cited from Beej's Guide to Network Programming:
- serverM.cpp: setupTCP(), code snippet for reaping dead processes.
- client.cpp: TCP connection setup snippet.
- utility.h: sigchld_handler(int s), setupUDP(...).

## ⚠️ Notes
Server M must be started first before launching backend servers and clients.\
Otherwise, TCP connections between clients and Server M will fail.

## 🤝 Contributing
1. Fork the repo
2. Create your feature branch (`git checkout -b feature/my-feature`)
3. Commit your changes (`git commit -m 'Add feature'`)
4. Push to the branch (`git push origin feature/my-feature`)
5. Open a Pull Request

## 📜 License
Distributed under the MIT License. See LICENSE for more information.

