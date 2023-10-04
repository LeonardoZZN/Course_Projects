// Name: Zining Zhu
// CS 455 PA1
// Fall 2023


import java.awt.geom.Line2D;
import java.awt.Point;

/**
   class SpiralGenerator
   
   Generates a "rectangular" spiral, using Java display coordinate system, moving 
   outward from the center of the spiral in a counter-clockwise direction.
   That is, it will head rightward on screen, then, upward, then left, then down, etc.
  
   Length of initial line is unitLength.
   Padding between "layers" of the spiral is also unitLength.
   
   NOTE: we have provided the public interface for this class.  Do not change
   the public interface.  You can add private instance variables, constants, 
   and private methods to the class.  You will also be completing the 
   implementation of the methods given. 
   
 */
public class SpiralGenerator {
   // The number of segments in one cycle of the rectangular spiral is 4.
   private static final int SIDES_IN_CYCLE = 4; 
   
   private Point startPoint; // Startpoint of the current segment
   private Point endPoint; // Endpoint of the current segment
   private int inputLength; // Input unit length 
   private int segLength; // Current length of segments
   private int segCounter; // Number of created segments  

   /**
      Creates a SpiralGenerator starting at startPosition, with length of first segnment and 
      distance between "layers" both given as unitLength.  Both values are assuming the Java 
      graphics coordinate system.
      @param startPosition starting point of the first segment in the spiral
      @param unitLength in pixels, must be > 0
   */
   public SpiralGenerator(Point startPosition, int unitLength) {
      startPoint = startPosition;
      endPoint = startPosition;
      inputLength = unitLength;
      segLength = 0;
      segCounter = 0;
   }

   /**
      Return the coordinates of the next line segment in the spiral.
      @return the new segment as a Line2D.Double object
    */
   public Line2D nextSegment() {
      segCounter++;
      
      // Increases the length of segments by the unit length for every half-turn.
      if (segCounter % (SIDES_IN_CYCLE / 2) == 1) {
         segLength += inputLength;
      }
      
      // The next created segment is a horizontal component at the bottom.
      if (segCounter % SIDES_IN_CYCLE == 1) { 
         // A new segment starts at the previous endpoint.
         startPoint = endPoint;  
         // Finds the new endpoint rightward.
         endPoint = new Point((int) Math.round(startPoint.getX() + segLength), (int) Math.round(startPoint.getY()));            
         
         return new Line2D.Double(startPoint, endPoint);          
      } 
      // The next created segment is a vertical component on the right.  
      else if (segCounter % SIDES_IN_CYCLE == 2) {        
         startPoint = endPoint;  
         // Finds the new endpoint upward.
         endPoint = new Point((int) Math.round(startPoint.getX()), (int) Math.round(startPoint.getY() - segLength));          
         
         return new Line2D.Double(startPoint, endPoint);
      }      
      // The next created segment is a horizontal component at the top.
      else if (segCounter % SIDES_IN_CYCLE == 3) {             
         startPoint = endPoint;  
         // Finds the new endpoint leftward.
         endPoint = new Point((int) Math.round(startPoint.getX() - segLength), (int) Math.round(startPoint.getY()));          
         
         return new Line2D.Double(startPoint, endPoint);
      }
      // The next created segment is a vertical component on the left.
      else { 
         startPoint = endPoint;           
         // Finds the new endpoint downward.
         endPoint = new Point((int) Math.round(startPoint.getX()), (int) Math.round(startPoint.getY() + segLength));          
         
         return new Line2D.Double(startPoint, endPoint);
      }  
   }
}
