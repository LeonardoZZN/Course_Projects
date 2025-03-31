// Name: Zining Zhu
// USC NetID: ziningzh
// CS 455 PA3
// Fall 2023

import java.util.Arrays;
import java.util.Random;
/** 
   MineField
      Class with locations of mines for a minesweeper game.
      This class is mutable, because we sometimes need to change it once it's created.
      Mutators: populateMineField, resetEmpty
      Includes convenience method to tell the number of mines adjacent to a location.
 */
public class MineField {
   private static final double LOC_FRACTION = 1 / 3.0; // Used for constraining the number of mines

   /**
    * Representation invariants:
    * 1. minesLoc.length > 0.
    * 2. minesLoc[0].length > 0.
    * 3. minesLoc is a rectangular 2D array.
    * 4. 0 <= minesNum < (1/3 * minesLoc.length * minesLoc[0].length)
    */
   // <put instance variables here>
   private boolean[][] minesLoc;
   private int minesNum;

   /**
      Creates a minefield with same dimensions as the given array, and populate it with the mines in
      the array such that if mineData[row][col] is true, then hasMine(row,col) will be true and vice
      versa.  numMines() for this minefield will correspond to the number of 'true' values in
      mineData.
      @param mineData  the data for the mines; must have at least one row and one col,
                       and must be rectangular (i.e., every row is the same length)
    */
   public MineField(boolean[][] mineData) {
      // Makes a defensive copy and count the number of mines
      boolean[][] dataCopy = new boolean[mineData.length][mineData[0].length];
      int mineCounter = 0;
      for (int i = 0; i < mineData.length; i++) {
         for (int j = 0; j < mineData[0].length; j++) {
            dataCopy[i][j] = mineData[i][j];
            if (mineData[i][j]) {
               mineCounter++;
            }
         }
      }
      minesLoc = dataCopy;
      minesNum = mineCounter;
   }


   /**
      Create an empty minefield (i.e. no mines anywhere), that may later have numMines mines (once 
      populateMineField is called on this object).  Until populateMineField is called on such a 
      MineField, numMines() will not correspond to the number of mines currently in the MineField.
      @param numRows  number of rows this minefield will have, must be positive
      @param numCols  number of columns this minefield will have, must be positive
      @param numMines   number of mines this minefield will have,  once we populate it.
      PRE: numRows > 0 and numCols > 0 and 0 <= numMines < (1/3 of total number of field locations). 
    */
   public MineField(int numRows, int numCols, int numMines) {
      assert numRows > 0 && numCols > 0 && 0 <= numMines &&  numMines < (int) (LOC_FRACTION * numRows * numCols);

      minesLoc = new boolean[numRows][numCols];
      minesNum = numMines;
   }
   

   /**
      Removes any current mines on the minefield, and puts numMines() mines in random locations on
      the minefield, ensuring that no mine is placed at (row, col).
      @param row the row of the location to avoid placing a mine
      @param col the column of the location to avoid placing a mine
      PRE: inRange(row, col) and numMines() < (1/3 * numRows() * numCols())
    */
   public void populateMineField(int row, int col) {
      assert inRange(row, col) && minesNum < (int) (LOC_FRACTION * numRows() * numCols());

      resetEmpty();

      //Puts mines in random locations
      Random generator = new Random();
      int numR = numRows();
      int numC = numCols();
      int locCounter = 1;
      while (locCounter <= minesNum) {
         int randomRow = generator.nextInt(numR);
         int randomCol = generator.nextInt(numC);
         if ((randomRow != row || randomCol != col) && !minesLoc[randomRow][randomCol]) {
            minesLoc[randomRow][randomCol] = true;
            locCounter++;
         }
      }
   }
   
   
   /**
      Resets the minefield to all empty squares.  This does not affect numMines(), numRows() or
      numCols().  Thus, after this call, the actual number of mines in the minefield does not match
      numMines().  
      Note: This is the state a minefield created with the three-arg constructor is in at the 
      beginning of a game.
    */
   public void resetEmpty() {
      for (int i = 0; i < minesLoc.length; i++) {
         for (int j = 0; j < minesLoc[0].length; j++) {
            minesLoc[i][j] = false;
         }
      }
   }

   
  /**
     Returns the number of mines adjacent to the specified location (not counting a possible 
     mine at (row, col) itself).
     Diagonals are also considered adjacent, so the return value will be in the range [0,8]
     @param row  row of the location to check
     @param col  column of the location to check
     @return  the number of mines adjacent to the square at (row, col)
     PRE: inRange(row, col)
   */
   public int numAdjacentMines(int row, int col) {
      assert inRange(row, col);

      int counter = 0;
      if (row == 0 && col == 0) { // Upper left corner
         counter = upperLeftNum();
      }
      else if (row == 0 && col == numCols() - 1) { // Upper right corner
         counter = upperRightNum();
      }
      else if (row == numRows() - 1 && col == 0) { // Lower left corner
         counter = lowerLeftNum();
      }
      else if (row == numRows() - 1 && col == numCols() - 1) { // Lower right corner
         counter = lowerRightNum();
      }
      else if (row == 0) { // Top boundary
         counter = topNum(col);
      }
      else if (row == numRows() - 1) { // Bottom boundary
         counter = bottomNum(col);
      }
      else if (col == 0) { // Left boundary
         counter = leftNum(row);
      }
      else if (col == numCols() - 1) { // Right boundary
         counter = rightNum(row);
      }
      else { // Inside the field without touching any boundary
         counter = centralNum(row, col);
      }

      assert 0 <= counter && counter <= 8;
      return counter;
   }
   
   
   /**
      Returns true iff (row,col) is a valid field location.  Row numbers and column numbers
      start from 0.
      @param row  row of the location to consider
      @param col  column of the location to consider
      @return whether (row, col) is a valid field location
   */
   public boolean inRange(int row, int col) {
      return (0 <= row && row < minesLoc.length) && (0 <= col && col < minesLoc[0].length);
   }
   
   
   /**
      Returns the number of rows in the field.
      @return number of rows in the field
   */  
   public int numRows() {
      return minesLoc.length;
   }
   
   
   /**
      Returns the number of columns in the field.
      @return number of columns in the field
   */    
   public int numCols() {
      return minesLoc[0].length;
   }
   
   
   /**
      Returns whether there is a mine in this square
      @param row  row of the location to check
      @param col  column of the location to check
      @return whether there is a mine in this square
      PRE: inRange(row, col)   
   */    
   public boolean hasMine(int row, int col) {
      assert inRange(row, col);
      return minesLoc[row][col];
   }
   
   
   /**
      Returns the number of mines you can have in this minefield.  For mines created with the 3-arg
      constructor, some of the time this value does not match the actual number of mines currently
      on the field.  See doc for that constructor, resetEmpty, and populateMineField for more
      details.
      @return number of mines
    */
   public int numMines() {
      return minesNum;
   }


   /**
    * Overrides the toString method to print the state of objects
    * @return instance variables
    */
   public String toString() {
      return "the minefield is: " + Arrays.deepToString(minesLoc)
            + "\\n The number of mines: " + minesNum;
   }


   // <put private methods here>
   /**
    * Returns the number of mines adjacent to the upper left corner (minesLoc[0][0]).
    * @return number of mines around the upper left corner
    */
   private int upperLeftNum() {
      int counter = 0;
      if (minesLoc[0][1]) {
         counter++;
      }
      if (minesLoc[1][0]) {
         counter++;
      }
      if (minesLoc[1][1]) {
         counter++;
      }
      return counter;
   }


   /**
    * Returns the number of mines adjacent to the upper right corner (minesLoc[0][numCols() - 1]).
    * @return number of mines around the upper right corner
    */
   private int upperRightNum() {
      int counter = 0;
      int lastCol = numCols() - 1;
      if (minesLoc[0][lastCol - 1]) {
         counter++;
      }
      if (minesLoc[1][lastCol - 1]) {
         counter++;
      }
      if (minesLoc[1][lastCol]) {
         counter++;
      }
      return counter;
   }


   /**
    * Returns the number of mines adjacent to the lower left corner (minesLoc[numRows() - 1][0]).
    * @return number of mines around the lower left corner
    */
   private int lowerLeftNum() {
      int counter = 0;
      int lastRow = numRows() - 1;
      if (minesLoc[lastRow - 1][0]) {
         counter++;
      }
      if (minesLoc[lastRow - 1][1]) {
         counter++;
      }
      if (minesLoc[lastRow][1]) {
         counter++;
      }
      return counter;
   }


   /**
    * Returns the number of mines adjacent to the lower right corner (minesLoc[numRows() - 1][numCols() - 1]).
    * @return number of mines around the lower right corner
    */
   private int lowerRightNum() {
      int counter = 0;
      int lastRow = numRows() - 1;
      int lastCol = numCols() - 1;
      if (minesLoc[lastRow - 1][lastCol]) {
         counter++;
      }
      if (minesLoc[lastRow - 1][lastCol - 1]) {
         counter++;
      }
      if (minesLoc[lastRow][lastCol - 1]) {
         counter++;
      }
      return counter;
   }


   /**
    * Returns the number of mines adjacent to a square at the top boundary (minesLoc[0][col]).
    * @param col the column of the target square
    * @return number of mines around the target square
    */
   private int topNum(int col) {
      int counter = 0;
      if (minesLoc[0][col - 1]) {
         counter++;
      }
      if (minesLoc[1][col - 1]) {
         counter++;
      }
      if (minesLoc[1][col]) {
         counter++;
      }
      if (minesLoc[1][col + 1]) {
         counter++;
      }
      if (minesLoc[0][col + 1]) {
         counter++;
      }
      return counter;
   }


   /**
    * Returns the number of mines adjacent to a square at the bottom boundary (minesLoc[numRows() - 1][col]).
    * @param col the column of the target square
    * @return number of mines around the target square
    */
   private int bottomNum(int col) {
      int counter = 0;
      int lastRow = numRows() - 1;
      if (minesLoc[lastRow][col - 1]) {
         counter++;
      }
      if (minesLoc[lastRow - 1][col - 1]) {
         counter++;
      }
      if (minesLoc[lastRow - 1][col]) {
         counter++;
      }
      if (minesLoc[lastRow - 1][col + 1]) {
         counter++;
      }
      if (minesLoc[lastRow][col + 1]) {
         counter++;
      }
      return counter;
   }


   /**
    * Returns the number of mines adjacent to a square at the left boundary (minesLoc[row][0]).
    * @param row the row of the target square
    * @return number of mines around the target square
    */
   private int leftNum(int row) {
      int counter = 0;
      if (minesLoc[row - 1][0]) {
         counter++;
      }
      if (minesLoc[row - 1][1]) {
         counter++;
      }
      if (minesLoc[row][1]) {
         counter++;
      }
      if (minesLoc[row + 1][1]) {
         counter++;
      }
      if (minesLoc[row + 1][0]) {
         counter++;
      }
      return counter;
   }


   /**
    * Returns the number of mines adjacent to a square at the right boundary (minesLoc[row][numCols() - 1]).
    * @param row the row of the target square
    * @return number of mines around the target square
    */
   private int rightNum(int row) {
      int counter = 0;
      int lastCol = numCols() - 1;
      if (minesLoc[row - 1][lastCol]) {
         counter++;
      }
      if (minesLoc[row - 1][lastCol - 1]) {
         counter++;
      }
      if (minesLoc[row][lastCol - 1]) {
         counter++;
      }
      if (minesLoc[row + 1][lastCol - 1]) {
         counter++;
      }
      if (minesLoc[row + 1][lastCol]) {
         counter++;
      }
      return counter;
   }


   /**
    * Returns the number of mines adjacent to a square having eight neighbor squares.
    * @param row the row of the target square
    * @param col the column of the target square
    * @return number of mines around the target square
    */
   private int centralNum(int row, int col) {
      int counter = 0;
      for (int i = row - 1; i <= row + 1; i++) {
         for (int j = col - 1; j <= col + 1; j++) {
            if ((i != row || j != col) && minesLoc[i][j]) {
               counter++;
            }
         }
      }
      return counter;
   }
}

