// Name: Zining Zhu
// CS 455 PA1
// Fall 2023

/**
   This program is a console-based unit test program that tests the SpiralGenerator class.
   It checks a few general properties of a rectangular spiral for the created shape.
   COPYRIGHT (C) 2023 Zining Zhu. All Rights Reserved.
   @author Zining Zhu
   @version 1.01 2023-09-10
*/
import java.awt.Point;
import java.awt.geom.Line2D;

/**
   class SpiralGeneratorTester
   
   This class tests the SpiralGenerator class multiple times with different parameters,
   including the position of the center, the unit length, and the number of segments. 
   Then it prints out the setup information and the test output for each segment.
   
   This class uses the private instance method tester to check if each segment is 
   either horizontal or vertical and if each pair of segments are connected and perpendicular.
*/
public class SpiralGeneratorTester {
   private static final double EPSILON = 1E-14; // For floating-point numbers comparison
      
   public static void main(String[] args) {
      // Test case 1: outermost segment is inside frame boundary
      int initialX = 200;
      int initialY = 300; 
      int unitLength = 5;
      int numSeg = 10;
      tester(initialX, initialY, unitLength, numSeg);
      
      // Test case 2: outermost segment is at frame boundary
      initialX = 400;
      initialY = 250; 
      unitLength = 20;
      numSeg = 50;
      tester(initialX, initialY, unitLength, numSeg);
      
      // Test case 3: outermost segment is outside frame boundary
      initialX = 600;
      initialY = 300; 
      unitLength = 50;
      numSeg = 20;
      tester(initialX, initialY, unitLength, numSeg);
   }
   
   /**
      Prints out setup information about the initialX, initialY, unitLength, and numSeg,
      and then tests whether the general requirements of a rectangular spiral are met 
      by the object created by SpiralGenerator.
      @param initialX the x-coordinate of the center of spiral 
      @param initialY the y-coordinate of the center of spiral 
      @param unitLength the length of the first segment and also the distance between "layers", must be > 0
      @param numSeg the number of segments, must be > 0
   */
   private static void tester(int initialX, int initialY, int unitLength, int numSeg) {
      Point initialPoint = new Point(initialX, initialY);
      SpiralGenerator spiral = new SpiralGenerator(initialPoint, unitLength);
      
      System.out.println("Making a spiral starting from java.awt.Point" + initialPoint);
      System.out.println("with a unit length of " + unitLength + ", and made up of " + numSeg + "segments:");
      
      // Initializes the holder of last segments for comparison before the first segment is generated
      Line2D lastSeg = null;
      
      // Runs the tests for all created segments one by one
      for (int segCounter = 1; segCounter <= numSeg; segCounter++) {
         Line2D currentSeg = spiral.nextSegment();
         System.out.println("Segment #" + segCounter + ": " + currentSeg.getP1() + " " + currentSeg.getP2());
         
         // Test if the segment is horizontal or vertical in general
         if (Math.abs(currentSeg.getX1() - currentSeg.getX2()) > EPSILON 
             && Math.abs(currentSeg.getY1() - currentSeg.getY2()) > EPSILON) {
            System.out.println("FAILED: segment is not horizontal or vertical.  Skipping pair tests.");
         }
         // Pair tests
         else if (lastSeg != null) {
            // Test for connected endpoints
            if (!currentSeg.getP1().equals(lastSeg.getP1()) && !currentSeg.getP1().equals(lastSeg.getP2())
                && !currentSeg.getP2().equals(lastSeg.getP1()) && !currentSeg.getP2().equals(lastSeg.getP2())) {
               System.out.println("FAILED: last two segments are not connected.");
            }
            // Test for parallel cases
            if (((Math.abs(currentSeg.getX1()-currentSeg.getX2()) <= EPSILON) // two parallel 
                && (Math.abs(lastSeg.getX1()-lastSeg.getX2()) <= EPSILON))    // vertical lines
                ||((Math.abs(currentSeg.getY1()-currentSeg.getY2()) <= EPSILON) // two parallel 
                && (Math.abs(lastSeg.getY1()-lastSeg.getY2()) <= EPSILON))) {   // horizontal lines
               System.out.println("FAILED: last two segments are not perpendicular.");
            }
         }
         
         // Stores the current segment for the later use
         lastSeg = currentSeg;
      } 
   }   
}
