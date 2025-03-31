// Name: Leonardo Zhu
// USC NetID: ziningzh
// CS 455 PA4
// Fall 2023

import java.util.ArrayList;
import java.util.Map;
import java.util.TreeMap;

/**
 *  A Rack of Scrabble tiles contains all possible subsets of letter combinations based on an input string,
 *  even if some combinations are not real English words.
 *  In the constructor, the input string is first transformed into a character array which only includes
 *  English letters, followed by that a TreeMap object is used to sort the character array into a key set
 *  and count the multiplicity of each unique letter into a value set.
 *  All possible subsets of a multiset will be determined recursively by a method called allSubsets.
 */

public class Rack {
   /**
    * Representation invariants:
    * 1. 0 <= subsets.size() <= 2^(the length of characters in the input string)
    * 2. Each element in subsets is a string that is a specific recombination of partial or all letters originally
    * included in the input string.
    **/
   private ArrayList<String> subsets;

   public Rack(String input) {
      char[] inputChars = input.toCharArray();
      // Use a TreeMap to find the string of unique letters and the array of multiplicity
      Map<Character, Integer> uniqueMap = new TreeMap<>();
      for (char ele : inputChars) {
         if (('A' <= ele && ele <= 'Z') || ('a' <= ele && ele <= 'z')) { // Remove all non-English-letter chars
            if (!uniqueMap.containsKey(ele)) {
               uniqueMap.put(ele, 1);
            }
            else {
               uniqueMap.put(ele, uniqueMap.get(ele) + 1);
            }
         }
      }
      // Initialize a string composed of unique letters and an int array storing the multiplicity of letters
      String rackUnique = "";
      int[] rackMult = new int[uniqueMap.size()];
      int pointer = 0;
      for (Map.Entry<Character, Integer> curr : uniqueMap.entrySet()) {
         rackUnique += curr.getKey();
         rackMult[pointer] = curr.getValue();
         pointer++;
      }
      // Get all possible subsets
      subsets = getSubsets(rackUnique, rackMult);
   }

   /**
    * Get a copy of all the subsets in terms of string in the rack. The list of subsets is not necessarily sorted
    * in the alphabetical order (e.g., when there are repeated letters).
    * @return a copy of rack subsets of a string multiset
    */
   public ArrayList<String> getRack() {
      return new ArrayList<>(subsets); // Make a defensive copy
   }

   /**
    * A wrapper method that calls allSubsets with the correct starting parameters (index k = 0) outputs all possible
    * subsets of a string multiset represented by a string of unique letters and an integer array of multiplicity of
    * each unique letter.
    * PRE: mult.length must be at least as big as unique.length()
    *      0 <= k <= unique.length()
    * @param unique a string of unique letters.
    * @param mult the multiplicity of each letter from unique.
    * @return all subsets of an input string represented by unique and mult
    */
   private static ArrayList<String> getSubsets(String unique, int[] mult) {
      return allSubsets(unique, mult, 0);
   }

   /**
    * Finds all subsets of the multiset starting at position k in unique and mult.
    * unique and mult describe a multiset such that mult[i] is the multiplicity of the char: unique.charAt(i).
    * PRE: mult.length must be at least as big as unique.length()
    * 0 <= k <= unique.length()
    * @param unique a string of unique letters
    * @param mult the multiplicity of each letter from unique.
    * @param k the smallest index of unique and mult to consider.
    * @return all subsets of the indicated multiset.  Unlike the multiset in the parameters,
    * each subset is represented as a String that can have repeated characters in it.
    * @author Claire Bono
    */
   private static ArrayList<String> allSubsets(String unique, int[] mult, int k) {
      ArrayList<String> allCombos = new ArrayList<>();
      
      if (k == unique.length()) {  // multiset is empty
         allCombos.add("");
         return allCombos;
      }
      
      // get all subsets of the multiset without the first unique char
      ArrayList<String> restCombos = allSubsets(unique, mult, k+1);
      
      // prepend all possible numbers of the first char (i.e., the one at position k) 
      // to the front of each string in restCombos.  Suppose that char is 'a'...
      
      String firstPart = "";          // in outer loop firstPart takes on the values: "", "a", "aa", ...
      for (int n = 0; n <= mult[k]; n++) {   
         for (int i = 0; i < restCombos.size(); i++) {  // for each of the subsets 
                                                        // we found in the recursive call
            // create and add a new string with n 'a's in front of that subset
            allCombos.add(firstPart + restCombos.get(i));  
         }
         firstPart += unique.charAt(k);  // append another instance of 'a' to the first part
      }
      
      return allCombos;
   }
   
}
