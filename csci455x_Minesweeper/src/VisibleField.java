// Name: Zining Zhu
// USC NetID: ziningzh
// CS 455 PA3
// Fall 2023

/**
  VisibleField class
  This is the data that's being displayed at any one point in the game (i.e., visible field, because
  it's what the user can see about the minefield). Client can call getStatus(row, col) for any 
  square.  It actually has data about the whole current state of the game, including the underlying
  minefield (getMineField()).  Other accessors related to game status: numMinesLeft(),
  isGameOver().  It also has mutators related to actions the player could do (resetGameDisplay(),
  cycleGuess(), uncover()), and changes the game state accordingly.
  
  It, along with the MineField (accessible in mineField instance variable), forms the Model for the
  game application, whereas GameBoardPanel is the View and Controller in the MVC design pattern.  It
  contains the MineField that it's partially displaying.  That MineField can be accessed
  (or modified) from outside this class via the getMineField accessor.  
 */
public class VisibleField {
   // ----------------------------------------------------------   
   // The following public constants (plus values [0,8] mentioned in comments below) are the
   // possible states of one location (a "square") in the visible field (all are values that can be
   // returned by public method getStatus(row, col)).
   
   // The following are the covered states (all negative values):
   public static final int COVERED = -1;   // initial value of all squares
   public static final int MINE_GUESS = -2;
   public static final int QUESTION = -3;

   // The following are the uncovered states (all non-negative values):
   
   // values in the range [0,8] corresponds to number of mines adjacent to this opened square
   
   public static final int MINE = 9;      // this loc is a mine that hasn't been guessed already
                                          // (end of losing game)
   public static final int INCORRECT_GUESS = 10;  // is displayed a specific way at the end of
                                                  // losing game
   public static final int EXPLODED_MINE = 11;   // the one you uncovered by mistake (that caused
                                                 // you to lose)
   // ----------------------------------------------------------   

   /**
    * Representation invariants:
    * 1. minesField is a rectangular MineField object with minesField.nuwRows() > 0
    *    and minesField.nuwCols() > 0.
    * 2. coverField is a rectangular 2D integer array with coverField.length = minesField.numRows()
    *    and coverField[0].length = minesField.numCols().
    * 3. 0 <= numGuess <= minesField.nuwRows() * minesField.nuwCols().
    * 4. 0 <= numUnopened <= minesField.nuwRows() * minesField.nuwCols() - minesField.numMines().
    * 5. exploded is true iff the status of one element in coverField is set to EXPLODED_MINE;
    *    otherwise, it is always false.
    */
   // <put instance variables here>
   private MineField minesField;
   private int[][] coverField; // The visible field covering the mindField
   private int numGuess; // Number of guesses made
   private int numUnopened; // Number of the non-mine locations to be opened
   private boolean exploded; // Status of all the mines

   /**
      Create a visible field that has the given underlying mineField.
      The initial state will have all the locations covered, no mines guessed, and the game not
      over.
      @param mineField  the minefield to use for this VisibleField
    */
   public VisibleField(MineField mineField) {
      minesField = mineField;
      int rows = minesField.numRows();
      int columns = minesField.numCols();
      // Initializes a visible field
      coverField = new int[rows][columns];
      for (int i = 0; i < rows; i++) {
         for (int j = 0; j < columns; j++) {
            coverField[i][j] = COVERED;
         }
      }
      numGuess = 0;
      numUnopened = rows * columns - minesField.numMines();
      exploded = false;
   }
   
   
   /**
      Reset the object to its initial state (see constructor comments), using the same underlying
      MineField. 
   */     
   public void resetGameDisplay() {
      int rows = minesField.numRows();
      int columns = minesField.numCols();
      for (int i = 0; i < rows; i++) {
         for (int j = 0; j < columns; j++) {
            coverField[i][j] = COVERED;
         }
      }
      numGuess = 0;
      numUnopened = rows * columns - minesField.numMines();
      exploded = false;
   }
  
   
   /**
      Returns a reference to the mineField that this VisibleField "covers"
      @return the minefield
    */
   public MineField getMineField() {
      return minesField;
   }
   
   
   /**
      Returns the visible status of the square indicated.
      @param row  row of the square
      @param col  col of the square
      @return the status of the square at location (row, col).  See the public constants at the
            beginning of the class for the possible values that may be returned, and their meanings.
      PRE: getMineField().inRange(row, col)
    */
   public int getStatus(int row, int col) {
      assert getMineField().inRange(row, col);
      return coverField[row][col];
   }

   
   /**
      Returns the number of mines left to guess.  This has nothing to do with whether the mines
      guessed are correct or not.  Just gives the user an indication of how many more mines the user
      might want to guess.  This value will be negative if they have guessed more than the number of
      mines in the minefield.     
      @return the number of mines left to guess.
    */
   public int numMinesLeft() {
      return minesField.numMines() - numGuess;
   }
 
   
   /**
      Cycles through covered states for a square, updating number of guesses as necessary.  Call on
      a COVERED square changes its status to MINE_GUESS; call on a MINE_GUESS square changes it to
      QUESTION;  call on a QUESTION square changes it to COVERED again; call on an uncovered square
      has no effect.  
      @param row  row of the square
      @param col  col of the square
      PRE: getMineField().inRange(row, col)
    */
   public void cycleGuess(int row, int col) {
      assert getMineField().inRange(row, col);

      int status = coverField[row][col];
      if (status == COVERED) {
         coverField[row][col] = MINE_GUESS;
         numGuess++;
      }
      else if (status == MINE_GUESS) {
         coverField[row][col] = QUESTION;
         numGuess--;
      }
      else if (status == QUESTION) {
         coverField[row][col] = COVERED;
      }
   }

   
   /**
      Uncovers this square and returns false iff you uncover a mine here.
      If the square wasn't a mine or adjacent to a mine it also uncovers all the squares in the
      neighboring area that are also not next to any mines, possibly uncovering a large region.
      Any mine-adjacent squares you reach will also be uncovered, and form (possibly along with
      parts of the edge of the whole field) the boundary of this region.
      Does not uncover, or keep searching through, squares that have the status MINE_GUESS. 
      Note: this action may cause the game to end: either in a win (opened all the non-mine squares)
      or a loss (opened a mine).
      @param row  of the square
      @param col  of the square
      @return false   iff you uncover a mine at (row, col)
      PRE: getMineField().inRange(row, col)
    */
   public boolean uncover(int row, int col) {
      assert getMineField().inRange(row, col);

      if (minesField.hasMine(row, col)) {
         coverField[row][col] = EXPLODED_MINE;
         exploded = true;
         // Updates status of the other locations
         for (int i = 0; i < minesField.numRows(); i++) {
            for (int j = 0; j < minesField.numCols(); j++) {
               int status = coverField[i][j];
               if (minesField.hasMine(i, j)) {
                  if (status == COVERED || status == QUESTION) {
                     coverField[i][j] = MINE;
                  }
               }
               else {
                  if (status == MINE_GUESS) {
                     coverField[i][j] = INCORRECT_GUESS;
                  }
               }
            }
         }
         return false;
      }
      else {
         int lastRow = minesField.numRows() - 1;
         int lastCol = minesField.numCols() - 1;
         floodUncover(row, col, lastRow, lastCol); // Recursively uncover all the adjacent non-mine squares
         return true;
      }
   }
 
   
   /**
      Returns whether the game is over.
      (Note: This is not a mutator.)
      @return whether game has ended
    */
   public boolean isGameOver() {
      return exploded || numUnopened == 0;
   }
 
   
   /**
      Returns whether this square has been uncovered.  (i.e., is in any one of the uncovered states, 
      vs. any one of the covered states).
      @param row of the square
      @param col of the square
      @return whether the square is uncovered
      PRE: getMineField().inRange(row, col)
    */
   public boolean isUncovered(int row, int col) {
      assert getMineField().inRange(row, col);

      int status = coverField[row][col];
      return status != COVERED && status != MINE_GUESS && status != QUESTION;
   }
   
 
   // <put private methods here>

   /**
    * Uncovers the specified square and all the squares that are not mines and adjacent to the specified one
    * through recursively calling this method itself.
    * @param row the row index of the specified square
    * @param col the column index of the specified square
    * @param lastRow the last row index of this visible field
    * @param lastCol the last column index of this visible field
    */
   private void floodUncover(int row, int col, int lastRow, int lastCol) {
      // Base cases
      if (row < 0 || row > lastRow || col < 0 || col > lastCol) {
         return;
      }

      int status = coverField[row][col];
      // Uncovers the specified square and recursively checks the adjacent ones
      if (status == COVERED || status == QUESTION) {
         int numMinesAround = minesField.numAdjacentMines(row, col);
         coverField[row][col] = numMinesAround;
         numUnopened--;
         if (numMinesAround == 0) {
            floodUncover(row, col - 1, lastRow, lastCol); // Checks the left square
            floodUncover(row - 1, col - 1, lastRow, lastCol); // Checks the upper left square
            floodUncover(row - 1, col, lastRow, lastCol); // Checks the upper square
            floodUncover(row - 1, col + 1, lastRow, lastCol); // Checks the upper right square
            floodUncover(row, col + 1, lastRow, lastCol); // Checks the right square
            floodUncover(row + 1, col + 1, lastRow, lastCol); // Checks the lower right square
            floodUncover(row + 1, col, lastRow, lastCol); // Checks the lower square
            floodUncover(row + 1, col - 1, lastRow, lastCol); // Checks the lower left square
         }
      }
   }
}
