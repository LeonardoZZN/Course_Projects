// Name: Leonardo Zhu
// USC NetID: ziningzh
// CS 455 PA4
// Fall 2023

/**
 * This class stores the information about the Scrabble scores associated with all letters
 * and provides a static method for clients to assess the score of a specific word.
 * Letters that occur more often in the English language are worth less, and vice versa.
 * This class works for both upper and lower case versions of the letters, e.g., 'a' and 'A' will have the same score.
 */
public class ScoreTable {
   // Constants: score values
   private static final int POINT_1 = 1;
   private static final int POINT_2 = 2;
   private static final int POINT_3 = 3;
   private static final int POINT_4 = 4;
   private static final int POINT_5 = 5;
   private static final int POINT_8 = 8;
   private static final int POINT_10 = 10;

   // The following integer array constant stores the scores for letters from either 'A' to 'Z' or 'a' to 'z'.
   private static final int[] scoreArray = {POINT_1, POINT_3, POINT_3, POINT_2, POINT_1, POINT_4, POINT_2, POINT_4,
         POINT_1, POINT_8, POINT_5, POINT_1, POINT_3, POINT_1, POINT_1, POINT_3, POINT_10, POINT_1, POINT_1,
         POINT_1, POINT_1, POINT_4, POINT_4, POINT_8, POINT_4, POINT_10};

   /**
    * Evaluate the Scrabble score of the input word by summing the scores of constituent letters of the word one by one.
    * As this method works for both uppercase and lowercase letters, it converts all letters to their lowercase forms
    * at the beginning.
    * PRE: input can only contain English letters in either upper or lower case without any other characters including
    * white spaces.
    * @param input the word to be evaluated for a Scrabble score
    * @return the Scrabble score corresponding to the input word ranges from 1 to 10. 0 means the input word is empty.
    */
   public static int getScore(String input) {
      int score = 0;
      String inputLower = input.toLowerCase();
      for (int i = 0; i < inputLower.length(); i++) {
         score += scoreArray[inputLower.charAt(i) - 'a'];
      }
      return score;
   }
}