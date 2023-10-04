// Name: Zining Zhu
// CS 455 PA2
// Fall 2023

/**
   This program is a hard-coded unit test program that tests the Bookshelf class.
   COPYRIGHT (C) 2023 Zining Zhu. All Rights Reserved.
   @author Zining Zhu
   @version 1.01 2023-09-10
*/
import java.util.ArrayList;

/**
   class BookshelfTester
   
   This class tests all of the methods of the Bookshelf class
*/
public class BookshelfTester {

   public static void main(String[] args) {
      
      ex2Test();
      ex3Test();
      ex4Test();

   }
   
   private static void ex2Test() {
      
      // Tests the constructor for an empty Bookshelf and toString method
      Bookshelf emptyShelf = new Bookshelf();
      String expectedResult = "[]";
      System.out.println("Expected result for the no-argument constructor: " + expectedResult);
      System.out.println("Actual output for the no-argument constructor:   " + emptyShelf.toString());
      System.out.println();

      // Tests the constructor and toString method with an input pileOfBooks
      ArrayList<Integer> pileOfBooks = new ArrayList<>();
      pileOfBooks.add(1);
      pileOfBooks.add(4);
      pileOfBooks.add(5);
      pileOfBooks.add(5);
      pileOfBooks.add(5);
      pileOfBooks.add(8);
      pileOfBooks.add(10);
      Bookshelf nonEmptyShelf = new Bookshelf(pileOfBooks);
      expectedResult = "[1, 4, 5, 5, 5, 8, 10]";
      System.out.println("Expected result for the constructor with a pileOfBooks: " + expectedResult);
      System.out.println("Actual output for the constructor with a pileOfBooks:   " + nonEmptyShelf.toString());
      System.out.println();

      pileOfBooks = new ArrayList<>();
      pileOfBooks.add(4);
      pileOfBooks.add(5);
      pileOfBooks.add(6);
      pileOfBooks.add(15);
      pileOfBooks.add(15);
      pileOfBooks.add(15);
      pileOfBooks.add(20);
      nonEmptyShelf = new Bookshelf(pileOfBooks);
      expectedResult = "[4, 5, 6, 15, 15, 15, 20]";
      System.out.println("Expected result for the constructor with a pileOfBooks: " + expectedResult);
      System.out.println("Actual output for the constructor with a pileOfBooks:   " + nonEmptyShelf.toString());
      System.out.println("--------------------------------");
   }
   
   private static void ex3Test() {
      
      // Tests the four public mutator methods: addFront(), addLast(), removeFront(), removeLast()
      ArrayList<Integer> pileOfBooks = new ArrayList<>();
      pileOfBooks.add(4);
      pileOfBooks.add(5);
      pileOfBooks.add(6);
      pileOfBooks.add(15);
      pileOfBooks.add(15);
      pileOfBooks.add(15);
      pileOfBooks.add(20);
      Bookshelf nonEmptyShelf = new Bookshelf(pileOfBooks);
      System.out.println("Tests for the four mutators.");
      nonEmptyShelf.addFront(3);
      System.out.println("Add in the front: " + 3);
      nonEmptyShelf.addLast(21);
      System.out.println("Add at the last: " + 21);
      nonEmptyShelf.addFront(2);
      System.out.println("Add in the front: " + 2);
      nonEmptyShelf.addFront(1);
      System.out.println("Add in the front: " + 1);
      String expectedResult = "[1, 2, 3, 4, 5, 6, 15, 15, 15, 20, 21]";
      System.out.println("Expected result for the constructor with a pileOfBooks: " + expectedResult);
      System.out.println("Actual output for the constructor with a pileOfBooks:   " + nonEmptyShelf.toString());
      System.out.println();
      
      nonEmptyShelf.removeLast();
      System.out.println("Remove the last book");
      nonEmptyShelf.removeFront();
      System.out.println("Remove the first book");
      nonEmptyShelf.removeFront();
      System.out.println("Remove the first book");
      nonEmptyShelf.addFront(1);
      System.out.println("Add in the front: " + 1);
      nonEmptyShelf.removeLast();
      System.out.println("Remove the last book");
      nonEmptyShelf.addLast(15);
      System.out.println("Add at the last: " + 15);
      nonEmptyShelf.removeFront();
      System.out.println("Remove the first book");
      expectedResult = "[3, 4, 5, 6, 15, 15, 15, 15]";
      System.out.println("Expected result for the constructor with a pileOfBooks: " + expectedResult);
      System.out.println("Actual output for the constructor with a pileOfBooks:   " + nonEmptyShelf.toString());
      System.out.println("--------------------------------");
   }
   
   private static void ex4Test() {
      
      // Tests the last three public Bookshelf methods: getHeight(), size(), isSorted()
      ArrayList<Integer> pileOfBooks = new ArrayList<>();
      pileOfBooks.add(4);
      pileOfBooks.add(5);
      pileOfBooks.add(6);
      pileOfBooks.add(15);
      pileOfBooks.add(15);
      pileOfBooks.add(15);
      pileOfBooks.add(20);
      Bookshelf nonEmptyShelf = new Bookshelf(pileOfBooks);
      System.out.println("Tests for the last three methods.");
      System.out.println("The current Bookshelf is " + nonEmptyShelf.toString());
      int num = nonEmptyShelf.size();
      System.out.println("The number of books on the latest Bookshelf is " + num);
      
      int height = nonEmptyShelf.getHeight(0);
      System.out.println("The height of the first book is " + height);
      
      height = nonEmptyShelf.getHeight(num - 1);
      System.out.println("The height of the last book is " + height);
      
      System.out.println("The expected output of the isSorted() for this Bookshelf should be true.");
      if (nonEmptyShelf.isSorted()) {
         System.out.println("The returned boolean is true. The isSorted() method does work!");
      }
      else {
         System.out.println("The returned boolean is false. The isSorted() method does not work.");
      }
      
      System.out.println("Another test below:");
      pileOfBooks = new ArrayList<Integer>();
      pileOfBooks.add(1);
      pileOfBooks.add(2);
      pileOfBooks.add(5);
      pileOfBooks.add(5);
      pileOfBooks.add(5);
      pileOfBooks.add(7);
      pileOfBooks.add(10);
      nonEmptyShelf = new Bookshelf(pileOfBooks);
      System.out.println("The current Bookshelf is " + nonEmptyShelf.toString());
      num = nonEmptyShelf.size();
      System.out.println("The number of books on the latest Bookshelf is " + num);
      
      height = nonEmptyShelf.getHeight(0);
      System.out.println("The height of the first book is " + height);
      
      height = nonEmptyShelf.getHeight(num - 1);
      System.out.println("The height of the last book is " + height);
      
      System.out.println("The expected output of the isSorted() for this Bookshelf should be true.");
      if (nonEmptyShelf.isSorted()) {
         System.out.println("The returned boolean is true. The isSorted() method does work!");
      }
      else {
         System.out.println("The returned boolean is false. The isSorted() method does not work.");
      }
   }
   
}
