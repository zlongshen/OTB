//*******************************************************************
// Copyright (C) 2000 ImageLinks Inc.
//
// OSSIM is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation.
//
// This software is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//
// You should have received a copy of the GNU General Public License
// along with this software. If not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-
// 1307, USA.
//
// See the GPL in the COPYING.GPL file for more details.
//
// Author: Garrett Potts (gpotts@imagelinks.com)
//
// Description: This is a 3-D vector without the homogeneous
//              coordinate.
//
//*******************************************************************
//  $Id: ossimColumnVector3d.h,v 1.7 2005/06/28 15:04:13 gpotts Exp $
#ifndef ossimColumnVector3d_HEADER
#define ossimColumnVector3d_HEADER
#include <base/common/ossimConstants.h>
#include <base/context/ossimNotifyContext.h>
#include <float.h> // for FLT_EPSILON
#include <iomanip>
#include <iostream>
#include <cmath>
using namespace std;

#include <matrix/newmat.h>

class OSSIMDLLEXPORT ossimColumnVector3d
{
public:
   ossimColumnVector3d()
      {
         data[0]=0;
         data[1]=0;
         data[2]=0;
      }

   ossimColumnVector3d(double x, double y, double z=0)
      {
         data[0]=x;
         data[1]=y;
         data[2]=z;
      }

   ossimColumnVector3d(const ossimColumnVector3d &rhs)
      {
         data[0] = rhs.data[0];
         data[1] = rhs.data[1];
         data[2] = rhs.data[2];
      }

   const ossimColumnVector3d& operator=(const NEWMAT::ColumnVector& rhs)
   {
      if ( (rhs.Ncols() == 1) && (rhs.Nrows() == 3) )
      {
         data[0] = rhs[0];
         data[1] = rhs[1];
         data[2] = rhs[2];
      }
      return *this;
   }

   friend ostream& operator <<(ostream& out, const ossimColumnVector3d& v)
      {
         return out << setiosflags(ios::fixed) << setprecision(15)
                    << v[0] << " " << v[1] <<" "
                    << v[2];
      }

   friend ossimColumnVector3d operator*(double scalar,
					const ossimColumnVector3d &v)
      {
         return ossimColumnVector3d(v.data[0]*scalar,
                                    v.data[1]*scalar,
                                    v.data[2]*scalar);
      }

   /*!
    *  Multiplies column vector times matrix.
    *  Note:
    *  If "lhs" is not a 3x3 that's an error and a blank column vector
    *  is returned.
    */
   friend ossimColumnVector3d operator*(const NEWMAT::Matrix& lhs,
                                        const ossimColumnVector3d& rhs)
   {
      if ((lhs.Ncols() == 3) && (lhs.Nrows() == 3))
      {
         return ossimColumnVector3d( (lhs[0][0]*rhs[0] + lhs[0][1]*rhs[1] + lhs[0][2]*rhs[2]),
                                     (lhs[1][0]*rhs[0] + lhs[1][1]*rhs[1] + lhs[1][2]*rhs[2]),
                                     (lhs[2][0]*rhs[0] + lhs[2][1]*rhs[1] + lhs[2][2]*rhs[2]));
      }
      else if((lhs.Ncols() == 4) && (lhs.Nrows() == 4))
      {
         return ossimColumnVector3d( (lhs[0][0]*rhs[0] + lhs[0][1]*rhs[1] + lhs[0][2]*rhs[2] + lhs[0][3]) ,
                                     (lhs[1][0]*rhs[0] + lhs[1][1]*rhs[1] + lhs[1][2]*rhs[2] + lhs[1][3]) ,
                                     (lhs[2][0]*rhs[0] + lhs[2][1]*rhs[1] + lhs[2][2]*rhs[2] + lhs[2][3]));
      }
      ossimNotify(ossimNotifyLevel_WARN) << "Multiplying a 3 row column vector by an invalid matrix" << std::endl;
      return rhs;
   }

   double& operator [](int index)
      {
         return data[index];
      }

   const double& operator [](int index)const
      {
         return data[index];
      }

   ossimColumnVector3d operator*(double scalar)const
      {
         return ossimColumnVector3d(data[0]*scalar,
                                    data[1]*scalar,
                                    data[2]*scalar);
      }

   ossimColumnVector3d operator +(const ossimColumnVector3d &rhs)const
      {
         return ossimColumnVector3d( data[0]+rhs[0],
                                     data[1]+rhs[1],
                                     data[2]+rhs[2]);
      }

   const ossimColumnVector3d& operator +=(const ossimColumnVector3d &rhs)
      {
         data[0] += rhs[0];
         data[1] += rhs[1];
         data[2] += rhs[2];

         return *this;
      }

   ossimColumnVector3d operator - ()const
      {
         return ossimColumnVector3d(-data[0],
                                    -data[1],
                                    -data[2]);
      }

   ossimColumnVector3d operator /(double scalar)const
      {
         return ossimColumnVector3d( data[0]/scalar,
                                     data[1]/scalar,
                                     data[2]/scalar);
      }

   const ossimColumnVector3d& operator /=(double scalar)
      {
         data[0]/=scalar;
         data[1]/=scalar;
         data[2]/=scalar;

         return *this;
      }

   ossimColumnVector3d operator -(const ossimColumnVector3d &rhs)const
      {
         return ossimColumnVector3d( data[0]-rhs[0],
                                     data[1]-rhs[1],
                                     data[2]-rhs[2]);
      }

   const ossimColumnVector3d& operator -=(const ossimColumnVector3d &rhs)
      {
         data[0] -= rhs[0];
         data[1] -= rhs[1];
         data[2] -= rhs[2];

         return *this;
      }

   bool operator ==(const ossimColumnVector3d &rhs) const
      { return ((fabs(data[0] - rhs[0]) <= FLT_EPSILON) &&
		(fabs(data[1] - rhs[1]) <= FLT_EPSILON) &&
		(fabs(data[2] - rhs[2]) <= FLT_EPSILON));
      }

   bool operator !=(const ossimColumnVector3d &rhs) const
      { return !(*this == rhs); }

   double magnitude()const
      {
         return sqrt(data[0]*data[0] +
                     data[1]*data[1] +
                     data[2]*data[2]);
      }
   double dot(const ossimColumnVector3d &rhs) const
      {
         return (data[0]*rhs[0]+
                 data[1]*rhs[1]+
                 data[2]*rhs[2]);
      }

   ossimColumnVector3d unit() const
   {
      double mag = magnitude();

      if(fabs(mag) > FLT_EPSILON)
      {
         mag = 1.0/mag;
         return (*this *mag);
      }

      return *this;
   }

   ossimColumnVector3d cross(const ossimColumnVector3d &rhs) const
      {
         return ossimColumnVector3d( data[1]*rhs[2] - data[2]*rhs[1],
                               data[2]*rhs[0] - data[0]*rhs[2],
                               data[0]*rhs[1] - data[1]*rhs[0]);
      }

   const ossimColumnVector3d& xAligned()
      {
         data[0] = 1;
         data[1] = 0;
         data[2] = 0;

         return *this;
      }
   const ossimColumnVector3d& yAligned()//
      {
         data[0] = 0;
         data[1] = 1;
         data[2] = 0;

         return *this;
      }
   const ossimColumnVector3d& zAligned() //
      {
         data[0] = 0;
         data[1] = 0;
         data[2] = 1;

         return *this;
      }

private:
   double data[3];
};



#endif
