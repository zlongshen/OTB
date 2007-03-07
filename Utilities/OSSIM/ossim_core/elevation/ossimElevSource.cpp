//*****************************************************************************
// FILE: ossimElevSource.cc
//
// Copyright (C) 2001 ImageLinks, Inc.
//
// License:  LGPL
//
// See LICENSE.txt file in the top level directory for more details.
//
// DESCRIPTION:
//   Contains implementation of class ossimElevSource. This is the base class
//   for all sources of elevation data. it maintains a single common static
//   instance of the geoid for use by all instances of objects derived from
//   this one. 
//
// SOFTWARE HISTORY:
//>
//   18Apr2001  Oscar Kramer
//              Initial coding.
//<
//*****************************************************************************
// $Id: ossimElevSource.cpp,v 1.22 2005/11/17 17:22:58 gpotts Exp $

#include <elevation/ossimElevSource.h>
#include <base/common/ossimPreferences.h>
#include <base/data_types/ossimGpt.h>
#include <base/data_types/ossimEcefRay.h>
#include <base/data_types/datum/ossimDatum.h>
#include <base/data_types/ellipse/ossimEllipsoid.h>
#include <base/context/ossimNotifyContext.h>

RTTI_DEF1(ossimElevSource, "ossimElevSource" , ossimSource)

//***
// Define Trace flags for use within this file:
//***
#include <base/common/ossimTrace.h>
static ossimTrace traceExec  ("ossimElevSource:exec");
static ossimTrace traceDebug ("ossimElevSource:debug");

const double ossimElevSource::DEFAULT_NULL_HEIGHT = OSSIM_DBL_NAN;
const double ossimElevSource::DEFAULT_SEA_LEVEL   = 0.0;

static const char ENABLE_STATS_KW[] = "elevation.compute_statistics.enabled";

ossimElevSource::ossimElevSource()
   :
      theMinHeightAboveMSL (0.0),
      theMaxHeightAboveMSL (0.0),
      theNullHeightValue   (DEFAULT_NULL_HEIGHT),
      theSeaLevelValue     (DEFAULT_SEA_LEVEL),
      theGroundRect(),
      theComputeStatsFlag(true)
   
{
   if (traceExec())
   {
      ossimNotify(ossimNotifyLevel_DEBUG)
         << "DEBUG ossimElevSource::ossimElevSource: entering..."
         << std::endl;
   }
   
   // User can turn on off with a keyword.
   const char* lookup =
      ossimPreferences::instance()->findPreference(ENABLE_STATS_KW);
   if (lookup)
   {
     theComputeStatsFlag = ossimString(lookup).toBool();
   }

   if (traceDebug())
   {
      ossimNotify(ossimNotifyLevel_DEBUG)
         << "DEBUG:"
         << "\ntheComputeStatsFlag:  "
         << (theComputeStatsFlag?"true":"false")
         << endl;
   }


   if (traceExec())
   {
      ossimNotify(ossimNotifyLevel_DEBUG)
         << "DEBUG ossimElevSource::ossimElevSource: returning..."
         << std::endl;
   }
}

ossimElevSource::~ossimElevSource()
{}

double ossimElevSource::getHeightAboveEllipsoid(const ossimGpt& argGpt)
{
   ossimNotify(ossimNotifyLevel_FATAL) << "FATAL ossimElevSource::getHeightAboveEllipsoid Not implemented..."
                                       << std::endl;
   return theNullHeightValue;
}

//*****************************************************************************
//  METHOD: intersectRay()
//  
//  Service method for intersecting a ray with the elevation surface to
//  arrive at a ground point. The ray is expected to originate ABOVE the
//  surface and pointing down.
//
//  NOTE: the gpt argument is expected to be initialized with the desired
//  datum, including ellipsoid, for the proper intersection point to be
//  computed.
//
//  LIMITATION: This release supports only single valued solutions, i.e., it
//  is possible a ray passing through one side of a mountain and out the other
//  will return an intersection with the far side. Eventually, a more robust
//  algorithm will be employed.
//
//*****************************************************************************
bool ossimElevSource::intersectRay(const ossimEcefRay& ray, ossimGpt& gpt)
{
   if (traceExec())  ossimNotify(ossimNotifyLevel_DEBUG) << "DEBUG ossimElevSource::intersectRay: entering..." << std::endl;

   static const double CONVERGENCE_THRESHOLD = 0.001; // meters
   static const int    MAX_NUM_ITERATIONS    = 50;
   
   double          h_ellips; // height above ellipsoid
   bool            intersected;
   ossimEcefPoint  prev_intersect_pt (ray.origin());
   ossimEcefPoint  new_intersect_pt;
   double          distance;
   bool            done = false;
   int             iteration_count = 0;
   
   //***
   // Set the initial guess for horizontal intersect position as the ray's
   // origin, and establish the datum and ellipsoid:
   //***
   const ossimDatum*     datum     = gpt.datum();
   const ossimEllipsoid* ellipsoid = datum->ellipsoid();
//    double lat, lon, h;

//    ellipsoid->XYZToLatLonHeight(ray.origin().x(),
//                                 ray.origin().y(),
//                                 ray.origin().z(),
//                                 lat, lon, h);
//    ossimGpt nadirGpt(lat, lon, h);

//    std::cout << "nadir pt = " << nadirGpt << std::endl;
   
   gpt = ossimGpt(prev_intersect_pt, datum);

   //
   // Loop to iterate on ray intersection with variable elevation surface:
   //
   do
   {
      //
      // Intersect ray with ellipsoid inflated by h_ellips:
      //
      h_ellips = getHeightAboveEllipsoid(gpt);
      if (h_ellips == theNullHeightValue) h_ellips = 0.0;
      
      intersected = ellipsoid->nearestIntersection(ray,
                                                   h_ellips,
                                                   new_intersect_pt);
      if (!intersected)
      {
         //
         // No intersection (looking over the horizon), so set ground point
         // to NaNs:
         //
         gpt.makeNan();
         done = true;
      }
      else
      {
         //
         // Assign the ground point to the latest iteration's intersection
         // point:
         //
         gpt = ossimGpt(new_intersect_pt, datum);
         
         //
         // Determine if convergence achieved:
         //
         distance = (new_intersect_pt - prev_intersect_pt).magnitude();
         if (distance < CONVERGENCE_THRESHOLD)
            done = true;
         else
         {
            prev_intersect_pt = new_intersect_pt;
         }
      }

      iteration_count++;

   } while ((!done) && (iteration_count < MAX_NUM_ITERATIONS));

   if (iteration_count == MAX_NUM_ITERATIONS)
   {
      if(traceDebug())
      {
         ossimNotify(ossimNotifyLevel_WARN) << "WARNING ossimElevSource::intersectRay: Max number of iterations reached solving for ground "
                                            << "point. Result is probably inaccurate." << std::endl;
      }
   }
   
   if (traceExec())  ossimNotify(ossimNotifyLevel_DEBUG) << "DEBUG ossimElevSource::intersectRay: returning..." << std::endl;
   return intersected;
}

double ossimElevSource::getMinHeightAboveMSL() const
{
   return theMinHeightAboveMSL;
}

double ossimElevSource::getMaxHeightAboveMSL() const
{
   return theMaxHeightAboveMSL;
}

double ossimElevSource::getNullHeightValue() const
{
   return theNullHeightValue;
}

double ossimElevSource::getSeaLevelValue() const
{
   return theSeaLevelValue;
}

const ossimGrect& ossimElevSource::getBoundingGndRect() const
{
   return theGroundRect;
}

bool ossimElevSource::canConnectMyInputTo(ossim_int32 inputIndex,
                                          const ossimConnectableObject* object)const
{
   return false;
}

void ossimElevSource::initialize()
{}

ossimFilename ossimElevSource::getFilename() const
{
   return ossimFilename::NIL;
}

bool ossimElevSource::open() const
{
   return true;
}

void ossimElevSource::close() const
{
}

bool ossimElevSource::getComputeStatisticsFlag() const
{
   return theComputeStatsFlag;
}

void ossimElevSource::setComputeStatisticsFlag(bool flag)
{
   theComputeStatsFlag = flag;
}
