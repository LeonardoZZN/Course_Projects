// Name: Zining Zhu
// CSCI455 PA2
// Fall 2023

import java.util.Arrays;
import java.util.ArrayList;

/**
 * Class Bookshelf
 * Implements idea of arranging books into a bookshelf.
 * Books on a bookshelf can only be accessed in a specific way so books don’t fall down;
 * You can add or remove a book only when it’s on one of the ends of the shelf.   
 * However, you can look at any book on a shelf by giving its location (starting at 0).
 * Books are identified only by their height; two books of the same height can be
 * thought of as two copies of the same book.
 * @author Zining Zhu
 * @version 1.01 2023-09-10
 */
public class Bookshelf {

   /**
    *  Representation invariant:
    *  The heights of books are positive;
    */

   private ArrayList<Integer> books;

   /**
    * Creates an empty Bookshelf object i.e. with no books
    */
   public Bookshelf() {
      
      books = new ArrayList<Integer>();
      assert isValidBookshelf();  // sample assert statement (you will be adding more of these calls)
   
   }

   /**
    * Creates a Bookshelf with the arrangement specified in pileOfBooks. Example
    * values: [20, 1, 9].
    * 
    * PRE: pileOfBooks contains an array list of 0 or more positive numbers
    * representing the height of each book.
    */
   public Bookshelf(ArrayList<Integer> pileOfBooks) {
       
      books = new ArrayList<Integer>(pileOfBooks);
      assert isValidBookshelf();
      
   }

   /**
    * Inserts book with specified height at the front end of the Bookshelf, i.e., it
    * will end up at position 0.
    * @param height the height of the book to be added on the Bookshelf
    * PRE: height > 0 (height of book is always positive)
    */
   public void addFront(int height) {
      
      assert height > 0;
      books.add(0, height);
      assert isValidBookshelf();
      
   }

   /**
    * Inserts book with specified height from the back end of the Bookshelf.
    * @param height the height of the book to be added on the Bookshelf
    * PRE: height > 0 (height of book is always positive)
    */
   public void addLast(int height) {
      
      assert height > 0;     
      books.add(height);      
      assert isValidBookshelf();
      
   }

   /**
    * Removes book from the front end of the Bookshelf and returns the height of the
    * removed book.
    * @return the height of the book that is removed from the Bookshelf
    * PRE: this.size() > 0 i.e. can be called only on non-empty BookShelf
    */
   public int removeFront() {
      
      assert size() > 0;
      int removedHeight = books.remove(0);
      
      assert isValidBookshelf();
      return removedHeight;
      
   }

   /**
    * Removes book from the back end of the Bookshelf and returns the height of the
    * removed book.
    * @return the height of the book that is removed from the Bookshelf
    * PRE: this.size() > 0 i.e. can be called only on non-empty BookShelf
    */
   public int removeLast() {
      
      assert size() > 0;
      int removedHeight = books.remove(books.size() - 1);  
      
      assert isValidBookshelf();
      return removedHeight;
  
   }

   /*
    * Gets the height of the book at the given position.
    * @param position index of the book being searched for 
    * @return the height of the book in query 
    * PRE: 0 <= position < this.size()
    */
   public int getHeight(int position) {
      
      assert (0 <= position && position < books.size());
      int height = books.get(position);   
      
      assert isValidBookshelf();
      return height;
      
   }

   /**
    * Returns number of books on the this Bookshelf.
    * @return an integer number of books on the Bookshelf
    */
   public int size() {
      
      int num = books.size();
      
      assert isValidBookshelf();
      return num;

   }

   /**
    * Returns string representation of this Bookshelf. Returns a string with the height of all
    * books on the bookshelf, in the order they are in on the bookshelf, using the format shown
    * by example here:  “[7, 33, 5, 4, 3]”
    * @return a String representation of the Bookshelf
    */
   public String toString() {

      String shelf = Arrays.toString(books.toArray());   
      
      assert isValidBookshelf();
      return shelf;

   }

   /**
    * Returns true iff the books on this Bookshelf are in non-decreasing order.
    * (Note: this is an accessor; it does not change the bookshelf.)
    * @return a boolean to represent that the Bookshelf is sorted or not
    */
   public boolean isSorted() {
      
      boolean sortStatus = true;
      
      // Checks the case for books.size() > 0
      int maxHeight = 0; // Initializes a height record 
      int pos = 0;
      while (sortStatus == true && pos < books.size()) {
         int currentHeight = books.get(pos);
         
         if (maxHeight > currentHeight) {
            sortStatus = false;
         } 
         else if (maxHeight < currentHeight) {
            maxHeight = currentHeight;
         }
         
         pos++;
      }
      
      assert isValidBookshelf();
      return sortStatus;  
   }

   /**
    * Returns true iff the Bookshelf data is in a valid state.
    * (See representation invariant comment for more details.)
    * @return a boolean value for the status of the Bookshelf
    */
   private boolean isValidBookshelf() {
      
      boolean validStatus = true;
      
      // Checks that the heights of all books are positive
      int pos = 0;
      while (validStatus == true && pos < books.size()) {
         
         if (books.get(pos) <= 0) {
            validStatus = false;
         }
         
         pos++;
      }
      
      return validStatus;  

   }

}
