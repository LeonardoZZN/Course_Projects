// Name: Leonardo Zhu
// USC NetID: ziningzh
// CSCI 455 PA5
// Fall 2023


#ifndef TABLE_H
#define TABLE_H

// Table class
//    stores a collection of (string, int) entries
//    such that each key (the string) is unique
//    with efficient (O(1)) lookup, insert, and removal of entries.
//    This hash table adopted separate chaining to deal with collisions.

// Note: it's good practice to *not* put "using" statement in *header* files.  Thus
// here, things from std libary appear as, for example, std::string


#include <iostream>
#include <string>

// the following line of code is a forward declaration of Node struct.
// allows us to use Node* (or ListType because of typedef) in private 
// section of Table below. (Complete Node definition is in listFuncs.h)
struct Node;

typedef Node * ListType;

class Table {
 public:

   static const unsigned int HASH_SIZE = 9973;  // a prime number

   // create an empty table, i.e., one where numEntries() is 0
   // (Underlying hash table is HASH_SIZE.)
   Table();

   // create an empty table, i.e., one where numEntries() is 0
   // such that the underlying hash table is hSize
   // PRE: hSize >= 0
   Table(unsigned int hSize);

   // insert a new pair into the table
   // return false iff this key was already present 
   //         (and no change made to table)
   // @param key the key of the node to be inserted
   // @param value the value of the node to be inserted
   // @return a boolean value indicating the status of this operation
   bool insert(const std::string &key, int value);

   // return the address of the value that goes with this key
   // or NULL if key is not present.
   //   Thus, this method can be used to either lookup the value or
   //   update the value that goes with this key.
   // @param key the key used to look up its associative value
   // @return the address of the value for the given key
   int *lookup(const std::string &key);

   // remove a pair given its key
   // return false iff key wasn't present
   //         (and no change made to table)
   // @param key the key of the node to be removed
   // @return a boolean value indicating the status of this operation
   bool remove(const std::string &key);

   // print out all the entries in the table, one per line, to cout
   // Sample output:
   //   zhou 283
   //   sam 84
   //   babs 99
   void printAll() const;

   // count the number of data tuples in the table
   // @return the number of entries in the table
   int numEntries() const;      

   // hashStats: for diagnostic purposes only
   // prints out info to let us know if we're getting a good distribution
   // of values in the hash table.
   // Sample output from this function
   //   number of buckets: 997
   //   number of entries: 10
   //   number of non-empty buckets: 9
   //   longest chain: 2
   // @param out the ostream that can be cout or cerr
   void hashStats(std::ostream &out) const;

 private:

   //***********do not change the following two lines of code*************
   // making these private disallows copying of tables  
   // (do not implement these two methods)
   Table &operator =(const Table &);
   Table(const Table &);

   unsigned int hashCode(const std::string &word) const;

   // private data and private method headers below:
   
   // representation invariants:
   // 1. hashSize is the size of a hash table as well as the number of buckets. hashSize >= 0
   // 2. data is a dynamic array of hashSize in which each element is a linked ilst
   // 3. numEntry is the total number of data tuples in a hash table. numEntry >= 0
   // 4. numUsedBucket is the number of non-empty buckets. 0 <= numUsedBucket <= numEntry

   unsigned int hashSize; // size of the hash table 

   ListType * data; 
   
   int numEntry;
   
   int numUsedBucket;
   
   // evaluate the length of the longest chain among all buckets in the table
   // @return number of nodes in the longest chain 
   int maxChainLen() const;
   
};

#endif

