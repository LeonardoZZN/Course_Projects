// Name: Leonardo Zhu
// USC NetID: ziningzh
// CSCI 455 PA5
// Fall 2023

/*
 * grades.cpp
 * A program to test the Table class.
 * How to run it:
 *      grades [hashSize]
 * 
 * the optional argument hashSize is the size of hash table to use.
 * if it's not given, the program uses default size (Table::HASH_SIZE)
 *
 */

#include "Table.h"
#include <iostream>
#include <string>
#include <sstream>

// cstdlib needed for call to atoi
#include <cstdlib>

using namespace std;

void interface(Table * grades);

int main(int argc, char * argv[]) {


   Table * grades;  // Table is dynamically allocated below, so we can call
                     // different constructors depending on input from the user.
   
   // optionally gets the hash table size from the command line
   if (argc > 1) {
      int hashSize = atoi(argv[1]);  // atoi converts c-string to int
      
      if (hashSize < 1) {
         cout << "Command line argument (hashSize) must be a positive number" 
              << endl;
         return 1;
      }

      grades = new Table(hashSize);

   }
   else {   // no command line args given -- use default table size
      grades = new Table();
   }


   grades->hashStats(cout);

   interface(grades);
   
   return 0;
}

// Operation for "insert" command. 
// Read in a name and its associative score.
// Insert this name and score in the grade table.
// If this name was already present, print a message to that effect, and don't do the insert.
// @param stream the input stream
// @param grades the pointer to a Table object 
void insertOpt(stringstream & stream, Table * grades) {
   string name;
   stream >> name;
   int score;
   stream >> score;
   bool result = grades->insert(name, score);
   if (!result) {
      cout << "This name was already present. No change was made to the grade table." << endl;
   }
}

// Operation for "change" command. 
// Read in a name and a new score.
// Change the score for the given name
// Print an appropriate message if this name isn't present.
// @param stream the input stream
// @param grades the pointer to a Table object 
void changeOpt(stringstream & stream, Table * grades) {
   string name;
   stream >> name;
   int newscore;
   stream >> newscore;
   int * pointer = grades->lookup(name);
   if (pointer != NULL) {
      *pointer = newscore;
   } 
   else {
      cout << "This name is not present. Please insert a pair of name and score to the grade table first." << endl;
   }           
}

// Operation for "lookup" command. 
// Read in a name. Then lookup the name and print out his or her score
// Print an appropriate message if this name is not in the table.
// @param stream the input stream
// @param grades the pointer to a Table object 
void lookupOpt(stringstream & stream, Table * grades) {
   string name;
   stream >> name;
   int * pointer = grades->lookup(name);
   if (pointer != NULL) {
      cout << "The score of " << name << ": " << *pointer << endl;
   } 
   else {
      cout << "This name is not present. Please insert a pair of name and score to the grade table first." << endl;
   }
}

// Operation for "remove" command. 
// Read in a name. Then remove this student from the grade table.
// If this student wasn't in the grade table, print a message to that effect.
// @param stream the input stream
// @param grades the pointer to a Table object 
void removeOpt(stringstream & stream, Table * grades) {
   string name;
   stream >> name;
   bool result = grades->remove(name);
   if (!result) {
      cout << "This name is not present. No change was made to the grade table." << endl;
   }
}

// Print out a brief command summary
void printCmdSummary() {
   cout << "Valid commands are: insert name score, change name newscore, lookup name, " 
        << endl;
   cout << "                   remove name, print, size, stats, help, quit." 
        << endl;

}

// A helper function to repeatedly reads and executes commands from the user,
// printing out the command prompt (cmd>) after it finishes the previous command, 
// until the user enters the quit command.
// If a user give an invalid command name, this function would print out 
// "ERROR: invalid command" and the command summary.
// @param grades the pointer to a Table object
void interface(Table * grades) {
   bool keepgoing = true;
   string input;
   
   do {
      cout << "cmd>";
      // reads the whole line as a string
      getline(cin, input);
      stringstream stream(input);
      string command;
      stream >> command;
      
      if (command == "insert") {
         insertOpt(stream, grades);
      } 
      else if (command == "change") {
         changeOpt(stream, grades);
      } 
      else if (command == "lookup") {
         lookupOpt(stream, grades);
      }
      else if (command == "remove") {
         removeOpt(stream, grades);
      }
      else if (command == "print") {
         grades->printAll();
      } 
      else if (command == "size") {
         cout << "The number of entries in the table: " << grades->numEntries() << endl;
      }
      else if (command == "stats") {
         grades->hashStats(cout);
      }
      else if (command == "help") {
         printCmdSummary();
      }
      else if (command == "quit") {
         keepgoing = false;
      }
      else {
         cout << "ERROR: invalid command" << endl;
         printCmdSummary();
      }   

   } while (keepgoing);
}
 
