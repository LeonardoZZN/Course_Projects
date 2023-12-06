// Name: Leonardo Zhu
// USC NetID: ziningzh
// CSCI 455 PA5
// Fall 2023


//*************************************************************************
// Node class definition and declarations for functions on ListType.
// The data members of this particular type of node defined here are
// a string named key, an integer named value, and another Node named next.

// Note: we don't need Node in Table.h
// because it's used by the Table class; not by any Table client code.

// Note2: it's good practice to *not* put "using" statement in *header* files.  Thus
// here, things from std libary appear as, for example, std::string

#ifndef LIST_FUNCS_H
#define LIST_FUNCS_H

#include <string>
  
// define a Node structure including a string "key" and an integer "value"
struct Node {
   std::string key;
   int value;

   Node *next;

   // construct a single new node, and point it to NULL
   // @param theKey the key of the new node
   // @param theValue the value of the new node
   Node(const std::string &theKey, int theValue);

   // construct a single new node, and point it to another node
   // @param theKey the key of the new node
   // @param theValue the value of the new node
   Node(const std::string &theKey, int theValue, Node *n);
};


typedef Node * ListType;

//*************************************************************************
//add function headers (aka, function prototypes) for your functions
//that operate on a list here (i.e., each includes a parameter of type
//ListType or ListType&).  No function definitions go in this file.

// append a new node at the tail of a linked list.
// return false iff this key was already present (and no change made to table)         
// @param list a ListType passed by reference
// @param theKey the key of the new node to be appended
// @param theValue the value of the new node to be appended
// @return a boolean value to indicate whether the appending operation succeed
bool listAppend(ListType &list, const std::string &theKey, int theValue);

// return the address of the value that goes with a given key
// or NULL if key is not present.
// @param list a ListType passed by reference but used as a copy
// @param theKey the key used to look up the value of interest
// @return the address of the value of interest
int *listLookup(const ListType &list, const std::string &theKey);

// remove a node given its key from a given list
// return false iff key wasn't present (and no change made to table)
// @param list a ListType passed by reference
// @param theKey the key of the node to be removed
// @return a boolean value to indicate whether the removing operation succeed
bool listRemove(ListType &list, const std::string &theKey);

// print out data members of all the nodes in the table, one per line, to cout
// Sample output:
//   zhou 283
//   sam 84
//   babs 99
// @param list the linked list to be printed
void printList(const ListType &list);

// count the number of nodes in a given list
// @param list the linked list of interest
// @return the number of nodes
int numNodes(const ListType &list);


// keep the following line at the end of the file
#endif
