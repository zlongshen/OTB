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
// Class declaration for ossimEnviHeaderFileWriter.  Meta data class for
// writing an ENVI (The Environment for Visualizing Images) header file.
//
//----------------------------------------------------------------------------
// $Id: ossimEnviHeaderFileWriter.h,v 1.4 2005/08/08 22:00:24 dburken Exp $

#ifndef ossimEnviHeaderFileWriter_H
#define ossimEnviHeaderFileWriter_H

#include <fstream>
#include <imaging/metadata/ossimMetadataFileWriter.h>
#include <base/data_types/ossimKeywordlist.h>
#include <support_data/envi/ossimEnviHeader.h>

/**
 * Class for writing a "ENVI" style header.
 */
class OSSIMDLLEXPORT ossimEnviHeaderFileWriter : public ossimMetadataFileWriter
{
public:

   /** default constructor */
   ossimEnviHeaderFileWriter();

   /** virtual destructor */
   virtual ~ossimEnviHeaderFileWriter();

   /**
    * Initializes the state of the writer.  Users should use this to set
    * the envi header fields that are not taken from theInputConnection prior
    * to calling execute.
    *
    * @return true on success, false on failure.
    */
   virtual bool loadState(const ossimKeywordlist& kwl, const char* prefix = 0);

   /**
    * Satisfies pure virtual from ossimMetadataWriter base.
    *
    * Appends the writers image types to the "metadatatypeList".
    * 
    * @param metadatatypeList stl::vector<ossimString> list to append to.
    *
    * @note Appends to the list, does not clear it first.
    */
   virtual void getMetadatatypeList(
      std::vector<ossimString>& metadatatypeList) const;

   /**
    * Satisfies pure virtual from ossimMetadataWriter base.
    *
    * @param imageType string representing image type.
    *
    * @return true if "metadataType" is supported by writer.
    */
   virtual bool hasMetadataType(const ossimString& metadataType)const;   
   
private:
   
   virtual bool writeFile();

   ossimEnviHeader theHdr;
   
TYPE_DATA
};
#endif /* End of #ifndef ossimEnviHeaderFileWriter_H */
