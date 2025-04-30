#Makefile for EE450 Spring 2025 Project
CXX = g++
CXXFLAGS = -Wall -g -std=c++11

# Executables
EXECUTABLES = client serverM serverA serverP serverQ 


all: $(EXECUTABLES)

client: client.cpp utility.h
	$(CXX) $(CXXFLAGS) -o client client.cpp

serverM: serverM.cpp utility.h
	$(CXX) $(CXXFLAGS) -o serverM serverM.cpp

serverA: serverA.cpp utility.h
	$(CXX) $(CXXFLAGS) -o serverA serverA.cpp

serverP: serverP.cpp utility.h
	$(CXX) $(CXXFLAGS) -o serverP serverP.cpp

serverQ: serverQ.cpp utility.h
	$(CXX) $(CXXFLAGS) -o serverQ serverQ.cpp

# Clean target
clean:
	rm -f $(EXECUTABLES)
