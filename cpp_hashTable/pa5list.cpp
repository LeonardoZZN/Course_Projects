// Name: Leonardo Zhu
// USC NetID: ziningzh
// CSCI 455 PA5
// Fall 2023

// pa5list.cpp
// a program to test the linked list code necessary for a hash table chain

// You are not required to submit this program for pa5.

// We gave you this starter file for it so you don't have to figure
// out the #include stuff.  The code that's being tested will be in
// listFuncs.cpp, which uses the header file listFuncs.h

// The pa5 Makefile includes a rule that compiles these two modules
// into one executable.

#include <iostream>
#include <string>
#include <cassert>

using namespace std;

#include "listFuncs.h"


int main() {
   ListType list = NULL;
   printList(list);
   
   if (!listRemove(list, "Tim")) {
      cout << "listRemove works for empty list!" << endl;
   }
   
   listAppend(list, "April", 1);
   listAppend(list, "Bob", 2);
   listAppend(list, "Claire", 3);
   listAppend(list, "David", 4);
   printList(list);
   
   cout << numNodes(list) << endl;
   cout << endl;
   
   if (!listAppend(list, "Bob", 4)) {
      cout << "listAppend works for duplicate keys!" << endl;
   }
   
   cout << "The address of the value associated with Claire is " << listLookup(list, "Claire") << endl;
   
   listRemove(list, "April");
   printList(list);
   cout << endl;
   
   listRemove(list, "Bob");
   printList(list);
   cout << endl;
   
   if (!listRemove(list, "Bob")) {
      cout << "listRemove works for non-existing word" << endl;
   }
   cout << endl;
   
   listRemove(list, "Claire");
   printList(list);
   
   return 0;
}
