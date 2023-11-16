// Name: Leonardo Zhu
// USC NetID: ziningzh
// CS 455 PA4
// Fall 2023

import java.io.IOException;

/**
   This class extending the IOException class reports problems with the dictionary file.
 */
public class IllegalDictionaryException extends IOException {

   public IllegalDictionaryException() {}
   public IllegalDictionaryException(String message)
   {
      super(message);
   }
}
