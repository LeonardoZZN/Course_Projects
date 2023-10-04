// Name: Zining Zhu
// USC NetID: ziningzh
// CSCI455 PA2
// Fall 2023

/**
 * This program runs the BookshelfKeeperProg class to allow clients to create a Bookshelf object with
 * any number of books of positive heights and maintain the books in a non-decreasing order of heights
 * through the BookshelfKeeper class. It also allows clients to pick a book from the bookshelf or put a 
 * book onto the shelf while maintaining its correct order of heights to avoid books falling down.
 */

import java.util.Scanner;
import java.util.ArrayList;

/**
 * Class BookshelfKeeperProg
 *
 * Enables users to set up a Bookshelf object representing a series of books of positive heights that is  
 * maintained in a BookshelfKeeper object and then perform a series of pickPos or putHeight operations 
 * through the BookshelfKeeper class in an interactive mode with user commands called pick and put.
 * It can also be run in a batch mode by using input and output redirection.
 * 
 */
public class BookshelfKeeperProg {
   
   public static void main(String[] args) {
      
      Scanner in = new Scanner(System.in);
      System.out.println("Please enter initial arrangement of books followed by newline:");
      
      // Gets the initial configuration of a bookshelf
      ArrayList<Integer> bookList = iniConfig(in);
      
      // Initializes a Bookshelf and a BookshelfKeeper with input books
      // Checks that all heights > 0
      if (!checkHeight(bookList)) {
         wrongHeight();
         return;
      }
 
      Bookshelf inputBooks = new Bookshelf(new ArrayList<Integer>(bookList));
      BookshelfKeeper keeper = new BookshelfKeeper(); 
      if (inputBooks.size() != 0 && inputBooks.isSorted()) {
         keeper = new BookshelfKeeper(inputBooks);
      }
      // Warning of decreasing order of heights
      else if (!inputBooks.isSorted()) {
         wrongOrder();
         return;
      }
      System.out.println(keeper.toString());
      
      // Operations of the bookshelf keeper
      System.out.println("Type pick <index> or put <height> followed by newline. Type end to exit.");
      operation(in, keeper); 
      
   }
   
   
   /**
    * Checks if the heights of all books are positive
    *
    * @param bookList a list of books to be checked
    * @return a boolean representing the result of height check
    */
   private static boolean checkHeight(ArrayList<Integer> bookList) {
      
      boolean allPositive = true;
      
      int index = 0;
      while (allPositive == true && index < bookList.size()) {         
         if (bookList.get(index) <= 0) {
            allPositive = false;
         }        
         index++;
      }
      
      return allPositive;  

   } 
   
   /**
    * Creates the initial configuration of a bookshelf based on external input
    *
    * @param in an Scanner object to read the input from external sources
    * @return a boolean representing the result of height check
    */
   private static ArrayList<Integer> iniConfig(Scanner in) {
      
      Scanner inLocal = new Scanner(in.nextLine());
      inLocal.useDelimiter("\\s+");
      ArrayList<Integer> list = new ArrayList<>();
      while (inLocal.hasNextInt()) {
         list.add(inLocal.nextInt());      
      }
      
      return list;
   }
   
   /**
    * Operates the bookshelf keeper based on external commands
    *
    * @param in an Scanner object to read the input from external sources
    * @param keeper the bookshelf keeper that executes commands.
    */
   private static void operation(Scanner in, BookshelfKeeper keeper) {
      
      while (in.hasNextLine()) {
         Scanner inLocal =  new Scanner(in.nextLine());
         String command = inLocal.next();
         if (command.equals("end")) {
            System.out.println("Exiting Program.");
            return;
         }
         else if (command.equals("put")) {
            int height = inLocal.nextInt();
            if (height > 0) {
               keeper.putHeight(height);
               System.out.println(keeper.toString());
            } 
            // Warning of non-positive height
            else {
               wrongHeight();
               return; 
            }
         }
         else if (command.equals("pick")) {
            int index = inLocal.nextInt();
            if (index >= 0 && index < keeper.getNumBooks()) {
               keeper.pickPos(index);
               System.out.println(keeper.toString());
            }
            // Warning of an invalid pick position
            else {
               wrongPos();
               return;               
            }
         }
         // Warning of invalid commands
         else {
            wrongCommand();
            return;
         }      
      }     
   }
   
   /**
    * Prints out an error message for non-positive heights of books
    */
   private static void wrongHeight() {
      System.out.println("ERROR: Height of a book must be positive.");
      System.out.println("Exiting Program.");
   }
   
   /**
    * Prints out an error message for decreasing order of heights 
    */
   private static void wrongOrder() {
      System.out.println("ERROR: Heights must be specified in non-decreasing order.");
      System.out.println("Exiting Program.");
   }
   
   /**
    * Prints out an error message for invalid commands
    */
   private static void wrongCommand() {
      System.out.println("ERROR: Invalid command. Valid commands are pick, put, or end.");
      System.out.println("Exiting Program.");
   }
   
   /**
    * Prints out an error message for an invalid position where the client tries to pick a book
    */
   private static void wrongPos() {
       System.out.println("ERROR: Entered pick operation is invalid on this shelf.");
       System.out.println("Exiting Program.");
   }
   
}