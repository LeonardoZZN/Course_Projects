// Name: Leonardo Zhu
// USC NetID: ziningzh
// CS 455 PA4
// Fall 2023

import java.io.File;
import java.io.FileNotFoundException;
import java.util.*;

/**
 * A dictionary of all anagram sets derived from processing an input dictionary indicated by a file name.
 * This class has a getAnagramsOf method that finds all anagrams of a particular string efficiently.
 * Besides, for an input Rack object, it also provides a possibleWords method to create a list of all anagrams
 * that can be found in the constructed dictionary object, where the list is sorted in decreasing order by Scrabble
 * scores and in alphabetical order when two words have the same score.
 * Note: the processing is case-sensitive; so if the dictionary has all lower case words,
 * there will possibly exist matched anagram sets only when the string to test has at least
 * one lower case letters, and likewise if the dictionary words are all upper case.
 */
public class AnagramDictionary {
   /**
    * Representation invariants:
    * 1. 0 <= anagramDict.size() <= the length of the input dictionary
    * 2. 0 <= anagramDict.get(key).size() <= the length of the input dictionary, for any key
    */
   private Map<String, Set<String>> anagramDict;

   /**
    * Create an anagram dictionary from the list of words given in the file indicated by fileName.
    * @param fileName  the name of the file to read from
    * @throws FileNotFoundException  if the file is not found
    * @throws IllegalDictionaryException  if the dictionary has any duplicate words
    */
   public AnagramDictionary(String fileName) throws FileNotFoundException,
                                                    IllegalDictionaryException {
      File dictFile = new File(fileName);
      try(Scanner in = new Scanner(dictFile)) {
         anagramDict = constructDict(in);
      }
   }

   /**
    * Construct an anagram dictionary by converting the input words into sorted character arrays for comparison
    * and then adding the words into correct HashSets of the corresponding anagrams to avoid duplicate words
    * @param in the input stream to read the given original dictionary
    * @return a built dictionary of anagrams
    * @throws IllegalDictionaryException  if the dictionary has any duplicate words
    */
   private static Map<String, Set<String>> constructDict(Scanner in) throws IllegalDictionaryException{
      Map<String, Set<String>> dictMap = new HashMap<>();
      while (in.hasNext()) {
         String word = in.next();
         char[] charArray = word.toCharArray();
         // Use the quicksort algorithm to sort the character array. Time complexity: O(nlog(n))
         Arrays.sort(charArray);
         String sortedWord = new String(charArray);

         // Create a new HashSet for a new Key
         if (!dictMap.containsKey(sortedWord)) {
            dictMap.put(sortedWord, new HashSet<String>());
         }

         // Add input words to the corresponding HashSets and check if there are duplicate words
         if (!dictMap.get(sortedWord).add(word)) {
            throw new IllegalDictionaryException(
                  "ERROR: Illegal dictionary: dictionary file has a duplicate word: " + word);
         }
      }
      return dictMap;
   }

   /**
    * Find all anagrams of the given string. This method is case-sensitive.
    * E.g. "CARE" and "race" would not be recognized as anagrams.
    * @param string string to process
    * @return a list of the anagrams for a given string.
    *         If no corresponding anagrams exist, the returned list is empty.
    */
   public ArrayList<String> getAnagramsOf(String string) {
      char[] charArray = string.toCharArray();
      Arrays.sort(charArray);
      String sortedInput = new String(charArray);

      ArrayList<String> anagramList = new ArrayList<>();
      // Iterate over all elements of the anagram set to add them to the ArrayList
      // Time complexity: O(anagramDict.get(sortedInput).size())
      if (anagramDict.containsKey(sortedInput)) {
         anagramList.addAll(anagramDict.get(sortedInput));
      }
      return anagramList;
   }

   /**
    * Acquire all anagram sets included in the given dictionary for all the subsets of a given rack multiset.
    * Return the complete word list sorted in decreasing order by Scrabble scores and also in alphabetical order
    * if two words have the same score.
    * @param rack a given multiset of strings whose subsets represent all possible combinations of letters provided
    *             in the construction of the Rack object.
    * @return a sorted list of strings that can be found in the given dictionary for a given rack.
    */
   public ArrayList<String> possibleWords(Rack rack) {
      ArrayList<String> words = new ArrayList<>();
      for (String str : rack.getRack()) {
         ArrayList<String> anagrams = this.getAnagramsOf(str);
         if (!anagrams.isEmpty()) {
            words.addAll(anagrams);
         }
      }
      Collections.sort(words); // Sort the list in alphabetical order at first
      Collections.sort(words, new WordComparator());
      return words;
   }


   /**
    * An inner class that implements the Comparator interface, used to sort the output word rack
    * in decreasing order of Scrabble score.
    */
   static class WordComparator implements Comparator<String> {
      /**
       * Compare the Scrabble scores of two strings.
       * @param a the first string to be compared.
       * @param b the second string to be compared.
       * @return the comparison result that is negative if the first object has a score greater than that of
       * the second object, equal if the scores are the same, and positive if the first object has a score
       * less than that of the second object.
       */
      public int compare(String a, String b) {
         return ScoreTable.getScore(b) - ScoreTable.getScore(a);
      }
   }

}
