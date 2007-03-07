#include <sstream>
#include <parallel/ossimOrthoIgen.h>
#include <parallel/ossimIgen.h>
#include <base/context/ossimNotifyContext.h>
#include <base/common/ossimTrace.h>
#include <base/data_types/ossimKeywordlist.h>
#include <imaging/annotation/ossimGeoAnnotationSource.h>
#include <imaging/ossimImageChain.h>
#include <imaging/formats/ossimImageHandler.h>
#include <imaging/tile_sources/ossimImageRenderer.h>
#include <imaging/tile_sources/ossimHistogramRemapper.h>
#include <imaging/tile_sources/ossimImageCombiner.h>
#include <imaging/resample/ossimFilterResampler.h>
#include <imaging/factory/ossimImageHandlerRegistry.h>
#include <imaging/factory/ossimImageWriterFactoryRegistry.h>
#include <base/common/ossimKeywordNames.h>
#include <imaging/formats/tiff/ossimTiffWriter.h>
#include <projections/map_projections/ossimUtmProjection.h>
#include <projections/map_projections/ossimEquDistCylProjection.h>
#include <projections/factory/ossimProjectionFactoryRegistry.h>
#include <base/factory/ossimObjectFactoryRegistry.h>
#include <imaging/tile_sources/ossimGeoPolyCutter.h>
#include <imaging/tile_sources/ossimEastingNorthingCutter.h>
#include <base/misc/ossimUnitConversionTool.h>
#include "ossimMpi.h"

static ossimTrace traceDebug("ossimOrthoIgen:debug");
static ossimTrace traceLog("ossimOrthoIgen:log");

ossimOrthoIgen::ossimOrthoIgenFilename::ossimOrthoIgenFilename(const ossimFilename& file, bool decodeEntry)
{
   if(decodeEntry)
   {
      setFilenameWithDecoding(file);
   }
   else
   {
      setFilenameAndEntry(file, -1);
   }
}

void ossimOrthoIgen::ossimOrthoIgenFilename::setFilenameWithDecoding(const ossimFilename& file)
{
   ossimFilename::size_type idx = file.rfind("|", file.length());
   ossimString actualFile = file;
   ossimString entry("-1");       
   if(idx != ossimFilename::npos)
   {
      actualFile = ossimString(file.begin(), file.begin()+idx);
      entry      = ossimString(file.begin()+idx+1,
                               file.end());
      entry = entry.trim();
      actualFile = actualFile.trim();
   }

   theFilename = ossimFilename(actualFile);
   theEntry    = entry.toInt32();
}

ossimOrthoIgen::ossimOrthoIgen()
   :
   theThumbnailRes(""),
   theThumbnailFlag(false),
   theMetersPerPixelOverride(OSSIM_DBL_NAN, OSSIM_DBL_NAN),
   theProjectionType(OSSIM_UNKNOWN_PROJECTION),
   theProjectionName(""),
   theGeographicOriginOfLatitude(0.0),
   theCombinerType("ossimImageMosaic"),
   theResamplerType("nearest neighbor"),
   theTemplateView(""),
   theTilingTemplate(""),
   theTilingFilename(""),
   theChainTemplate(""),
   theCombinerTemplate(""),
   theAnnotationTemplate(""),
   theWriterTemplate(""),
   theSlaveBuffers("2"),
   theCutCenter(OSSIM_DBL_NAN, OSSIM_DBL_NAN),
   theCutDxDy(OSSIM_DBL_NAN, OSSIM_DBL_NAN),
   theCutCenterUnit(OSSIM_UNIT_UNKNOWN),
   theCutDxDyUnit(OSSIM_UNIT_UNKNOWN),
   theLowPercentClip(OSSIM_DBL_NAN),
   theHighPercentClip(OSSIM_DBL_NAN),
   theFilenames()
{
   // setDefaultValues();
}

void ossimOrthoIgen::addArguments(ossimArgumentParser& argumentParser)
{
   argumentParser.getApplicationUsage()->addCommandLineOption("--annotate", "annotation keyword list");

   argumentParser.getApplicationUsage()->addCommandLineOption("-t or --thumbnail", "thumbnail resolution");
   
   argumentParser.getApplicationUsage()->addCommandLineOption("--meters","Specifies an override for the meters per pixel");
   argumentParser.getApplicationUsage()->addCommandLineOption("--slave-buffers","number of slave tile buffers for mpi processing (default = 2)");
   argumentParser.getApplicationUsage()->addCommandLineOption("--view-template","Specify an external file that contains view information");
   argumentParser.getApplicationUsage()->addCommandLineOption("--chain-template","Specify an external file that contains chain information");
   argumentParser.getApplicationUsage()->addCommandLineOption("--combiner-template","Specify an external file that contains combiner information");
   argumentParser.getApplicationUsage()->addCommandLineOption("--tiling-template","Specify an external file that contains tiling information");
   argumentParser.getApplicationUsage()->addCommandLineOption("--writer-template","Specify an external file that contains tiling information");
   
   argumentParser.getApplicationUsage()->addCommandLineOption("--utm","Defaults to a utm image chain with GSD = to the input");

   argumentParser.getApplicationUsage()->addCommandLineOption("--geo","Defaults to a geographic image chain with GSD = to the input.  Origin of latitude is on the equator.");
   argumentParser.getApplicationUsage()->addCommandLineOption("--input-proj","Makes the view equal to the input.  If more than one file then the first is taken");
   
   argumentParser.getApplicationUsage()->addCommandLineOption("--geo-scaled","Takes a latitude as an argument for purpose of scaling.  Specifies that no spec file was defined.  Defaults to a scaled geographic image chain with GSD = to the input.");
   
   argumentParser.getApplicationUsage()->addCommandLineOption("--combiner-type","Specify what mosaic to use, ossimImageMosiac or ossimFeatherMosaic or osimBlendMosaic ... etc");
   
   argumentParser.getApplicationUsage()->addCommandLineOption("--resample-type","Specify what resampler to use, nearest neighbor, bilinear, bicubic");
   
   argumentParser.getApplicationUsage()->addCommandLineOption("--cut-center-ll","Specify the center cut in lat lon space.  Takes two argument <lat> <lon>");
   argumentParser.getApplicationUsage()->addCommandLineOption("--cut-radius-meters","Specify the cut distance in meters.  A bounding box for the cut will be produced");

   argumentParser.getApplicationUsage()->addCommandLineOption("--hist-stretch","Specify in normalized percent the low clip and then the high clip value");
}

void ossimOrthoIgen::initialize(ossimArgumentParser& argumentParser)
{
   double tempDouble;
   double tempDouble2;
   ossimString tempString;
   ossimArgumentParser::ossimParameter stringParam(tempString);
   ossimArgumentParser::ossimParameter doubleParam(tempDouble);
   ossimArgumentParser::ossimParameter doubleParam2(tempDouble2);
   theCutCenter.makeNan();
   theCutDxDy.makeNan();
   theCutCenterUnit   = OSSIM_UNIT_UNKNOWN;
   theCutDxDyUnit     = OSSIM_UNIT_UNKNOWN;
   theLowPercentClip  = OSSIM_DBL_NAN;
   theHighPercentClip = OSSIM_DBL_NAN;
   
   theMetersPerPixelOverride.makeNan();
   if(argumentParser.read("--annotate", stringParam))
   {
      theAnnotationTemplate = ossimFilename(tempString);
   }
   if(argumentParser.read("-t", stringParam)   ||
      argumentParser.read("--thumbnail", stringParam))
   {
      theThumbnailRes  = tempString;
      theThumbnailFlag = true;
   }
   if(argumentParser.read("--slave-buffers", stringParam))
   {
      theSlaveBuffers = tempString;
   }
   if(argumentParser.read("--cut-center-ll", doubleParam, doubleParam2))
   {
      theCutCenter.lat = tempDouble;
      theCutCenter.lon = tempDouble2;
      theCutCenterUnit = OSSIM_DEGREES;
   }
   if(argumentParser.read("--cut-radius-meters", doubleParam))
   {
      theCutDxDy.x = tempDouble;
      theCutDxDy.y = tempDouble;
      theCutDxDyUnit = OSSIM_METERS;
   }
   if(argumentParser.read("--hist-stretch", doubleParam, doubleParam2))
   {
      theLowPercentClip = tempDouble;
      theHighPercentClip = tempDouble2;
   }
   if(argumentParser.read("--meters", doubleParam))
   {
      theMetersPerPixelOverride.x = tempDouble;
      theMetersPerPixelOverride.y = tempDouble;
   }
   if(argumentParser.read("--writer-template",
                          stringParam))
   {
      theWriterTemplate = tempString;
   }
   if(argumentParser.read("--tiling-template",
                          stringParam))
   {
      theTilingTemplate = ossimFilename(tempString);
   }
   if(argumentParser.read("--chain-template", stringParam))
   {
      theChainTemplate = ossimFilename(tempString);
   }
   if(argumentParser.read("--combiner-template", stringParam))
   {
      theCombinerTemplate = ossimFilename(tempString);
   }
   if (argumentParser.read("--utm"))
   {
      theProjectionType = OSSIM_UTM_PROJECTION;
      theProjectionName = "ossimUtmProjection";
   }
   else if(argumentParser.read("--geo"))
   {
      theProjectionType = OSSIM_GEO_PROJECTION;
      theProjectionName = "ossimEquDistCylProjection";
   }
   else if(argumentParser.read("--input-proj"))
   {
      theProjectionType = OSSIM_INPUT_PROJECTION;
   }
   else
   {
      theProjectionType = OSSIM_UNKNOWN_PROJECTION;
   }
   if(argumentParser.read("--view-template", stringParam))
   {
      theTemplateView = ossimFilename(tempString);
      theProjectionType = OSSIM_EXTERNAL_PROJECTION;
   }
   if(argumentParser.read("--geo-scaled", doubleParam))
   {
      theProjectionType = OSSIM_GEO_PROJECTION;
      theProjectionName = "ossimEquDistCylProjection";
      if ( (tempDouble < 90.0) && (tempDouble > -90.0) )
      {
         theGeographicOriginOfLatitude = tempDouble;
      }
      else
      {
         theGeographicOriginOfLatitude = 0.0;
         ossimNotify(ossimNotifyLevel_WARN)
            << "ossimOrthoIgen::initialize WARNING:"
            << "\nLatitude out  of range!  Must be between -90 and 90."
            << "\nRemains at zero."
            << std::endl;
      }
      if (traceDebug())
      {
         ossimNotify(ossimNotifyLevel_DEBUG)
            << "ossimOrthoIgen::initialize DEBUG:"
            << "\ngeographicOriginOfLatitude:  " << theGeographicOriginOfLatitude
            << std::endl;
      }
   }
   if(argumentParser.read("--combiner-type", stringParam))
   {
      theCombinerType = tempString;
   }
   else
   {
      theCombinerType = "ossimImageMosaic";
   }
   if(argumentParser.read("--resample-type", stringParam))
   {
      theResamplerType = tempString;
   }

   
}

void ossimOrthoIgen::addFiles(ossimArgumentParser& argumentParser,
                              bool withDecoding,
                              ossim_uint32 startIdx)
{
   ossim_uint32 idx = startIdx;
   while(argumentParser.argv()[idx])
   {
      addFile(ossimFilename(argumentParser.argv()[idx]), withDecoding);
//      theFilenames.push_back(ossimFilename(argumentParser.argv()[idx]));
      ++idx;
   }
}

bool ossimOrthoIgen::execute()
{
   if (traceDebug())
   {
      ossimNotify(ossimNotifyLevel_DEBUG)
         << "ossimOrthoIgen::execute DEBUG: Entered ..."
         << std::endl;
   }
//   double start=0, stop=0;

   if(theFilenames.size() < 1)
   {
      ossimNotify(ossimNotifyLevel_WARN)
         << "ossimOrthoIgen::execute WARNING: No filenames to process"
         << std::endl;
      return false;
   }

//    if(ossimMpi::instance()->getRank() == 0)
//    {
//       start = ossimMpi::instance()->getTime();
//    }
   ossimKeywordlist igenKwl;
   ossimKeywordlist inputGeom;
   if(ossimMpi::instance()->getRank() == 0)
   {
      if(!setupIgenKwl(igenKwl))
      {
         if (traceDebug())
         {
            ossimNotify(ossimNotifyLevel_DEBUG)
               << "ossimOrthoIgen::execute DEBUG: setupIgenKwl returned false..."
               << std::endl;
         }
         return false;
      }

      if (traceLog())
      {
         const char* lookup = igenKwl.find("object2.filename");
         if (lookup)
         {
            ossimFilename logFile = lookup;
            logFile.setExtension("log");
            igenKwl.write(logFile.c_str());
         }
      }
   }
   
   ossimIgen *igen = new ossimIgen;
   igen->initialize(igenKwl);
   igen->outputProduct();
   
//    if(ossimMpi::instance()->getRank() == 0)
//    {
//       stop = ossimMpi::instance()->getTime();
//       ossimNotify(ossimNotifyLevel_NOTICE)
//          << "Time elapsed: " << (stop-start)
//          << std::endl;
//    }
   delete igen;
   igen = NULL;

   return true;
}

void ossimOrthoIgen::clearFilenameList()
{
   theFilenames.clear();
}

void ossimOrthoIgen::addFile(const ossimFilename& file,
                             bool withEncodedEntry)
{
   ossimOrthoIgenFilename filename;
   if(withEncodedEntry)
   {
      filename.setFilenameWithDecoding(file);
   }
   else
   {
      filename.setFilenameAndEntry(file, -1);
   }
   
   if (traceDebug())
   {
      ossimNotify(ossimNotifyLevel_DEBUG)
         << "ossimOrthoIgen::addFile DEBUG:"
         << "\nAdded file: " << filename.theFilename
         << std::endl;
   }   

   theFilenames.push_back(filename);
}

void ossimOrthoIgen::setDefaultValues()
{
   theThumbnailRes = "";
   theThumbnailFlag = false;
   theMetersPerPixelOverride.makeNan();
   theTemplateView = "";
   theProjectionType = OSSIM_UNKNOWN_PROJECTION;
   theGeographicOriginOfLatitude = 0.0;
   theCombinerType = "ossimImageMosaic";
   theResamplerType = "nearest neighbor";
   theTilingTemplate = "";
   theTilingFilename = "";
   theSlaveBuffers = "2";
   clearFilenameList();
   theLowPercentClip = OSSIM_DBL_NAN;
   theHighPercentClip = OSSIM_DBL_NAN;
   theCutCenter.makeNan();
   theCutDxDy.makeNan();
   theCutCenterUnit   = OSSIM_UNIT_UNKNOWN;
   theCutDxDyUnit     = OSSIM_UNIT_UNKNOWN;
}

void ossimOrthoIgen::setCombinerType(const ossimString& combinerType)
{
   theCombinerType = combinerType;
}

void ossimOrthoIgen::setProjectionName(const ossimString& projName)
{
   // for now we will only support utm or geographic for this.
   theProjectionName = projName;

   if(projName == "ossimUtmProjection")
   {
      theProjectionType = OSSIM_UTM_PROJECTION;
   }
   else if(projName == "ossimEquDistCylProjection")
   {
      theProjectionType = OSSIM_GEO_PROJECTION;
   }
}

void ossimOrthoIgen::setResamplerType(const ossimString& resamplerType)
{
   theResamplerType = resamplerType;
}

void ossimOrthoIgen::setCutCenter(const ossimDpt& dpt,
                                  ossimUnitType unit)
{
   theCutCenter     = dpt;
   theCutCenterUnit = unit;
}

void ossimOrthoIgen::setCutDxDy(const ossimDpt& dpt,
                                ossimUnitType unit)
{
   theCutDxDy     = dpt;
   theCutDxDyUnit = unit;
}

bool ossimOrthoIgen::setupIgenKwl(ossimKeywordlist& kwl)
{
   if (traceDebug())
   {
      ossimNotify(ossimNotifyLevel_DEBUG)
         << "ossimOrthoIgen::setupIgenKwl DEBUG: Entered ..."
         << std::endl;
   }
   setupTiling(kwl);
   kwl.add("igen.thumbnail",
           theThumbnailFlag,
           true);
   if((theThumbnailRes != "")&&
      theThumbnailFlag)
   {
      kwl.add("igen.thumbnail_res",
              theThumbnailRes.c_str(),
              true);
   }
   if(theSlaveBuffers == "")
   {
      kwl.add("igen.slave_tile_buffers",
              2,
              true);
   }
   else
   {
      kwl.add("igen.slave_tile_buffers",
              theSlaveBuffers.c_str(),
              true);
   }
   ossim_uint32 inputFileIdx = 0;
   if(!setupView(kwl))
   {
      if (traceDebug())
      {
         ossimNotify(ossimNotifyLevel_DEBUG)
            << "ossimOrthoIgen::execute DEBUG: setupView returned false..."
            << std::endl;
      }
       return false;
   }

   if(theFilenames.size() > 1)
   {
      ossimKeywordlist tempKwl;
      ossimRefPtr<ossimObject> obj;
      

      if(theChainTemplate.exists())
      {
         tempKwl.addFile(theChainTemplate);
         obj = ossimObjectFactoryRegistry::instance()->createObject(tempKwl);
      }
      ossimRefPtr<ossimImageChain> chain = PTR_CAST(ossimImageChain, obj.get());
      
      if(!chain)  // then create a default rendering chain
      {
         ossimImageChain*    tempChain = new ossimImageChain;
         if(theProjectionType != OSSIM_UNKNOWN_PROJECTION)
         {
            ossimImageRenderer* tempObject   = new ossimImageRenderer;
            if(tempObject->getResampler())
            {
               tempObject->getResampler()->setFilterType(theResamplerType);
            }
            if(tempObject)
            {
               tempChain->addChild(tempObject);
            }
         }
         obj   = tempChain;
         chain = tempChain;
      }
      
      ossimRefPtr<ossimConnectableObject> mosaicObject;
      if(theCombinerTemplate.exists())
      {
         tempKwl.clear();
         tempKwl.addFile(theCombinerTemplate);
      }
      mosaicObject = (ossimConnectableObject*)ossimObjectFactoryRegistry::instance()->createObject(tempKwl);
      if(!PTR_CAST(ossimImageCombiner,
                  mosaicObject.get()))
      {
         mosaicObject = 0;
      }
      if(!mosaicObject.valid())
      {
         mosaicObject = (ossimConnectableObject*)ossimObjectFactoryRegistry::instance()->createObject(theCombinerType);
      }
      if(!mosaicObject)
      {
         mosaicObject = (ossimConnectableObject*)ossimObjectFactoryRegistry::instance()->createObject(ossimString("ossimImageMosaic"));
      }
      
      if(!mosaicObject)
      {
         ossimNotify(ossimNotifyLevel_WARN)
            << "Unable to create a mosaic object" << std::endl;
         return false;
      }
      
      ossim_uint32 fileSize = theFilenames.size()-1;
      ossim_uint32 idx;
      ossim_uint32 chainIdx = 1;
      ossimRefPtr<ossimImageChain> rootChain = new ossimImageChain;
      chain->saveState(kwl, "object1.object1.");
      for(idx = inputFileIdx;idx < fileSize; ++idx)
      {
         // first lets add an input to the chain
         //
         ossimFilename input  = theFilenames[idx].theFilename;
         ossimRefPtr<ossimImageHandler> handler = ossimImageHandlerRegistry::instance()->open(input);
         ossimHistogramRemapper* histRemapper = 0;
         if(handler.valid())
         {
            std::vector<ossim_uint32> entryList;
            if(theFilenames[idx].theEntry >-1)
            {
               entryList.push_back(theFilenames[idx].theEntry);
            }
            else
            {
               handler->getEntryList(entryList);
            }

            ossim_uint32 entryIdx = 0;
            for(entryIdx = 0; entryIdx < entryList.size(); ++entryIdx)
            {
               ossimImageHandler* h = (ossimImageHandler*)handler->dup();
               h->setCurrentEntry(entryList[entryIdx]);
               ossimImageChain* tempChain = (ossimImageChain*)chain->dup();
               tempChain->addLast(h);
               
               if((theHighPercentClip!=OSSIM_DBL_NAN)&&
                  (theLowPercentClip!=OSSIM_DBL_NAN))
               {
                  ossimFilename inputHisto = handler->createHistogramFilename();
                  if(inputHisto.exists())
                  {
                     tempChain->addLast(histRemapper);
                     histRemapper = new ossimHistogramRemapper;
                     tempChain->insertRight(histRemapper, h);
                     histRemapper->openHistogram(inputHisto);
                     histRemapper->setHighNormalizedClipPoint(1.0-theHighPercentClip);
                     histRemapper->setLowNormalizedClipPoint(theLowPercentClip);
                  }
                  else
                  {
                     ossimNotify(ossimNotifyLevel_WARN)
                        <<"Histogram file " << inputHisto
                        << " not found!"
                        << std::endl;
                  }
               }
               tempChain->makeUniqueIds();
               tempChain->saveState(kwl, (ossimString("object1.object")+ossimString::toString(chainIdx)+".").c_str());
               rootChain->add(tempChain);
               ++chainIdx;
               mosaicObject->connectMyInputTo(tempChain);
            }
         }
         else if(input == "NULL_FILE")
         {
            ossimImageChain* tempChain = (ossimImageChain*)chain->dup();
            tempChain->makeUniqueIds();
            tempChain->saveState(kwl, (ossimString("object1.object")+ossimString::toString(chainIdx)+".").c_str());
            rootChain->add(tempChain);
            ++chainIdx;
            mosaicObject->connectMyInputTo(tempChain);
         }
      }
      mosaicObject->saveState(kwl, (ossimString("object1.object")+
                                    ossimString::toString(chainIdx)+".").c_str());

      ++chainIdx;

      ossimRefPtr<ossimConnectableObject> input = mosaicObject;

      // Annotation setup...
      ossimRefPtr<ossimConnectableObject> annotator =
         setupAnnotation(kwl, input.get());
      if (annotator.valid())
      {
         annotator->saveState(
            kwl,
            (ossimString("object1.object")+
             ossimString::toString(chainIdx)+".").c_str());
         input = annotator;
         ++chainIdx;
      }
      else
      {
         if (traceDebug())
         {
            ossimNotify(ossimNotifyLevel_DEBUG)
               << "ossimOrthoIgen::setupIgenKwl DEBUG:\n"
               << "setupAnnotation failed..."
               << std::endl;
         }
         // return false; Not sure if we want this to be an error???
      }
      
      ossimRefPtr<ossimConnectableObject> cutter = setupCutter(kwl,
                                                               input.get());
      
      if(cutter.valid())
      {
         cutter->saveState(kwl, (ossimString("object1.object")+
                                 ossimString::toString(chainIdx)+".").c_str());
         input = cutter;
      }
                  
      rootChain = 0;
      kwl.add("object1.type",
              "ossimImageChain",
              true);

      if(!setupWriter(kwl, input.get()))
      {
         if (traceDebug())
         {
            ossimNotify(ossimNotifyLevel_DEBUG)
               << "ossimOrthoIgen::execute DEBUG: setupWriter returned false..."
               << std::endl;
         }
         return false;
      }
      
   }

   return true;
}

ossimRefPtr<ossimConnectableObject> ossimOrthoIgen::setupCutter(
   ossimKeywordlist& kwl, ossimConnectableObject* input)
{
   ossimRefPtr<ossimConnectableObject> result;
   if(theCutDxDy.hasNans())
   {
      return result;
   }
   ossimRefPtr<ossimProjection> viewProj = ossimProjectionFactoryRegistry::instance()->createProjection(kwl, "product.projection.");
   
   if(viewProj.valid())
   {
      ossimMapProjection* mapProj = PTR_CAST(ossimMapProjection, viewProj.get());
      if(mapProj)
      {
         ossimGpt centerLatLon(theCutCenter.lat,
                               theCutCenter.lon,
                               0.0,
                               mapProj->getDatum());
//         if(theCutDxDyUnit == OSSIM_METERS)
         {
            if(!mapProj->isGeographic())
            {
               ossimDpt dxdy(ossimUnitConversionTool(theCutDxDy.x, theCutDxDyUnit).getValue(OSSIM_METERS),
                             ossimUnitConversionTool(theCutDxDy.y, theCutDxDyUnit).getValue(OSSIM_METERS));
               
               ossimEastingNorthingCutter* cutter = new ossimEastingNorthingCutter;
               ossimDpt pt, centerEn;
               mapProj->worldToLineSample(centerLatLon,
                                          pt);
               pt = ossimIpt(pt);
               mapProj->lineSampleToEastingNorthing(pt, centerEn);
               
               ossimDpt ul = centerEn + ossimDpt(-dxdy.x,
                                                 dxdy.y);
               ossimDpt lr = centerEn + ossimDpt(dxdy.x,
                                                 -dxdy.y);
               
               
               cutter->setEastingNorthingRectangle(ul, lr);
               
               result = cutter;
            }
            else
            {
               ossimDpt dxdy(ossimUnitConversionTool(theCutDxDy.x, theCutDxDyUnit).getValue(OSSIM_DEGREES),
                             ossimUnitConversionTool(theCutDxDy.y, theCutDxDyUnit).getValue(OSSIM_DEGREES));
               ossimGeoPolyCutter* cutter = new ossimGeoPolyCutter;
               ossimDpt metersPerDegree = centerLatLon.metersPerDegree();
               ossimDpt degreePerMeter;
               std::vector<ossimGpt> polygon;
               degreePerMeter.x = ((metersPerDegree.x > FLT_EPSILON)?1.0/metersPerDegree.x:0.0);
               degreePerMeter.y = ((metersPerDegree.y > FLT_EPSILON)?1.0/metersPerDegree.y:0.0);
               
               ossimGpt ul(centerLatLon.latd() + dxdy.y,
                           centerLatLon.lond() - dxdy.x);
               ossimGpt ur(centerLatLon.latd() + dxdy.y,
                           centerLatLon.lond() + dxdy.x);
               ossimGpt lr(centerLatLon.latd() - dxdy.y,
                           centerLatLon.lond() + dxdy.x);
               ossimGpt ll(centerLatLon.latd() - dxdy.y,
                           centerLatLon.lond() - dxdy.x);

               polygon.push_back(ul);
               polygon.push_back(ur);
               polygon.push_back(lr);
               polygon.push_back(ll);
               cutter->setNumberOfPolygons(1);
               cutter->setPolygon(polygon);

               result = cutter;

            }
         }
      }
   }
   if(result.valid())
   {
      result->connectMyInputTo(input);
   }
   
   return result;
}

bool ossimOrthoIgen::setupWriter(ossimKeywordlist& kwl, ossimConnectableObject* input)
{
   ossimFilename outputFilename = theFilenames[theFilenames.size()-1].theFilename;
   ossimImageFileWriter* writer = 0;
   if(theTilingFilename!="")
   {
      if(!outputFilename.isDir())
      {
         outputFilename = outputFilename.path();
      }
      outputFilename = outputFilename.dirCat(theTilingFilename);
   }

   if((theWriterTemplate == "")||
      (!theWriterTemplate.exists()))
   {
      ossimRefPtr<ossimImageFileWriter> outputObj = 0;
      if(theTilingFilename == "%SRTM%")
      {
         ossimKeywordlist kwlWriter;
         
         kwlWriter.add("type",
                       "ossimGeneralRasterWriter",
                       true);
         kwlWriter.add("byte_order",
                       "big_endian");
         outputObj = ossimImageWriterFactoryRegistry::instance()->createWriter(kwlWriter);
         outputFilename = outputFilename.path();
      }
      else if(outputFilename.ext() != "")
      {
         ossimFilename ext = outputFilename.ext();
         outputObj = ossimImageWriterFactoryRegistry::instance()->createWriterFromExtension(ext);
         if(!outputObj.valid())
         {
            outputObj = new ossimTiffWriter;
            writer = (ossimImageFileWriter*)outputObj.get();
            outputFilename.setExtension("tif");
         }
      }
      else
      {
         // just do a default just so we can set the keywords for now
         //
         outputObj = new ossimTiffWriter;
      }
      if(outputObj.valid())
      {
         writer = (ossimImageFileWriter*)outputObj.get();
         writer->setFilename(outputFilename);
         
         writer->connectMyInputTo(0, input);
         writer->saveState(kwl, "object2.");
      }
      kwl.add("object2.",
              ossimKeywordNames::FILENAME_KW,
              outputFilename.c_str(),
              true);
      
      if(!kwl.find("object2.type"))
      {
         kwl.add("object2.",
                 "type",
                 "ossimImageFileWriter",
                 true);
      }
   }

   return true;
}

bool ossimOrthoIgen::setupView(ossimKeywordlist& kwl)
{
   ossimKeywordlist tempKwl;
   if(theTemplateView != "")
   {
      tempKwl.addFile(theTemplateView);
   }
   ossimRefPtr<ossimObject> productObj = ossimObjectFactoryRegistry::instance()->createObject(tempKwl, "product.projection.");
   if(!productObj.valid())
   {
      productObj = ossimObjectFactoryRegistry::instance()->createObject(tempKwl);
      if(!PTR_CAST(ossimProjection, productObj.get()))
      {
         productObj = 0;
      }
      else
      {
         if(!theMetersPerPixelOverride.hasNans())
         {
            ossimMapProjection* mapProj = PTR_CAST(ossimMapProjection, productObj.get());

            if(mapProj&&!theMetersPerPixelOverride.hasNans())
            {
               mapProj->setMetersPerPixel(theMetersPerPixelOverride);
            }
         }
      }
   }
   if(!productObj.valid() && (theTemplateView!=""))
   {
      ossimNotify(ossimNotifyLevel_WARN)
         << "ossimOrthoIgen::setupView WARNING: File template "
         << theTemplateView << " did not have a valid view" << std::endl;
   }
   if(productObj.valid())
   {
      productObj->saveState(kwl, "product.projection.");
   }
   else
   {
      ossimKeywordlist inputGeom;
      // Open up the image.
      ossimFilename input  = theFilenames[0].theFilename;
      ossimRefPtr<ossimImageHandler> handler =
         ossimImageHandlerRegistry::instance()->open(input);
      if(!handler)
      {
         if (traceDebug())
         {
            ossimNotify(ossimNotifyLevel_DEBUG)
               << "ossimOrthoIgen::setupView DEBUG:"
               << "\nCould not open: " << input.c_str()
               << std::endl;
         }
 
         return false;
      }
      handler->getImageGeometry(inputGeom);
      
      // Get the input projection.
      ossimRefPtr<ossimProjection> inputProj =
         ossimProjectionFactoryRegistry::instance()->createProjection(
            inputGeom);
      if(inputProj.valid()&&
         (theProjectionType !=OSSIM_UNKNOWN_PROJECTION))
      {
         // Get the input resolution.
         ossimDpt metersPerPixel = inputProj->getMetersPerPixel();
         
         ossim_float64 gsd = (metersPerPixel.x+metersPerPixel.y)/2.0;
         if(!theMetersPerPixelOverride.hasNans())
         {
            gsd = theMetersPerPixelOverride.x;
         }
         
         // Get the bounding rect.
         ossimIrect rect = handler->getBoundingRect();
         
         // Create an output projection.
         ossimRefPtr<ossimMapProjection> outputProjection;
         if (theProjectionType == OSSIM_GEO_PROJECTION)
         {
            // Assuming WGS 84 for now.
            outputProjection = new ossimEquDistCylProjection();
            ossimGpt gpt(theGeographicOriginOfLatitude, 0.0);
            
            // Set the origin.
            outputProjection->setOrigin(gpt);
            // Set the resolution.
            outputProjection->setMetersPerPixel(ossimDpt(gsd, gsd));
            
            // Save the state to keyword list.
            outputProjection->saveState(kwl, "product.projection.");
         }
         else if((theProjectionType == OSSIM_UTM_PROJECTION)||
                 ((theProjectionType == OSSIM_INPUT_PROJECTION) &&
                  !PTR_CAST(ossimMapProjection, inputProj.get())))
            
         {
            // Default to UTM.
            ossimUtmProjection* utm = new ossimUtmProjection;
            ossimGpt midGpt;
            
            
            inputProj->lineSampleToWorld(ossimDpt(rect.midPoint()),
                                         midGpt);
            
            utm->setZone(midGpt);
            
            ossimDpt eastingNorthing;
            
            utm->setMetersPerPixel(ossimDpt(gsd, gsd));
            
            eastingNorthing = utm->forward(utm->origin());
            
            utm->setUlEastingNorthing(eastingNorthing);
            outputProjection = utm;
            // Set the resolution.
            outputProjection->setMetersPerPixel(ossimDpt(gsd, gsd));
            // Save the state to keyword list.
            outputProjection->saveState(kwl, "product.projection.");
         }
         else // input is a map projection so just use that if we are type INPUT
         {
            inputProj->saveState(kwl, "product.projection.");
         }
      }
   }
   return true;
}

ossimRefPtr<ossimConnectableObject> ossimOrthoIgen::setupAnnotation(
   ossimKeywordlist& kwl, ossimConnectableObject* input) const
{
   ossimRefPtr<ossimConnectableObject> result = NULL;
   
   if(theAnnotationTemplate.exists() == false)
   {
      return result;
   }
   
   ossimKeywordlist tempKwl;
   if (tempKwl.addFile(theAnnotationTemplate) == false)
   {
      return result;
   }
   
   ossimRefPtr<ossimObject> obj = ossimObjectFactoryRegistry::instance()->
      createObject(tempKwl, "object1.");
   if (obj.valid())
   {
      ossimGeoAnnotationSource* oga = PTR_CAST(ossimGeoAnnotationSource,
                                               obj.get());
      if (oga)
      {
         // Connect to input.
         oga->connectMyInputTo(input);
         
         // Set the view.
         ossimRefPtr<ossimProjection> viewProj =
            ossimProjectionFactoryRegistry::instance()->createProjection(
               kwl, "product.projection.");
         if (viewProj.valid())
         {
            oga->setProjection(viewProj.get(),
                               false); // False for doesn't own projection.
         }

         result = oga;
      }
   }
   
   return result;
}

bool ossimOrthoIgen::setupTiling(ossimKeywordlist& kwl)
{
   ossimKeywordlist tempKwl;
   ossimFilename outputFilename = theFilenames[theFilenames.size()-1].theFilename;

   if((theTilingTemplate == "")||
      (!tempKwl.addFile(theTilingTemplate)))
   {
      return false;
   }

   if(outputFilename.isDir())
   {
      if(tempKwl.find("igen.tiling.type"))
      {
         kwl.add(tempKwl);
         theTilingFilename = tempKwl.find("igen.tiling.tile_name_mask");
      }
      else if(tempKwl.find("type"))
      {
         kwl.add("igen.tiling.", tempKwl);
         theTilingFilename = tempKwl.find("tile_name_mask");
      }
   }
   else
   {
      theTilingFilename = outputFilename.file();
      if(tempKwl.find("igen.tiling.type"))
      {
         tempKwl.add("igen.tiling.tile_name_mask",
                     theTilingFilename.c_str(),
                     true);
         kwl.add(tempKwl);
      }
      else if(tempKwl.find("type"))
      {
         tempKwl.add("tile_name_mask",
                     theTilingFilename.c_str(),
                     true);
         kwl.add("igen.tiling.", tempKwl);
      }
   }
   
   return true;
}

void ossimOrthoIgen::setMetersPerPixel(const ossimDpt& mpp)
{
   theMetersPerPixelOverride = mpp;
}

void ossimOrthoIgen::setThumbnailResolution(const ossimIpt& res)
{
   std::ostringstream out;

   out << res.x << " " << res.y;

   theThumbnailRes = out.str().c_str();
}

void ossimOrthoIgen::setThumbnailFlag(bool thumbnailFlag)
{
   theThumbnailFlag = thumbnailFlag;
}
