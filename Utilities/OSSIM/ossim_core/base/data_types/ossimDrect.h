//*******************************************************************
// Copyright (C) 2000 ImageLinks Inc.
//
// License:  LGPL
// 
// See LICENSE.txt file in the top level directory for more details.
//
// Author:  David Burken
//
// Description:
//
// Contains class declaration for drect.
// Container class for four double points representing a rectangle.
// 
//*******************************************************************
//  $Id: ossimDrect.h,v 1.23 2005/07/15 18:51:25 dburken Exp $

#ifndef ossimDrect_HEADER
#define ossimDrect_HEADER
#include <vector>
using namespace std;

#include <base/data_types/ossimDpt.h>
#include <base/data_types/ossimIpt.h>
#include <base/data_types/ossimFpt.h>

//***
// NOTE:  A word on corner points...
//
// There is the concept of "pixel is area" and "pixel is point".
// - Pixel is area means the (x,y) pixel coordinate refers to the upper left
//   corner of the pixel, NOT the center of the pixel.
// - Pixel is point means the (x,y) pixel coordinate refers to the center
//   of the pixel, NOT the upper left corner.
//
// For the uniformity purposes, all pixel points  should be in the
// "pixel is point" form; therefore, the (x,y) point should represent the
// CENTER of the pixel.
//***

//***
// Forward class declarations.
//***
class OSSIMDLLEXPORT ossimIrect;
class OSSIMDLLEXPORT ossimPolygon;

//*******************************************************************
// CLASS:  ossimDrect
//*******************************************************************

class OSSIMDLLEXPORT ossimDrect
{
public:
   enum
   {
      UPPER_LEFT  = 1,
      LOWER_LEFT  = 2,
      LOWER_RIGHT = 4,
      UPPER_RIGHT = 8
   };

   ossimDrect()
      :
         theUlCorner(0.0, 0.0),
         theUrCorner(0.0, 0.0),
         theLrCorner(0.0, 0.0),
         theLlCorner(0.0, 0.0),
         theOrientMode(OSSIM_LEFT_HANDED)
      {}

   ossimDrect(const ossimDpt&   ul_corner,
              const ossimDpt&   lr_corner,
              ossimCoordSysOrientMode mode=OSSIM_LEFT_HANDED)
      :
         theUlCorner(ul_corner),
         theUrCorner(lr_corner.x, ul_corner.y),
         theLrCorner(lr_corner),
         theLlCorner(ul_corner.x, lr_corner.y),
         theOrientMode(mode)
      {
      }

   ossimDrect(const double& ul_corner_x,
              const double& ul_corner_y,
              const double& lr_corner_x,
              const double& lr_corner_y,
              ossimCoordSysOrientMode mode=OSSIM_LEFT_HANDED)
      :
         theUlCorner(ul_corner_x, ul_corner_y),
         theUrCorner(lr_corner_x, ul_corner_y),
         theLrCorner(lr_corner_x, lr_corner_y),
         theLlCorner(ul_corner_x, lr_corner_y),
         theOrientMode(mode)
      {}
   
   ossimDrect(const ossimDrect& rect)
      :
         theUlCorner(rect.ul()),
         theUrCorner(rect.ur()),
         theLrCorner(rect.lr()),
         theLlCorner(rect.ll()),
         theOrientMode(rect.orientMode())
      {}

   ossimDrect(const ossimIrect& rect);

   /*!
    * Must compute a bounding rect given a collection of points or polygon:
    */
   ossimDrect(const vector<ossimDpt>& points,
              ossimCoordSysOrientMode mode=OSSIM_LEFT_HANDED);
   ossimDrect(const ossimDpt& p1,
              const ossimDpt& p2,
              const ossimDpt& p3,
              const ossimDpt& p4,
              ossimCoordSysOrientMode mode=OSSIM_LEFT_HANDED);
   ossimDrect(const ossimPolygon& polygon, 
              ossimCoordSysOrientMode mode=OSSIM_LEFT_HANDED);

   /** destructor */
   ~ossimDrect();
   
   const ossimDrect& operator=  (const ossimDrect& rect);
   const ossimDrect& operator=  (const ossimIrect& rect);
   bool         operator!= (const ossimDrect& rect) const;
   bool         operator== (const ossimDrect& rect) const;
    friend ossimDrect operator*(double scalar, const ossimDrect& rect)
      {
         ossimDpt ul(rect.theUlCorner.x*scalar,
                     rect.theUlCorner.y*scalar);

         if(rect.theOrientMode == OSSIM_LEFT_HANDED)
         {
            return ossimDrect(ul.x,
                              ul.y,
                              ul.x+rect.width()*scalar-1,
                              ul.y+rect.height()*scalar-1,
                              rect.theOrientMode);
         }
         return ossimDrect(ul.x,
                           ul.y,
                           ul.x+rect.width()*scalar-1,
                           ul.y-(rect.height()*scalar-1),
                           rect.theOrientMode);
         
      }
   
   const ossimDrect& operator *=(double scalar)
      {
          ossimDpt ul(theUlCorner.x*scalar,
                      theUlCorner.y*scalar);

          if(theOrientMode == OSSIM_LEFT_HANDED)
          {
             *this = ossimDrect(ul.x,
                                ul.y,
                                (ul.x+width()*scalar-1),
                                (ul.y+height()*scalar-1),
                                theOrientMode);
          }
          else
          {
             *this = ossimDrect(ul.x,
                                ul.y,
                                (ul.x+width()*scalar-1),
                                (ul.y-(height()*scalar-1)),
                                theOrientMode);
                                
          }
         return *this;
      }
   
   ossimDrect operator *(double scalar)const
      {
         ossimDpt ul((theUlCorner.x*scalar),
                     (theUlCorner.y*scalar));

         if(theOrientMode == OSSIM_LEFT_HANDED)
         {
            return ossimDrect(ul.x,
                              ul.y,
                              (ul.x+width()*scalar-1),
                              (ul.y+height()*scalar-1),
                              theOrientMode);
         }

         return ossimDrect(ul.x,
                           ul.y,
                           (ul.x+width()*scalar-1),
                           ul.y-(height()*scalar-1),
                           theOrientMode);
      }
   const ossimDrect& operator *=(const ossimDpt& scalar)
      {
         ossimDpt ul((theUlCorner.x*scalar.x),
                     (theUlCorner.y*scalar.y));
         
         if(theOrientMode == OSSIM_LEFT_HANDED)
         {
            *this = ossimDrect(ul.x,
                               ul.y,
                               (ul.x+width()*scalar.x - 1),
                               (ul.y+height()*scalar.y - 1),
                               theOrientMode);
         }
         else
         {
            *this = ossimDrect(ul.x,
                               ul.y,
                               (ul.x+width()*scalar.x - 1),
                               (ul.y-(height()*scalar.y - 1)),
                               theOrientMode);            
         }
         return *this;
      }
   
   ossimDrect operator *(const ossimDpt& scalar)const
      {
         ossimDpt ul((theUlCorner.x*scalar.x),
                     (theUlCorner.y*scalar.y));
         
         if(theOrientMode == OSSIM_LEFT_HANDED)
         {
            return ossimDrect(ul.x,
                              ul.y,
                              (ul.x+width()*scalar.x-1),
                              (ul.y+height()*scalar.y-1),
                              theOrientMode);
         }
         return ossimDrect(ul.x,
                           ul.y,
                           (ul.x+width()*scalar.x-1),
                           (ul.y-(height()*scalar.y-1)),
                           theOrientMode);
      }
   const ossimDrect& operator +=(const ossimDpt& shift)
      {
         ossimDpt ul((theUlCorner.x+shift.x),
                     (theUlCorner.y+shift.y));
                     
         if(theOrientMode == OSSIM_LEFT_HANDED)
         {
            *this = ossimDrect(ul.x,
                               ul.y,
                               ul.x+width()-1,
                               ul.y+height()-1,
                               theOrientMode);
         }
         else
         {
            *this = ossimDrect(ul.x,
                               ul.y,
                               ul.x+width()-1,
                               ul.y-(height()-1),
                               theOrientMode);            
         }
         return *this;
      }
   
   const ossimDrect& operator -=(const ossimDpt& shift)
      {
         ossimDpt ul((theUlCorner.x-shift.x),
                     (theUlCorner.y-shift.y));
         
         if(theOrientMode == OSSIM_LEFT_HANDED)
         {
            *this = ossimDrect(ul.x,
                               ul.y,
                               ul.x+width()-1,
                               ul.y+height()-1,
                               theOrientMode);
         }
         else
         {
            *this = ossimDrect(ul.x,
                               ul.y,
                               ul.x+width()-1,
                               ul.y-(height()-1),
                               theOrientMode);
         }
         return *this;
      }
   
   ossimDrect operator +(const ossimDpt& shift)const
      {
         ossimDpt ul((theUlCorner.x+shift.x),
                     (theUlCorner.y+shift.y));
                     
         if(theOrientMode == OSSIM_LEFT_HANDED)
         {
            return ossimDrect(ul.x,
                              ul.y,
                              ul.x+width()-1,
                              ul.y+height()-1,
                              theOrientMode);
         }
         else
         {
            return ossimDrect(ul.x,
                              ul.y,
                              ul.x+width()-1,
                              ul.y-(height()-1),
                              theOrientMode);
         }
      }  

   ossimDrect operator -(const ossimDpt& shift)const
      {
         ossimIpt ul(irint(theUlCorner.x-shift.x),
                     irint(theUlCorner.y-shift.y));
                     
         if(theOrientMode == OSSIM_LEFT_HANDED)
         {
            return ossimDrect(ul.x,
                              ul.y,
                              ul.x+width()-1,
                              ul.y+height()-1,
                              theOrientMode);
         }
         else
         {
            return ossimDrect(ul.x,
                              ul.y,
                              ul.x+width()-1,
                              ul.y-(height()-1),
                              theOrientMode);            
         }
      }  

   ossimDpt ul() const { return theUlCorner; }
   ossimDpt ur() const { return theUrCorner; }
   ossimDpt lr() const { return theLrCorner; }
   ossimDpt ll() const { return theLlCorner; }

   void getBounds(double& minx, double& miny,
                  double& maxx, double& maxy)const
      {
         minx = theUlCorner.x;
         maxx = theUrCorner.x;
         if(theOrientMode == OSSIM_LEFT_HANDED)
         {
            miny = theUlCorner.y;
            maxy = theUrCorner.y;
         }
         else
         {
            maxy = theUlCorner.y;
            miny = theUrCorner.y;
         }
      }
   void makeNan()
      {
         theUlCorner.makeNan();
         theLlCorner.makeNan();
         theLrCorner.makeNan();
         theUrCorner.makeNan();
      }
   
   bool hasNans()const{ return (theUlCorner.hasNans() ||
                                theLlCorner.hasNans() ||
                                theLrCorner.hasNans() ||
                                theUrCorner.hasNans());}
   
   bool isNan()const{ return (theUlCorner.hasNans() &&
                              theLlCorner.hasNans() &&
                              theLrCorner.hasNans() &&
                              theUrCorner.hasNans());}

   double area()const
      {
         return width()*height();
      }
   //***
   // This class supports both left and right-handed coordinate systems. For
   // both, the positive x-axis extends to the "right".
   //***
   ossimCoordSysOrientMode orientMode() const { return theOrientMode; }
   void setOrientMode(ossimCoordSysOrientMode mode) { theOrientMode = mode; }
   
   /*!
    * Sets the upper left corner to "pt".  Adjusts the remaining corners
    * accordingly.
    */
   void set_ul(const ossimDpt& pt);

   /*!
    * Sets the upper right corner to "pt".  Adjusts the remaining corners
    * accordingly.
    */
   void set_ur(const ossimDpt& pt);

   /*!
    * Sets the lower right corner to "pt".  Adjusts the remaining corners
    * accordingly.
    */
   void set_lr(const ossimDpt& pt);

   /*!
    * Sets the lower left corner to "pt".  Adjusts the remaining corners
    * accordingly.
    */
   void set_ll(const ossimDpt& pt);

   /*!
    * Sets the upper left x.  Adjusts the remaining corners accordingly.
    */
   void set_ulx(ossim_float64 x);

   /*!
    * Sets the upper left y.  Adjusts the remaining corners accordingly.
    */
   void set_uly(ossim_float64 y);

   /*!
    * Sets the upper right x.  Adjusts the remaining corners accordingly.
    */
   void set_urx(ossim_float64 x);

   /*!
    * Sets the upper right y.  Adjusts the remaining corners accordingly.
    */
   void set_ury(ossim_float64 y);

   /*!
    * Sets the lower right x.  Adjusts the remaining corners accordingly.
    */
   void set_lrx(ossim_float64 x);

   /*!
    * Sets the lower right y.  Adjusts the remaining corners accordingly.
    */
   void set_lry(ossim_float64 y);

   /*!
    * Sets the lower left x.  Adjusts the remaining corners accordingly.
    */
   void set_llx(ossim_float64 x);

   /*!
    * Sets the lower left y.  Adjusts the remaining corners accordingly.
    */
   void set_lly(ossim_float64 y);

   /*!
    * METHOD: initBoundingRect(points)
    * Initializes this rect to the bounding rect containing all points in the
    * collection passed in.
    */
   void initBoundingRect(const vector<ossimDpt>& points);

   /*!
    * Returns true if "pt" falls within rectangle.  Fall on an edge is also
    * considered to be within.
    */
   bool pointWithin(const ossimDpt& pt) const;

   /*!
    * Returns true if "pt" falls within rectangle.  Fall on an edge is also
    * considered to be within.
    */
   bool pointWithin(const ossimFpt& pt) const;

   /*!
    * Returns true if any portion of an input rectangle "rect" intersects
    * "this" rectangle.  
    */
   bool intersects(const ossimDrect& rect) const;

   /*!
    * Returns true if "this" rectangle is contained completely within the
    * input rectangle "rect".
    */
   bool completely_within(const ossimDrect& rect) const;

   /*!
    * Returns the height of a rectangle.
    */
   ossim_float64 height() const { return fabs(theLlCorner.y - theUlCorner.y) + 1.0; }

   /*!
    * Returns the width of a rectangle.
    */
   ossim_float64 width()  const { return fabs(theLrCorner.x - theLlCorner.x) + 1.0; }

   /*!
    * Stretches this rectangle out to integer boundaries.
    */
   void stretchOut();

   /*!
    * Will stretch the rect to the passed in tiled boundary.
    */
   void stretchToTileBoundary(const ossimDpt& widthHeight);

   /*!
    * Will subdivide this rect into four partiions.
    */
   void splitToQuad(ossimDrect& ulRect,
                    ossimDrect& urRect,
                    ossimDrect& lrRect,
                    ossimDrect& llRect);
   
   ossimDrect clipToRect(const ossimDrect& rect)const;

   inline ossimDpt midPoint()const;

   void print(ostream& os) const;

   ossimDrect combine(const ossimDrect& rect)const;
   
   friend OSSIMDLLEXPORT ostream& operator<<(ostream& os,
                                             const ossimDrect& rect);

   bool clip(ossimDpt &p1,
             ossimDpt  &p2)const;

   static long getCode(const ossimDpt& aPoint,
                       const ossimDrect& clipRect);
private:

   enum ossimCohenSutherlandClipCodes
   {
      NONE   = 0,
      LEFT   = 1,
      RIGHT  = 2,
      BOTTOM = 4,
      TOP    = 8
   };
   
   //***
   // Private data members representing the rectangle corners.
   //***
   ossimDpt theUlCorner;
   ossimDpt theUrCorner;
   ossimDpt theLrCorner;
   ossimDpt theLlCorner;

   ossimCoordSysOrientMode  theOrientMode;
};

//*******************************************************************
// Inline Method: ossimDrect::operator=(ossimDrect)
//*******************************************************************
inline const ossimDrect& ossimDrect::operator=(const ossimDrect& rect)
{
   if (this != &rect)
   {
      theUlCorner   = rect.theUlCorner;
      theUrCorner   = rect.theUrCorner;
      theLrCorner   = rect.theLrCorner;
      theLlCorner   = rect.theLlCorner;
      theOrientMode = rect.theOrientMode;
   }

   return *this;
}

//*******************************************************************
// Inline Method: ossimDrect::operator!=
//*******************************************************************
inline bool ossimDrect::operator!=(const ossimDrect& rect) const
{
   return ( (theUlCorner   != rect.theUlCorner) ||
            (theUrCorner   != rect.theUrCorner) ||
            (theLrCorner   != rect.theLrCorner) ||
            (theLlCorner   != rect.theLlCorner) ||
            (theOrientMode != rect.theOrientMode));
}

//*******************************************************************
// Inline Method: ossimDrect::operator==
//*******************************************************************
inline bool ossimDrect::operator==(const ossimDrect& rect) const
{
   return ( (theUlCorner   == rect.theUlCorner) &&
            (theUrCorner   == rect.theUrCorner) &&
            (theLrCorner   == rect.theLrCorner) &&
            (theLlCorner   == rect.theLlCorner) &&
            (theOrientMode == rect.theOrientMode));
}

//*******************************************************************
// Inline Method: ossimDrect::set_ul
//*******************************************************************
inline void ossimDrect::set_ul(const ossimDpt& pt)
{
   theUlCorner   = pt;
   theUrCorner.y = pt.y;
   theLlCorner.x = pt.x;
}

//*******************************************************************
// Inline Method: ossimDrect::set_ur
//*******************************************************************
inline void ossimDrect::set_ur(const ossimDpt& pt)
{
   theUrCorner   = pt;
   theUlCorner.y = pt.y;
   theLrCorner.x = pt.x;
}

//*******************************************************************
// Inline Method: ossimDrect::set_lr
//*******************************************************************
inline void ossimDrect::set_lr(const ossimDpt& pt)
{
   theLrCorner   = pt;
   theUrCorner.x = pt.x;
   theLlCorner.y = pt.y;
}

//*******************************************************************
// Inline Method: ossimDrect::set_ll
//*******************************************************************
inline void ossimDrect::set_ll(const ossimDpt& pt)
{
   theLlCorner   = pt;
   theUlCorner.x = pt.x;
   theLrCorner.y = pt.y;
}

//*******************************************************************
// Inline Method: ossimDrect::set_ulx
//*******************************************************************
inline void ossimDrect::set_ulx(ossim_float64 x)
{
   theUlCorner.x = x;
   theLlCorner.x = x;
}

//*******************************************************************
// Inline Method: ossimDrect::set_uly
//*******************************************************************
inline void ossimDrect::set_uly(ossim_float64 y)
{
   theUlCorner.y = y;
   theUrCorner.y = y;
}

//*******************************************************************
// Inline Method: ossimDrect::set_urx
//*******************************************************************
inline void ossimDrect::set_urx(ossim_float64 x)
{
   theUrCorner.x = x;
   theLrCorner.x = x;
}

//*******************************************************************
// Inline Method: ossimDrect::set_ury
//*******************************************************************
inline void ossimDrect::set_ury(ossim_float64 y)
{
   theUrCorner.y = y;
   theUlCorner.y = y;
}

//*******************************************************************
// Inline Method: ossimDrect::set_lrx
//*******************************************************************
inline void ossimDrect::set_lrx(ossim_float64 x)
{
   theLrCorner.x = x;
   theUrCorner.x = x;
}

//*******************************************************************
// Inline Method: ossimDrect::set_lry
//*******************************************************************
inline void ossimDrect::set_lry(ossim_float64 y)
{
   theLrCorner.y = y;
   theLlCorner.y = y;
}

//*******************************************************************
// Inline Method: ossimDrect::set_llx
//*******************************************************************
inline void ossimDrect::set_llx(ossim_float64 x)
{
   theLlCorner.x = x;
   theUlCorner.x = x;
}

//*******************************************************************
// Inline Method: ossimDrect::set_lly
//*******************************************************************
inline void ossimDrect::set_lly(ossim_float64 y)
{
   theLlCorner.y = y;
   theLrCorner.y = y;
}

//*******************************************************************
// Inline Method: ossimDrect::pointWithin(const ossimDpt& pt) 
//*******************************************************************
inline bool ossimDrect::pointWithin(const ossimDpt& pt) const
{
   if (theOrientMode == OSSIM_LEFT_HANDED)
      return ((pt.x >= ul().x) &&
              (pt.x <= ur().x) &&
              (pt.y >= ul().y) &&
              (pt.y <= ll().y));
   else
      return ((pt.x >= ul().x) &&
              (pt.x <= ur().x) &&
              (pt.y <= ul().y) &&
              (pt.y >= ll().y));
}

//*******************************************************************
// Inline Method: ossimDrect::pointWithin(const ossimFpt& pt)
//*******************************************************************
inline bool ossimDrect::pointWithin(const ossimFpt& pt) const
{
   if (theOrientMode == OSSIM_LEFT_HANDED)
   {
      return ( (pt.x >= ul().x) &&
               (pt.x <= ur().x) &&
               (pt.y >= ul().y) &&
               (pt.y <= ll().y) );
   }
   return ((pt.x >= ul().x) &&
           (pt.x <= ur().x) &&
           (pt.y <= ul().y) &&
           (pt.y >= ll().y));
}

//*******************************************************************
// Inline Method: ossimDrect::midPoint()
//*******************************************************************
inline ossimDpt ossimDrect::midPoint()const
{
   return ossimDpt( (ul().x + ur().x + ll().x + lr().x)*.25,
                    (ul().y + ur().y + ll().y + lr().y)*.25);
}

//*******************************************************************
// Inline Method: ossimDrect::combine(const ossimDrect& rect)
//*******************************************************************
inline ossimDrect ossimDrect::combine(const ossimDrect& rect)const
{
   if (theOrientMode != rect.theOrientMode)
      return(*this);
   
   ossimDpt ulCombine;
   ossimDpt lrCombine;

   if(theOrientMode == OSSIM_LEFT_HANDED)
   {
      ulCombine.x = ((ul().x <= rect.ul().x)?ul().x:rect.ul().x);
      ulCombine.y = ((ul().y <= rect.ul().y)?ul().y:rect.ul().y);
      lrCombine.x = ((lr().x >= rect.lr().x)?lr().x:rect.lr().x);
      lrCombine.y = ((lr().y >= rect.lr().y)?lr().y:rect.lr().y);
   }
   else
   {
      ulCombine.x = ((ul().x <= rect.ul().x)?ul().x:rect.ul().x);
      ulCombine.y = ((ul().y >= rect.ul().y)?ul().y:rect.ul().y);
      lrCombine.x = ((lr().x >= rect.lr().x)?lr().x:rect.lr().x);
      lrCombine.y = ((lr().y <= rect.lr().y)?lr().y:rect.lr().y);
   }

   return ossimDrect(ulCombine, lrCombine);
}
#endif
