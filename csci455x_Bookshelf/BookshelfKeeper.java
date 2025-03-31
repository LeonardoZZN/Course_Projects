// Name: Zining Zhu
// CSCI455 PA2
// Fall 2023


/**
 * Class BookshelfKeeper 
 *
 * Enables users to perform efficient pickPos or putHeight operation on a bookshelf of books kept in 
 * non-decreasing order by height, with the restriction that single books can only be added 
 * or removed from one of the two *ends* of the bookshelf to complete a higher level pick or put 
 * operation.  Pick or put operations are performed with minimum number of such adds or removes.
 * @author Zining Zhu
 * @version 1.01 2023-09-10
 */
public class BookshelfKeeper {

    /**
      Representation invariant:

      1. The heights of books contained on a Bookshelf are in non-decreasing order.
      2. numLastOperation >= 0.
      3. numTotalOperation >= 0.
      4. numLastOperation <= (shelf.size() + 1) after each pickPos or putHeight operation
   */
   
   private Bookshelf shelf;
   private int numTotalOperation; // Total number of Bookshelf mutator calls
   private int numLastOperation;  // Number of mutator calls in the last pickPos or putHeight operation

   /**
    * Creates a BookShelfKeeper object with an empty bookshelf
    */
   public BookshelfKeeper() {
      
      shelf = new Bookshelf();
      numTotalOperation = 0;  
      numLastOperation = 0;   
      assert isValidBookshelfKeeper();
      
   }

   /**
    * Creates a BookshelfKeeper object initialized with the given sorted bookshelf.
    * Note: method does not make a defensive copy of the bookshelf.
    * 
    * PRE: sortedBookshelf.isSorted() is true.
    */
   public BookshelfKeeper(Bookshelf sortedBookshelf) {
      
      assert sortedBookshelf.isSorted();
      
      shelf = sortedBookshelf;
      numTotalOperation = 0;
      numLastOperation = 0;
      assert isValidBookshelfKeeper();
      
   }

   /**
    * Removes a book from the specified position in the bookshelf and keeps bookshelf sorted 
    * after picking up the book.
    * 
    * Returns the number of calls to mutators on the contained bookshelf used to complete this
    * operation. This must be the minimum number to complete the operation.
    * 
    * PRE: 0 <= position < getNumBooks()
    */
   public int pickPos(int position) {
      
      assert (0 <= position && position < getNumBooks());
      
      numLastOperation = 0; // Resets the counter of mutator calls
      
      // Initializes a temporary Bookshelf to hold the unneeded books 
      Bookshelf bookHolder = new Bookshelf();
      
      // Picks from the front end
      if (position + 1 <= shelf.size() / 2.0) { 
         // Removes unneeded books from the front end
         numLastOperation += popFrontBooks(bookHolder, position);
         
         // Removes the target book
         shelf.removeFront();
         numLastOperation++;
         
         // Adds unneeded books back to the front end 
         numLastOperation += pushFrontBooks(bookHolder);
         
      }
      // Picks from the back end
      else { 
         // Removes unneeded books from the back end
         numLastOperation += popLastBooks(bookHolder, position);
         
         // Removes the target book
         shelf.removeLast();
         numLastOperation++;
         
         // Adds unneeded books back to the back end
         numLastOperation += pushLastBooks(bookHolder);
      }
      
      numTotalOperation += numLastOperation;
      
      assert isValidBookshelfKeeper();
      return numLastOperation;   
   }

   /**
    * Inserts book with specified height into the shelf.  Keeps the contained bookshelf sorted 
    * after the insertion.
    * 
    * Returns the number of calls to mutators on the contained bookshelf used to complete this
    * operation. This must be the minimum number to complete the operation.
    * 
    * PRE: height > 0
    */
   public int putHeight(int height) {
      
      assert height > 0;
      
      numLastOperation = 0; // Resets the counter of mutator calls
      
      // Initializes a temporary Bookshelf to hold the unneeded books 
      Bookshelf bookHolder = new Bookshelf();
      
      // Position of the first book with the given height on the Bookshelf
      int firstPos = searchFront(height);
      // Position of the last book with the given height on the Bookshelf
      int lastPos = searchBack(height);
      
      assert firstPos <= lastPos;
      /** 
       * If there is already at least one book with the same height as that of the one to be 
       * inserted, then call the insertPresent method to insert the book.
       */
      if (firstPos != -1 && lastPos != -1) {
         insertPresent(bookHolder, height, firstPos, lastPos);
      }
      // If there is no book with the same height present on the Bookshelf
      else {
         int location = posInsert(height); // Index of the inserted book
         if (location + 1 <= (shelf.size() + 1) / 2.0) {
            numLastOperation += popFrontBooks(bookHolder, location);
            
            shelf.addFront(height);
            numLastOperation++;
            
            numLastOperation += pushFrontBooks(bookHolder);
         } 
         else {
            numLastOperation += popLastBooks(bookHolder, location - 1);
            
            shelf.addLast(height);
            numLastOperation++;
            
            numLastOperation += pushLastBooks(bookHolder);
         }
      }
      
      numTotalOperation += numLastOperation;
      
      assert isValidBookshelfKeeper();
      return numLastOperation;   
   }

   /**
    * Returns the total number of calls made to mutators on the contained bookshelf
    * so far, i.e., all the ones done to perform all of the pick and put operations
    * that have been requested up to now.
    */
   public int getTotalOperations() {
       
      int totalNumCalls = numTotalOperation;
      
      assert isValidBookshelfKeeper();
      return totalNumCalls;   
   }

   /**
    * Returns the number of books on the contained bookshelf.
    */
   public int getNumBooks() {
      
      int numBook = shelf.size();
      assert isValidBookshelfKeeper();
      return numBook;   
   }

   /**
    * Returns string representation of this BookshelfKeeper. Returns a String containing height
    * of all books present in the bookshelf in the order they are on the bookshelf, followed 
    * by the number of bookshelf mutator calls made to perform the last pick or put operation, 
    * followed by the total number of such calls made since we created this BookshelfKeeper.
    * 
    * Example return string showing required format: “[1, 3, 5, 7, 33] 4 10”
    * 
    */
   public String toString() {
      
      String printout = new String(shelf.toString() + " " + numLastOperation + " " + numTotalOperation);
      assert isValidBookshelfKeeper();
      return printout;   
      
   }

   /**
    * Returns true iff the BookshelfKeeper data is in a valid state.
    * (See representation invariant comment for details.)
    */
   private boolean isValidBookshelfKeeper() {
      boolean validStatus = true;
      
      // Heights of books contained on a Bookshelf are in non-decreasing order.
      if (!shelf.isSorted()) {
         validStatus = false;
      }
      
      // numLastOperation >= 0 and numLastOperation <= (shelf.size() + 1) after each pickPos or putHeight operation
      // to ensure minimum number of mutator calls
      if (numLastOperation < 0 || numLastOperation > shelf.size() + 1) {
         validStatus = false;
      }
      
      // numTotalOperation >= 0.
      if (numTotalOperation < 0) {
         validStatus = false;
      }
      
      return validStatus;  

   }

   /**
    * Removes the unneeded/non-target books from the front end of the instance Bookshelf 
    * and temporarily stores them in a local Bookshelf.
    * 
    * @param tempList an empty Bookshelf used to store the removed books
    * @param position the index of the target book
    * @return the number of mutator calls
    *
    * PRE: tempList.size() equals 0;
    */
   private int popFrontBooks(Bookshelf tempList, int position) {
      
      assert tempList.size() == 0;
      
      int callCounter = 0;
      for (int index = 0; index < position; index++) {
         tempList.addLast(shelf.removeFront());
         callCounter++;
      }
      
      assert isValidBookshelfKeeper();
      return callCounter;
      
   }
   
   /**
    * Reads the temporary book holder and adds the unneeded/non-target books back to the front 
    * end of the instance Bookshelf.
    * 
    * @param tempList a local Bookshelf used to store the removed books
    * @return the number of mutator calls
    */
   private int pushFrontBooks(Bookshelf tempList) {
      
      int callCounter = 0;
      int length = tempList.size();
      for (int index = 0; index < length; index++) {
         shelf.addFront(tempList.removeLast());
         callCounter++;
      }
      assert isValidBookshelfKeeper();
      return callCounter;
      
   }
   
   /**
    * Removes the unneeded/non-target books from the back end of the instance Bookshelf 
    * and temporarily stores them in a local Bookshelf.
    * 
    * @param tempList an empty Bookshelf used to store the removed books
    * @param position the index of the target book
    * @return the number of mutator calls
    *
    * PRE: tempList.size() equals 0;
    */
   private int popLastBooks(Bookshelf tempList, int position) {
      
      assert tempList.size() == 0;
      
      int callCounter = 0;
      for (int index = shelf.size() - 1; index > position; index--) {
         tempList.addFront(shelf.removeLast());
         callCounter++;
      }
      assert isValidBookshelfKeeper();
      return callCounter;
      
   }

   /**
    * Reads the temporary book holder and adds the unneeded/non-target books back to the back 
    * end of the instance Bookshelf.
    * 
    * @param tempList a local Bookshelf used to store the removed books
    * @return the number of mutator calls
    */
   private int pushLastBooks(Bookshelf tempList) {
      
      int callCounter = 0;
      int length = tempList.size();
      for (int index = 0; index < length; index++) {
         shelf.addLast(tempList.removeFront());
         callCounter++;
      } 
      assert isValidBookshelfKeeper();
      return callCounter;
      
   }
   
   /**
    * Detects and returns the location of the first book with the given height already on the Bookshelf,
    * searched from the front end.
    *
    * @param height the given height of the inserted book
    * @return the position index of the first book with the given height on the Bookshelf. Returns -1 if not found. 
    */
   private int searchFront(int height) {
      int loc = 0;
      
      while (loc < shelf.size()) {
         if (shelf.getHeight(loc) == height) {
            return loc;
         }
         loc++;
      }
      
      assert isValidBookshelfKeeper();
      return -1;
   }
   
   /**
    * Detects and returns the location of the last book with the given height already on the Bookshelf,
    * searched from the back end.
    *
    * @param height the given height of the inserted book
    * @return the position index of the last book with the given height on the Bookshelf. Returns -1 if not found.
    */
   private int searchBack(int height) {
      int loc = shelf.size() - 1;
      
      while (loc >= 0) {
         if (shelf.getHeight(loc) == height) {
            return loc;
         }
         loc--;
      }
      
      assert isValidBookshelfKeeper();
      return loc;
   }
   
   /**
    * Returns the appropriate position of the book to be inserted with a given height 
    * when the height is not present in the Bookshelf.
    * 
    * @param height the given height of the inserted book
    * @return the proper position of the inserted book
    */
   private int posInsert(int height) {
      int loc = 0;
      
      for (int index = 0; index < shelf.size(); index++) {
         if (shelf.getHeight(index) < height) {
            loc = index + 1;    
         }
      }
      
      assert isValidBookshelfKeeper();
      return loc;
   }
   
   /**
    * Inserts the book with a specified height when there is at least one book with the same height already 
    * present on the Bookshelf
    *
    * @param tempList an empty local Bookshelf used to store the removed books
    * @param height the given height of the inserted book
    * @param firstPos position of the first book with the given height on the instance Bookshelf
    * @param lastPos position of the last book with the given height on the instance Bookshelf
    *
    * PRE: tempList.size() equals 0;
    */
   private void insertPresent(Bookshelf tempList, int height, int firstPos, int lastPos) {
      assert tempList.size() == 0; 
      
      if (firstPos == lastPos) {
         if (firstPos + 1 <= shelf.size() / 2.0) {
            numLastOperation += popFrontBooks(tempList, firstPos);
            
            shelf.addFront(height);
            numLastOperation++;
            
            numLastOperation += pushFrontBooks(tempList);
         }
         else {
            numLastOperation += popLastBooks(tempList, firstPos);
            
            shelf.addLast(height);
            numLastOperation++;
            
            numLastOperation += pushLastBooks(tempList);
         }
      }
      else {
         if (firstPos <= shelf.size() - 1 - lastPos) {            
            numLastOperation += popFrontBooks(tempList, firstPos);
            
            shelf.addFront(height);
            numLastOperation++;
            
            numLastOperation += pushFrontBooks(tempList);
         }
         else {
            numLastOperation += popLastBooks(tempList, lastPos);
            
            shelf.addLast(height);
            numLastOperation++;
            
            numLastOperation += pushLastBooks(tempList);
         }
      }
      
      assert isValidBookshelfKeeper();
   }
   
}
