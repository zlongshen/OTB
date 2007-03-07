//*******************************************************************
// Copyright (C) 2003 ImageLinks Inc. 
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
// Author:  David Burken <dburken@imagelinks.com>
// 
// Description:
//
// Class declaration for ossimIkonosMetaData.
// 
// This class parses a Space Imaging Ikonos meta data file.
//
//********************************************************************
// $Id: ossimIkonosMetaData.h,v 1.1 2003/05/29 21:17:10 dburken Exp $

#ifndef ossimIkonosMetaData_HEADER
#define ossimIkonosMetaData_HEADER

#include "base/common/ossimObject.h"
#include "base/common/ossimErrorStatusInterface.h"

class OSSIMDLLEXPORT ossimIkonosMetaData : public ossimObject,
                                           public ossimErrorStatusInterface
{
public:
   ossimIkonosMetaData();
   ossimIkonosMetaData(const char* metaDataFile);
   virtual ~ossimIkonosMetaData();

 private:
   
TYPE_DATA   
};

#endif /* #ifndef ossimIkonosMetaData_HEADER */
