// Name: Leonardo Zhu
// USC NetID: ziningzh
// CS 455 PA4
// Fall 2023

/**
 * This console-based program prompts users to give stings in the form of English letters so that the constituent
 * letters can comprise a Scrabble rack stored in a Rack object. Then this program creates a list of all legal words
 * that can be found from the letters on that rack, where the legal words are defined by a dictionary optionally
 * input from the command-line argument. If the specific dictionary is not prescribed by users, this program will
 * automatically use the default Scrabble dictionary called "SOWPODS" for reference. The dictionary will be
 * preprocessed by constructing a AnagramDictionary object to group anagrams into different sets for later use.
 * The output list of legal words is sorted in decreasing order by Scrabble scores which can be computed by a static
 * method defined in the ScoreTable class.
 * @author Zining Zhu
 * @version 1.01 2023-11-15
 */
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Scanner;

/**
 * This class contains the main method to be responsible for processing the command-line argument and the user input,
 * handling any error processing, and printing out the resultant word list of all legal anagrams for given racks.
 * It works with the AnagramDictionary class to process given dictionary and to search for legal words. A Rack object
 * will be initialized to figure out all possible letter combinations for a string given by the user. The ScoreTable
 * class provides a static method to calculate the Scrabble scores for any given words, which are printed in the
 * final word list.
 */
public class WordFinder {
   public static void main(String[] args) {
      String fileName;
      if (args.length == 0 || args[0].charAt(0) == '-') {
         fileName = "sowpods.txt";
      }
      else {
         fileName = args[0];
      }

      // Preprocess the dictionary
      AnagramDictionary anagramDict = processDict(fileName);
      if (anagramDict == null) {
         return;
      }

      // Initial prompt:
      System.out.println("Type . to quit.");
      System.out.print("Rack? ");
      Scanner in = new Scanner(System.in);
      while (in.hasNext()) {
         String input = in.next();
         if (input.equals(".")) {
            return;
         }
         else {
            Rack rack = new Rack(input);
            ArrayList<String> words = anagramDict.possibleWords(rack);
            System.out.println("We can make " + words.size() + " words from \"" + input + "\"");
            if (!words.isEmpty()) {
               System.out.println("All of the words with their scores (sorted by score):");
               for (String str : words) {
                  System.out.println(ScoreTable.getScore(str) + ": " + str);
               }
            }
            System.out.print("Rack? ");
         }
      }
   }

   /**
    * Convert the given dictionary indicated by the fileName to a dictionary built from sets of anagrams.
    * This method handles two possible exceptions when processing the given dictionary by warning the user
    * with an error message and returning an empty anagramDict that will trigger the termination of the main program.
    * @param fileName the name identifies the location of the given dictionary
    * @return an anagram dictionary constructed based on the given dictionary. If the original dictionary is not
    * found or is illegal, a null object will be returned.
    */
   private static AnagramDictionary processDict(String fileName) {
      AnagramDictionary dict = null;
      try {
          dict = new AnagramDictionary(fileName);
      }
      catch (FileNotFoundException exception) {
         System.out.println("ERROR: Dictionary file \"" + fileName + "\" does not exist.");
         System.out.println("Exiting program.");
      }
      catch (IllegalDictionaryException exception) {
         System.out.println(exception.getMessage());
         System.out.println("Exiting program.");
      }
      return dict;
   }
}

