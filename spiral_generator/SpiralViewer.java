// Name: Zining Zhu
// USC NetID: ziningzh
// CS 455 PA1
// Fall 2023

/**
   This program creates a rectangular spiral shape in a GUI baed on the Java display coordinate system.
   It prompts users for the length of the first segment that is also the padding distance between "layers"
   and for the number of segments, both of which are the parameters determining the size of the created spiral.
   COPYRIGHT (C) 2023 Zining Zhu. All Rights Reserved.
   @author Zining Zhu
   @version 1.01 2023-09-10
*/

import javax.swing.JFrame;
import java.util.Scanner;

/**
   class SpiralViewer
   
   This class prompts for the initial segment length and the total number of segments of a rectangular spiral. 
   Then it visualizes the spiral shape by creating a JFrame object that contains the SpiralComponent.
   
   The frame size is defined by constants FRAME_WIDTH and FRAME_HEIGHT.
*/
public class SpiralViewer {
   private static final int FRAME_WIDTH = 800; 
   private static final int FRAME_HEIGHT = 500; 
      
   public static void main(String[] args) {
      JFrame frame = new JFrame();
      
      // Sets up the frame window
      frame.setSize(FRAME_WIDTH, FRAME_HEIGHT);
      frame.setTitle("Spiral");
      frame.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
      
      Scanner in = new Scanner(System.in);
      
      // Prompts for the length of the initial segment
      System.out.print("Enter length of initial segment: ");
      int initialLength = Integer.parseInt(in.nextLine());
      // Error check
      while (initialLength <= 0) {
         System.out.println("ERROR: value must be > 0");
         System.out.print("Enter length of initial segment: ");
         initialLength = Integer.parseInt(in.nextLine());
      }
      
      // Prompts for the number of segments to display
      System.out.print("Enter number of segments to display: ");
      int numSeg = Integer.parseInt(in.nextLine());
      // Error check
      while (numSeg <= 0) {
         System.out.println("ERROR: value must be > 0");
         System.out.print("Enter number of segments to display: ");
         numSeg = Integer.parseInt(in.nextLine());
      }
      
      SpiralComponent component = new SpiralComponent(initialLength, numSeg);
      frame.add(component);
      
      frame.setVisible(true);
   }
}
   