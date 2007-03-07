//*******************************************************************
// Copyright (C) 2001 ImageLinks Inc. 
//
// License:  LGPL
// 
// See LICENSE.txt file in the top level directory for more details.
//
// Author:  David Burken
//
// Description:
//
// Class declaration of ossimU8ImageData.  Specialized image data object for
// unsigned char data.
//
// NOTE:  This object is optimized for unsigned char data and assumes the
//        following:  null pixel value  = 0.0
//                    min  pixel value  = 1.0
//                    max  pixel value  = 255.0
//
//        If you want anything else use the less efficient ossimImageData.
//
//*************************************************************************
// $Id: ossimU8ImageData.h,v 1.19 2005/07/23 15:54:50 dburken Exp $

#ifndef ossimU8ImageData_HEADER
#define ossimU8ImageData_HEADER

#include "ossimImageData.h"
#include "imaging/remap_tables/ossimNormalizedU8RemapTable.h"

class OSSIMDLLEXPORT ossimU8ImageData : public ossimImageData
{
public:
   ossimU8ImageData(ossimSource* source=NULL,
                    ossim_uint32 bands = 1);
   
   ossimU8ImageData(ossimSource* source,
                    ossim_uint32 bands,
                    ossim_uint32 width,
                    ossim_uint32 height);
   
   ossimU8ImageData(const ossimU8ImageData& rhs)
      :ossimImageData(rhs)
      {}

   virtual ~ossimU8ImageData();

   virtual ossimObject* dup()const;

   /*!
    * will fill the entire band with
    * the value.
    */
   void fill(ossim_uint32 band, double value);
   void fill(double value);

   bool isNull(ossim_uint32 offset)const;
   void setNull(ossim_uint32 offset);

   virtual ossimDataObjectStatus validate() const;

   /*!
    * will go to the band and offset and compute the
    * normalized float and return it back to the
    * caller through the result argument.
    */
   virtual void getNormalizedFloat(ossim_uint32 offset,
                                   ossim_uint32 bandNumber,
                                   float& result)const;

   /*!
    * This will assign to this object a normalized
    * value by unnormalizing to its native type.
    */
   virtual void setNormalizedFloat(ossim_uint32 offset,
                                   ossim_uint32 bandNumber,
                                   float input);
   
   /*!
    * Will use the memory that you pass in to normalize
    * this data object.
    */
   virtual void convertToNormalizedFloat(ossimImageData* result)const;
   
   /*!
    * Will use the memory that you pass in to normalize
    * this data object.
    */
   virtual void convertToNormalizedDouble(ossimImageData* result)const;

   /*!
    * Will take the normalized input and convert it
    * to this tile's data type.  Example:  if this
    * tile is of type UCHAR and its input is of type
    * NORALIZED_FLOAT it will unnormalize the data by
    * doing:
    *
    * minPix + normalizedInput*(maxPix-minPix)
    *
    * on a per band basis.
    */
   virtual void unnormalizeInput(ossimImageData* normalizedInput);

   /*!
    * This will compute the average value for the band.
    */
   virtual double computeAverageBandValue(ossim_uint32 bandNumber = 0);

   /*!
    * This will call the compute average band value and then
    * use that in the calculation of:
    
    * It will then do a SUM[(Mean - Actual)^2]/width*height.
    *
    * This is the average variance from the passed in
    * mean.  Basically think of the mean as a completely
    * grey image and we would like to see how this
    * image varies from the passed in mean.
    */
   virtual double computeMeanSquaredError(double meanValue,
                                          ossim_uint32 bandNumber = 0);
  
   virtual void populateHistogram(ossimMultiBandHistogram* histo);

   /*!
    *  Initializes data to null pixel values.
    */
   virtual void makeBlank();

   virtual void setValue(ossim_int32 x, ossim_int32 y, double color);

   virtual void loadBand(const void* src,
                         const ossimIrect& src_rect,
                         ossim_uint32 band);
   
   virtual void loadTile(void* src,
                         const ossimIrect& src_rect,
                         ossimInterleaveType il_type);

   virtual void unloadBand(void* dest,
                           const ossimIrect& dest_rect,
                           ossim_uint32 band) const;

   /*!
    * Copies entire tile to buf passed in.  Data put in buf is normalized.
    * The "buf" passed to method is assumed to be at least as big as:
    * "getSize() * sizeof(double)"
    */
   virtual void copyTileToNormalizedBuffer(double* buf) const;
   virtual void copyTileToNormalizedBuffer(float* buf) const;

   /*!
    * Will copy this tiles specified band number to the normalized buffer.
    * if the band is out of range then nothing is done and returns.
    */
   virtual void copyTileToNormalizedBuffer(ossim_uint32 band, double* buf) const;
   virtual void copyTileToNormalizedBuffer(ossim_uint32 band, float* buf)const;

   /*!
    * Copies buf passed in to tile.  Data is unnormalized to the tile's
    * scalar type.
    * The "buf" passed to method is assumed to be at least as big as the tiles:
    * "getSize() * sizeof(double)"
    */
   virtual void copyNormalizedBufferToTile(double* buf);
   virtual void copyNormalizedBufferToTile(float* buf);

   /*!
    * Will copy the normalized buffer to this tiles
    * specified band.  If band is out of range then nothing
    * is done and returns.
    */
   virtual void copyNormalizedBufferToTile(ossim_uint32 band,
                                           double* buf);
   virtual void copyNormalizedBufferToTile(ossim_uint32 band,
                                           float* buf);
   
   virtual ossim_float64 getMinNormalizedPix() const { return theRemapTable[1]; }
   
private:

   const ossimU8ImageData& operator =(const ossimU8ImageData &rhs);

   static const ossimNormalizedU8RemapTable theRemapTable;
   
TYPE_DATA
};

#endif
