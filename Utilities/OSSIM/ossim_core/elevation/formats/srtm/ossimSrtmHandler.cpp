//----------------------------------------------------------------------------
// Copyright (c) 2005, David Burken, all rights reserved.
//
// License:  LGPL
// 
// See LICENSE.txt file in the top level directory for more details.
//
// Author:  David Burken
//
// Description:
// 
// Shuttle Radar Topography Mission (SRTM) elevation source.
//
//----------------------------------------------------------------------------
// $Id: ossimSrtmHandler.cpp,v 1.7 2005/11/16 15:06:33 dburken Exp $

#include <elevation/formats/srtm/ossimSrtmHandler.h>
#include <support_data/srtm/ossimSrtmSupportData.h>
#include <base/common/ossimCommon.h>
#include <base/common/ossimKeywordNames.h>
#include <base/context/ossimNotifyContext.h>
#include <base/data_types/ossimGpt.h>
#include <base/misc/ossimEndian.h>
#include <base/factory/ossimStreamFactoryRegistry.h>

RTTI_DEF1(ossimSrtmHandler, "ossimSrtmHandler" , ossimElevCellHandler)

//***
// Define Trace flags for use within this file:
//***
#include <base/common/ossimTrace.h>
static ossimTrace traceDebug ("ossimSrtmElevSource:debug");

ossimSrtmHandler::ossimSrtmHandler(const ossimFilename& srtmFile)
   :
      ossimElevCellHandler(srtmFile),
      theFileStr(0),
      theNumberOfLines(0),
      theNumberOfSamples(0),
      theSrtmRecordSizeInBytes(0),
      theLatSpacing(0.0),
      theLonSpacing(0.0),
      theNwCornerPost(),
      theSwapper(NULL)
{
   ossimSrtmSupportData sd;
   if ( !sd.setFilename(srtmFile, false) )
   {
      theErrorStatus = ossimErrorCodes::OSSIM_ERROR;
      return;
   }

   theScalarType = sd.getScalarType();
   // SRTM is stored in big endian.
   if ( ossimGetByteOrder() == OSSIM_LITTLE_ENDIAN)
   {
      theSwapper = new ossimEndian();
   }

   theNumberOfLines         = sd.getNumberOfLines();
   theNumberOfSamples       = sd.getNumberOfSamples();
   theSrtmRecordSizeInBytes = theNumberOfSamples * ossimGetScalarSizeInBytes(theScalarType);
   theLatSpacing            = sd.getLatitudeSpacing();
   theLonSpacing            = sd.getLongitudeSpacing();
   theNwCornerPost.lon      = sd.getSouthwestLongitude();
   theNwCornerPost.lat      = sd.getSouthwestLatitude()+1.0;
   theScalarType            = sd.getScalarType();
   theGroundRect = ossimGrect(ossimGpt(theNwCornerPost.lat,
                                       theNwCornerPost.lon,
                                       0.0),
                              ossimGpt(theNwCornerPost.lat-1.0,
                                       theNwCornerPost.lon+1.0,
                                       0.0));

   //---
   //  Determine the mean spacing:
   //---
   // double center_lat = (south_boundary + north_boundary)/2.0;
   // theMeanSpacing = (theLatSpacing + theLonSpacing*cosd(center_lat))
   //          * ossimGpt().metersPerDegree().x / 2.0;

   //  Initialize the accuracy values:
   // theAbsLE90 = (double) acc.absLE();
   // theAbsCE90 = (double) acc.absCE();

   // Set the base class null height value.
   theNullHeightValue = -32768.0;

   // theMinHeightAboveMSL;
   // theMaxHeightAboveMSL;
}

ossimSrtmHandler::~ossimSrtmHandler()
{
   if (theSwapper)
   {
      delete theSwapper;
      theSwapper = NULL;
   }

   theFileStr = 0;
//    if(theFileStr)
//    {
//       delete theFileStr;
//       theFileStr = 0;
//    }
//   theFileStr.close();
}

double ossimSrtmHandler::getHeightAboveMSL(const ossimGpt& gpt)
{
   switch(theScalarType)
   {
      case OSSIM_SINT16:
      {
         return getHeightAboveMSLTemplate((ossim_sint16)0,
                                          gpt);
      }
      case OSSIM_FLOAT32:
      {
         return getHeightAboveMSLTemplate((ossim_float32)0,
                                          gpt);
      }
      default:
      {
         break;
      }
   }

   return NULL_POST;
}

template <class T>
double ossimSrtmHandler::getHeightAboveMSLTemplate(T dummy,
                                                   const ossimGpt& gpt)
{
   // Establish the grid indexes:
   double xi = (gpt.lon - theNwCornerPost.lon) / theLonSpacing;
   double yi = (theNwCornerPost.lat - gpt.lat) / theLatSpacing;

   int x0 = static_cast<int>(xi);
   int y0 = static_cast<int>(yi);

   // Check for right edge.
   if (gpt.lon == theGroundRect.lr().lon)
   {
      --x0; // Move over one post to the left.
   }
   
   // Check for bottom edge.
   if (gpt.lat == theGroundRect.lr().lat)
   {
      --y0; // Move up one post.
   }

   // Do some error checking.
   if ( xi < 0.0 || yi < 0.0 ||
        x0 > (theNumberOfSamples  - 2.0) ||
        y0 > (theNumberOfLines    - 2.0) )
   {
//       std::cout << "ossimSrtmHandler::getHeightAboveMSLTemplate: leaving 1" << std::endl;
      return theNullHeightValue;
   }

   // We need an open stream to file from here on out...
   if (!open())
   {
//       std::cout << "ossimSrtmHandler::getHeightAboveMSLTemplate: leaving 2" << std::endl;
      return theNullHeightValue;
   }

   T p[4];
   
   double p00;
   double p01;
   double p10;
   double p11;

   //---
   // Grab the four points from the srtm cell needed.
   //---
   streampos offset = y0 * theSrtmRecordSizeInBytes + x0 * sizeof(T);

//   ossim_int64 currentG = theFileStr->tellg();
   // lets a seek cur
   // 
   // Put the file pointer at the start of the first elevation post.
      theFileStr->seekg(offset, ios::beg);
      //theFileStr->seekg(offset-currentG, ios::cur);
   // Get the first post.
   theFileStr->read((char*)p, sizeof(T));
   
   // Get the second post.
   theFileStr->read((char*)(p+1), sizeof(T));
   
   // Move over to the next column.
   offset += theSrtmRecordSizeInBytes;

//   currentG = theFileStr->tellg();
//   theFileStr->seekg(offset-currentG, ios::cur);
   
   theFileStr->seekg(offset, ios::beg);
   
   // Get the third post.
   theFileStr->read((char*)(p+2), sizeof(T));

   // Get the fourth post.
   theFileStr->read((char*)(p+3), sizeof(T));

   if(theFileStr->fail())
   {
      return theNullHeightValue;
   }
   if (theSwapper)
   {
      theSwapper->swap((T*)p, 4);
   }

   p00 = p[0];
   p01 = p[1];
   p10 = p[2];
   p11 = p[3];
   
   // Perform bilinear interpolation:
//    double wx1 = xi  - x0;
//    double wy1 = yi  - y0;
//    double wx0 = 1.0 - wx1;
//    double wy0 = 1.0 - wy1;

   double xt0 = xi - x0;
   double yt0 = yi - y0;
   double xt1 = 1-xt0;
   double yt1 = 1-yt0;

//    double w00 = wx0*wy0;
//    double w01 = wx0*wy1;
//    double w10 = wx1*wy0;
//    double w11 = wx1*wy1;
   double w00 = xt1*yt1;
   double w01 = xt0*yt1;
   double w10 = xt1*yt0;
   double w11 = xt0*yt0;

   //***
   // Test for null posts and set the corresponding weights to 0:
   //***
   if (p00 == theNullHeightValue)
      w00 = 0.0;
   if (p01 == theNullHeightValue)
      w01 = 0.0;
   if (p10 == theNullHeightValue)
      w10 = 0.0;
   if (p11 == theNullHeightValue)
      w11 = 0.0;

#if 0 /* Serious debug only... */
   cout << "\np00:  " << p00
        << "\np01:  " << p01
        << "\np10:  " << p10
        << "\np11:  " << p11
        << "\nw00:  " << w00
        << "\nw01:  " << w01
        << "\nw10:  " << w10
        << "\nw11:  " << w11
        << endl;
#endif

   double sum_weights = w00 + w01 + w10 + w11;

   if (sum_weights)
   {
      return (p00*w00 + p01*w01 + p10*w10 + p11*w11) / sum_weights;
   }

   return theNullHeightValue;
}

double ossimSrtmHandler::getPostValue(const ossimIpt& gridPt) const
{
   static const char MODULE[] = "ossimSrtmHandler::getPostValue";
   
   if(!theFileStr.valid()) return theNullHeightValue;
   
   // Do some error checking.
   if ( gridPt.x < 0.0 || gridPt.y < 0.0 ||
        gridPt.x > (theNumberOfSamples  - 1) ||
        gridPt.y > (theNumberOfLines - 1) )
   {
      if(traceDebug())
      {
         ossimNotify(ossimNotifyLevel_WARN)
            << "WARNING " << MODULE << ": No intersection..." << std::endl;
      }
      return theNullHeightValue;
   }

   if (!open())
   {
      return theNullHeightValue;
   }

   streampos offset = gridPt.y * theSrtmRecordSizeInBytes + gridPt.x * 2;

   // Put the file pointer at the start of the first elevation post.
   theFileStr->seekg(offset, ios::beg);

   ossim_sint16 p;

   // Get the post.
   theFileStr->read((char*)&p, 2);
   if (theSwapper)
   {
      theSwapper->swap(p);
   }
   return p;
}

ossimIpt ossimSrtmHandler::getSizeOfElevCell() const
{
   return ossimIpt(theNumberOfSamples, theNumberOfLines);
}

const ossimSrtmHandler&
ossimSrtmHandler::operator=(const ossimSrtmHandler& rhs)
{
   return rhs;
}

ossimSrtmHandler::ossimSrtmHandler(const ossimSrtmHandler&)
{
}


bool ossimSrtmHandler::open() const
{
   if(theFileStr.valid())
   {
      if(!theFileStr->fail())
      {
         return true;
      }
   }
   theFileStr = 0;

   
   theFileStr = ossimStreamFactoryRegistry::instance()->createNewInputStream(theFilename,
                                                                             ios::in | ios::binary);   

   if(theFileStr.valid())
   {
      return (!theFileStr->fail());
   }
   return false;
}

void ossimSrtmHandler::close() const
{
   if(theFileStr.valid())
   {
      theFileStr->close();
      theFileStr = 0;
   }
   
}
