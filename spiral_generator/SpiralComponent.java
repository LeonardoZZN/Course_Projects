// Name: Zining Zhu
// CS 455 PA1
// Fall 2023

import java.awt.Graphics;
import java.awt.Graphics2D;
import javax.swing.JComponent;
import java.awt.Point;

/**
   class SpiralComponent
   
   This component entending JComponent draws a rectangular spiral shape 
   based on the Java display coordinate system.
   
   The size of the spiral depends on the length of the initial line (unitLength) 
   and the number of segments drawn in total (totalNumberSeg), 
   which are two instance variables declared through a constructor.
   
   The instance method paintComponent draws the spiral by continuously invoking 
   a segment-generating method on a "SpiralGenerator" object. 
*/
public class SpiralComponent extends JComponent {
   private int unitLength; 
   private int totalNumberSeg; // Total number of segments needed to be drawn
   
   /**
      Constructs a SpiralComponent with an initialLength referring to the unit length of the spiral, 
      and numSeg that gives the total number of segments to form the spiral. 
      @param initialLength the length of the first segment and also the distance between "layers", must be > 0
      @param numSeg the number of segments which are to be drawn counterclockwise, must be > 0
   */
   public SpiralComponent(int initialLength, int numSeg) {
      unitLength = initialLength;
      totalNumberSeg = numSeg;
   }
   
   /** 
      Draws a rectangular spiral with an argument g of type Graphics.
      @param g the Graphics object stores the graphics state used for drawing operations
   */
   public void paintComponent(Graphics g) {
      Graphics2D g2 = (Graphics2D) g;
      
      // Gets coordinates of the initial point centered in the window based on the width and height of the frame 
      Point initialPoint = new Point(getWidth() / 2, getHeight() / 2);
      
      SpiralGenerator spiral = new SpiralGenerator(initialPoint, unitLength); 
      
      // Draws the spiral by sequentially drawing connected segments
      for (int segCounter = 1; segCounter <= totalNumberSeg; segCounter++) {
         g2.draw(spiral.nextSegment());
      }
   }
}
