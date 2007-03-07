#ifndef ossimNadconGridDatum_HEADER
#define ossimNadconGridDatum_HEADER

#include "ossimThreeParamDatum.h"
#include "ossimNadconGridFile.h"
#include "base/data_types/ossimDrect.h"

class ossimNadconGridDatum : public ossimThreeParamDatum
{
public:
  ossimNadconGridDatum(const ossimFilename& datumDirectory,
		       const ossimString &code, 
		       const ossimString &name,
		       const ossimEllipsoid* anEllipsoid,
		       double sigmaX,
		       double sigmaY,
		       double sigmaZ,
		       double westLongitude,
		       double eastLongitude,
		       double southLatitude,
		       double northLatitude,
		       double param1,
		       double param2,
		       double param3);

protected:
  mutable ossimDrect theConusRect;
  mutable ossimDrect theHawaiiRect;
  mutable ossimDrect theAlaskaRect;
  mutable ossimDrect theStGeorgeRect;
  mutable ossimDrect theLatStLaurenceRect;
  mutable ossimDrect theStPaulRect;
  mutable ossimDrect thePrviRect;
  
  mutable ossimNadconGridFile theLatGrid;
  mutable ossimNadconGridFile theLonGrid;
  
  mutable ossimDrect    theCurrentGridRect;
  ossimFilename theDatumDirectory;
  
  void checkGrid(const ossimDpt& latLon)const;
};

#endif
