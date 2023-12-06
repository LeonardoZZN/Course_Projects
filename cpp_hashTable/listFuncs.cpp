// Name: Leonardo Zhu
// USC NetID: ziningzh
// CSCI 455 PA5
// Fall 2023


// listFuncs.cpp
//    the implementation of the declared member functions of the Node class
//    and the implementation of the declared functions on ListType.
#include <iostream>

#include <cassert>

#include "listFuncs.h"

using namespace std;

Node::Node(const string &theKey, int theValue) {
   key = theKey;
   value = theValue;
   next = NULL;
}

Node::Node(const string &theKey, int theValue, Node *n) {
   key = theKey;
   value = theValue;
   next = n;
}


//*************************************************************************
bool listAppend(ListType &list, const std::string &theKey, int theValue) {  
   // for empty list
   if (list == NULL) {
      list = new Node(theKey, theValue);
      return true;
   }
   
   ListType p = list;
   // if the list is not empty
   while (p->next != NULL) {
      if (p->key == theKey) {
         return false; // theKey was already present (and no change made to the list)
      }
      p = p->next;
   }
   
   // check the last node
   if (p->key == theKey) {
      return false; // theKey was already present (and no change made to the list)
   }
   
   p->next = new Node(theKey, theValue);
   return true;
}


int *listLookup(const ListType &list, const std::string &theKey) {
   ListType p = list;
   
   while (p != NULL) {
      if (p->key == theKey) {
         return &(p->value);
      }
      p = p->next;
   }
   
   return NULL;
}


bool listRemove(ListType &list, const std::string &theKey) {
   // for empty list
   if (list == NULL) {
      return false;
   }
   
   ListType p = list;
   // check the first node
   if (p->key == theKey) {
      list = p->next;
      delete p; // reclaim memory
      return true;
   }
   
   // if there are more than one node in the list
   while (p->next != NULL) {
      if (p->next->key == theKey) {
         ListType temp = p->next;
         p->next = p->next->next;
         delete temp; // reclaim memory
         return true;       
      }
      p = p->next;
   }
   return false;
}


void printList(const ListType &list) {
   ListType p = list;
   while (p != NULL) {
      cout << p->key << " " << p->value << endl;
      p = p->next;
   }
}


int numNodes(const ListType &list) {
   int counter = 0;
   ListType p = list;
   while (p != NULL) {
      counter++;
      p = p->next;
   }
   return counter;
}