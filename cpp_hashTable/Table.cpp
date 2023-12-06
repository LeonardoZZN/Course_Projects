// Name: Leonardo Zhu
// USC NetID: ziningzh
// CSCI 455 PA5
// Fall 2023

// Table.cpp  Table class implementation

// representation invariants:
// 1. hashSize is the size of a hash table as well as the number of buckets. hashSize >= 0
// 2. data is a dynamic array of hashSize in which each element is a linked ilst
// 3. numEntry is the total number of data tuples in a hash table. numEntry >= 0
// 4. numUsedBucket is the number of non-empty buckets. 0 <= numUsedBucket <= numEntry

#include "Table.h"

#include <iostream>
#include <string>
#include <cassert>

// for hash function called in private hashCode method defined below
#include <functional>

using namespace std;


// listFuncs.h has the definition of Node and its methods.  -- when
// you complete it it will also have the function prototypes for your
// list functions.  With this #include, you can use Node type (and
// Node*, and ListType), and call those list functions from inside
// your Table methods, below.

#include "listFuncs.h"

//*************************************************************************


Table::Table() {
   hashSize = HASH_SIZE;
   data = new ListType[hashSize]();
   numEntry = 0;
   numUsedBucket = 0;
}


Table::Table(unsigned int hSize) {
   hashSize = hSize;
   data = new ListType[hashSize]();
   numEntry = 0;
   numUsedBucket = 0;
}


int * Table::lookup(const string &key) {
   return listLookup(data[hashCode(key)], key);   
}


bool Table::remove(const string &key) {
   int code = hashCode(key);
   bool result = listRemove(data[code], key);  
   if (result) {
      numEntry--;
      if (data[code] == NULL) {
         numUsedBucket--;
      }
   }
   return result;
}


bool Table::insert(const string &key, int value) {
   int code = hashCode(key);
   bool emptyBucket = false;
   if (data[code] == NULL) {
      emptyBucket = true;
   }
       
   bool result = listAppend(data[code], key, value);
   if (result) {
      numEntry++;
      if (emptyBucket) {
         numUsedBucket++;
      }
   }
   return result;  
}


int Table::numEntries() const {
   return numEntry;      
}


void Table::printAll() const {
   for (int i = 0; i < hashSize; i++) {
      printList(data[i]);
   }
}


void Table::hashStats(ostream &out) const {
   out << "number of buckets: " << hashSize << endl;
   out << "number of entries: " << numEntry << endl;
   out << "number of non-empty buckets: " << numUsedBucket << endl;
   out << "longest chain: " << maxChainLen() << endl;
}


// hash function for a string
// returns a value in the range [0, hashSize)
unsigned int Table::hashCode(const string &word) const {

   // Note: calls a std library hash function for string (it uses the good hash
   //   algorithm for strings that we discussed in lecture).
   return hash<string>()(word) % hashSize;

}


// definitions for private methods here

// traverse all buckets of the table to search for the longest chain
// and then update the max chain length
int Table::maxChainLen() const {
   int maxLen = 0;
   for (int i = 0; i < hashSize; i++) {
      int len = numNodes(data[i]);
      if (len > maxLen) {
         maxLen = len;
      }
   }
   return maxLen;
}

