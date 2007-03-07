//*******************************************************************
//
// LICENSE: LGPL
//
// see top level LICENSE.txt
// 
// Author: Walt Bunch
//
// Description: Nitf support class for USE00A -
// Exploitation Usability extension.
// 
//********************************************************************
// $Id: ossimNitfUse00aTag.cpp,v 1.4 2005/08/09 11:32:21 gpotts Exp $
#include <support_data/nitf/ossimNitfUse00aTag.h>

RTTI_DEF1(ossimNitfUse00aTag, "ossimNitfUse00aTag", ossimNitfRegisteredTag);

#ifndef MIN
#define MIN(a,b) ((a)<(b)?(a):(b))
#endif

ossimNitfUse00aTag::ossimNitfUse00aTag()
{
   clearFields();
}

ossimNitfUse00aTag::~ossimNitfUse00aTag()
{
}

ossimString ossimNitfUse00aTag::getRegisterTagName()const
{
   return "USE00A";
}

void ossimNitfUse00aTag::parseStream(std::istream& in)
{
   clearFields();
   
   in.read(theAngleToNorth, ANGLE_TO_NORTH_SIZE);
   in.read(theMeanGsd, MEAN_GSD_SIZE);
   in.read(theField3, FIELD3_SIZE);
   in.read(theDynamicRange, DYNAMIC_RANGE_SIZE);
   in.read(theField5, FIELD5_SIZE);
   in.read(theField6, FIELD6_SIZE);
   in.read(theField7, FIELD7_SIZE);
   in.read(theOblAng, OBL_ANG_SIZE);
   in.read(theRollAng, ROLL_ANG_SIZE);
   in.read(theField10, FIELD10_SIZE);
   in.read(theField11, FIELD11_SIZE);
   in.read(theField12, FIELD12_SIZE);
   in.read(theField13, FIELD13_SIZE);
   in.read(theField14, FIELD14_SIZE);
   in.read(theField15, FIELD15_SIZE);
   in.read(theField16, FIELD16_SIZE);
   in.read(theNRef, N_REF_SIZE);
   in.read(theRevNum, REV_NUM_SIZE);
   in.read(theNSeg, N_SEG_SIZE);
   in.read(theMaxLpSeg, MAX_LP_SEG_SIZE);
   in.read(theField20, FIELD20_SIZE);
   in.read(theField21, FIELD21_SIZE);
   in.read(theSunEl, SUN_EL_SIZE);
   in.read(theSunAz, SUN_AZ_SIZE);
}

void ossimNitfUse00aTag::writeStream(std::ostream& out)
{
   out.write(theAngleToNorth, ANGLE_TO_NORTH_SIZE);
   out.write(theMeanGsd, MEAN_GSD_SIZE);
   out.write(theField3, FIELD3_SIZE);
   out.write(theDynamicRange, DYNAMIC_RANGE_SIZE);
   out.write(theField5, FIELD5_SIZE);
   out.write(theField6, FIELD6_SIZE);
   out.write(theField7, FIELD7_SIZE);
   out.write(theOblAng, OBL_ANG_SIZE);
   out.write(theRollAng, ROLL_ANG_SIZE);
   out.write(theField10, FIELD10_SIZE);
   out.write(theField11, FIELD11_SIZE);
   out.write(theField12, FIELD12_SIZE);
   out.write(theField13, FIELD13_SIZE);
   out.write(theField14, FIELD14_SIZE);
   out.write(theField15, FIELD15_SIZE);
   out.write(theField16, FIELD16_SIZE);
   out.write(theNRef, N_REF_SIZE);
   out.write(theRevNum, REV_NUM_SIZE);
   out.write(theNSeg, N_SEG_SIZE);
   out.write(theMaxLpSeg, MAX_LP_SEG_SIZE);
   out.write(theField20, FIELD20_SIZE);
   out.write(theField21, FIELD21_SIZE);
   out.write(theSunEl, SUN_EL_SIZE);
   out.write(theSunAz, SUN_AZ_SIZE);
}

ossim_uint32 ossimNitfUse00aTag::getSizeInBytes()const
{
   return 107;
}

void ossimNitfUse00aTag::clearFields()
{
   memset(theAngleToNorth, ' ', ANGLE_TO_NORTH_SIZE);
   memset(theMeanGsd, ' ', MEAN_GSD_SIZE);
   memset(theField3, ' ', FIELD3_SIZE);
   memset(theDynamicRange, ' ', DYNAMIC_RANGE_SIZE);
   memset(theField5, ' ', FIELD5_SIZE);
   memset(theField6, ' ', FIELD6_SIZE);
   memset(theField7, ' ', FIELD7_SIZE);
   memset(theOblAng, ' ', OBL_ANG_SIZE);
   memset(theRollAng, ' ', ROLL_ANG_SIZE);
   memset(theField10, ' ', FIELD10_SIZE);
   memset(theField11, ' ', FIELD11_SIZE);
   memset(theField12, ' ', FIELD12_SIZE);
   memset(theField13, ' ', FIELD13_SIZE);
   memset(theField14, ' ', FIELD14_SIZE);
   memset(theField15, ' ', FIELD15_SIZE);
   memset(theField16, ' ', FIELD16_SIZE);
   memset(theNRef, ' ', N_REF_SIZE);
   memset(theRevNum, ' ', REV_NUM_SIZE);
   memset(theNSeg, ' ', N_SEG_SIZE);
   memset(theMaxLpSeg, ' ', MAX_LP_SEG_SIZE);
   memset(theField20, ' ', FIELD20_SIZE);
   memset(theField21, ' ', FIELD21_SIZE);
   memset(theSunEl, ' ', SUN_EL_SIZE);
   memset(theSunAz, ' ', SUN_AZ_SIZE);

   theAngleToNorth[ANGLE_TO_NORTH_SIZE] = '\0';
   theMeanGsd[MEAN_GSD_SIZE] = '\0';
   theField3[FIELD3_SIZE] = '\0';
   theDynamicRange[DYNAMIC_RANGE_SIZE] = '\0';
   theField5[FIELD5_SIZE] = '\0';
   theField6[FIELD6_SIZE] = '\0';
   theField7[FIELD7_SIZE] = '\0';
   theOblAng[OBL_ANG_SIZE] = '\0';
   theRollAng[ROLL_ANG_SIZE] = '\0';
   theField10[FIELD10_SIZE] = '\0';
   theField11[FIELD11_SIZE] = '\0';
   theField12[FIELD12_SIZE] = '\0';
   theField13[FIELD13_SIZE] = '\0';
   theField14[FIELD14_SIZE] = '\0';
   theField15[FIELD15_SIZE] = '\0';
   theField16[FIELD16_SIZE] = '\0';
   theNRef[N_REF_SIZE] = '\0';
   theRevNum[REV_NUM_SIZE] = '\0';
   theNSeg[N_SEG_SIZE] = '\0';
   theMaxLpSeg[MAX_LP_SEG_SIZE] = '\0';
   theField20[FIELD20_SIZE] = '\0';
   theField21[FIELD21_SIZE] = '\0';
   theSunEl[SUN_EL_SIZE] = '\0';
   theSunAz[SUN_AZ_SIZE] = '\0';
}

ossimString ossimNitfUse00aTag::getAngleToNorth()const
{
   return theAngleToNorth;
}

void ossimNitfUse00aTag::setAngleToNorth(ossimString angleToNorth)
{
   memset(theAngleToNorth, ' ', ANGLE_TO_NORTH_SIZE);
   memcpy(theAngleToNorth, angleToNorth.c_str(), MIN(ANGLE_TO_NORTH_SIZE, angleToNorth.length()));
}

ossimString ossimNitfUse00aTag::getMeanGsd()const
{
   return theMeanGsd;
}

void ossimNitfUse00aTag::setMeanGsd(ossimString meanGsd)
{
   memset(theMeanGsd, ' ', MEAN_GSD_SIZE);
   memcpy(theMeanGsd, meanGsd.c_str(), MIN(MEAN_GSD_SIZE, meanGsd.length()));
}

ossimString ossimNitfUse00aTag::getField3()const
{
   return theField3;
}

void ossimNitfUse00aTag::setField3(ossimString field3)
{
   memset(theField3, ' ', FIELD3_SIZE);
   memcpy(theField3, field3.c_str(), MIN(FIELD3_SIZE, field3.length()));
}

ossimString ossimNitfUse00aTag::getDynamicRange()const
{
   return theDynamicRange;
}

void ossimNitfUse00aTag::setDynamicRange(ossimString dynamicRange)
{
   memset(theDynamicRange, ' ', DYNAMIC_RANGE_SIZE);
   memcpy(theDynamicRange, dynamicRange.c_str(), MIN(DYNAMIC_RANGE_SIZE, dynamicRange.length()));
}

ossimString ossimNitfUse00aTag::getField5()const
{
   return theField5;
}

void ossimNitfUse00aTag::setField5(ossimString field5)
{
   memset(theField5, ' ', FIELD5_SIZE);
   memcpy(theField5, field5.c_str(), MIN(FIELD5_SIZE, field5.length()));
}

ossimString ossimNitfUse00aTag::getField6()const
{
   return theField6;
}

void ossimNitfUse00aTag::setField6(ossimString field6)
{
   memset(theField6, ' ', FIELD6_SIZE);
   memcpy(theField6, field6.c_str(), MIN(FIELD6_SIZE, field6.length()));
}

ossimString ossimNitfUse00aTag::getField7()const
{
   return theField7;
}

void ossimNitfUse00aTag::setField7(ossimString field7)
{
   memset(theField7, ' ', FIELD7_SIZE);
   memcpy(theField7, field7.c_str(), MIN(FIELD7_SIZE, field7.length()));
}

ossimString ossimNitfUse00aTag::getOblAng()const
{
   return theOblAng;
}

void ossimNitfUse00aTag::setOblAng(ossimString oblAng)
{
   memset(theOblAng, ' ', OBL_ANG_SIZE);
   memcpy(theOblAng, oblAng.c_str(), MIN(OBL_ANG_SIZE, oblAng.length()));
}

ossimString ossimNitfUse00aTag::getRollAng()const
{
   return theRollAng;
}

void ossimNitfUse00aTag::setRollAng(ossimString rollAng)
{
   memset(theRollAng, ' ', ROLL_ANG_SIZE);
   memcpy(theRollAng, rollAng.c_str(), MIN(ROLL_ANG_SIZE, rollAng.length()));
}

ossimString ossimNitfUse00aTag::getField10()const
{
   return theField10;
}

void ossimNitfUse00aTag::setField10(ossimString field10)
{
   memset(theField10, ' ', FIELD10_SIZE);
   memcpy(theField10, field10.c_str(), MIN(FIELD10_SIZE, field10.length()));
}

ossimString ossimNitfUse00aTag::getField11()const
{
   return theField11;
}

void ossimNitfUse00aTag::setField11(ossimString field11)
{
   memset(theField11, ' ', FIELD11_SIZE);
   memcpy(theField11, field11.c_str(), MIN(FIELD11_SIZE, field11.length()));
}

ossimString ossimNitfUse00aTag::getField12()const
{
   return theField12;
}

void ossimNitfUse00aTag::setField12(ossimString field12)
{
   memset(theField12, ' ', FIELD12_SIZE);
   memcpy(theField12, field12.c_str(), MIN(FIELD12_SIZE, field12.length()));
}

ossimString ossimNitfUse00aTag::getField13()const
{
   return theField13;
}

void ossimNitfUse00aTag::setField13(ossimString field13)
{
   memset(theField13, ' ', FIELD13_SIZE);
   memcpy(theField13, field13.c_str(), MIN(FIELD13_SIZE, field13.length()));
}

ossimString ossimNitfUse00aTag::getField14()const
{
   return theField14;
}

void ossimNitfUse00aTag::setField14(ossimString field14)
{
   memset(theField14, ' ', FIELD14_SIZE);
   memcpy(theField14, field14.c_str(), MIN(FIELD14_SIZE, field14.length()));
}

ossimString ossimNitfUse00aTag::getField15()const
{
   return theField15;
}

void ossimNitfUse00aTag::setField15(ossimString field15)
{
   memset(theField15, ' ', FIELD15_SIZE);
   memcpy(theField15, field15.c_str(), MIN(FIELD15_SIZE, field15.length()));
}

ossimString ossimNitfUse00aTag::getField16()const
{
   return theField16;
}

void ossimNitfUse00aTag::setField16(ossimString field16)
{
   memset(theField16, ' ', FIELD16_SIZE);
   memcpy(theField16, field16.c_str(), MIN(FIELD16_SIZE, field16.length()));
}

ossimString ossimNitfUse00aTag::getNRef()const
{
   return theNRef;
}

void ossimNitfUse00aTag::setNRef(ossimString nRef)
{
   memset(theNRef, ' ', N_REF_SIZE);
   memcpy(theNRef, nRef.c_str(), MIN(N_REF_SIZE, nRef.length()));
}

ossimString ossimNitfUse00aTag::getRevNum()const
{
   return theRevNum;
}

void ossimNitfUse00aTag::setRevNum(ossimString revNum)
{
   memset(theRevNum, ' ', REV_NUM_SIZE);
   memcpy(theRevNum, revNum.c_str(), MIN(REV_NUM_SIZE, revNum.length()));
}

ossimString ossimNitfUse00aTag::getNSeg()const
{
   return theNSeg;
}

void ossimNitfUse00aTag::setNSeg(ossimString nSeg)
{
   memset(theNSeg, ' ', N_SEG_SIZE);
   memcpy(theNSeg, nSeg.c_str(), MIN(N_SEG_SIZE, nSeg.length()));
}

ossimString ossimNitfUse00aTag::getMaxLpSeg()const
{
   return theMaxLpSeg;
}

void ossimNitfUse00aTag::setMaxLpSeg(ossimString maxLpSeg)
{
   memset(theMaxLpSeg, ' ', MAX_LP_SEG_SIZE);
   memcpy(theMaxLpSeg, maxLpSeg.c_str(), MIN(MAX_LP_SEG_SIZE, maxLpSeg.length()));
}

ossimString ossimNitfUse00aTag::getField20()const
{
   return theField20;
}

void ossimNitfUse00aTag::setField20(ossimString field20)
{
   memset(theField20, ' ', FIELD20_SIZE);
   memcpy(theField20, field20.c_str(), MIN(FIELD20_SIZE, field20.length()));
}

ossimString ossimNitfUse00aTag::getField21()const
{
   return theField21;
}

void ossimNitfUse00aTag::setField21(ossimString field21)
{
   memset(theField21, ' ', FIELD21_SIZE);
   memcpy(theField21, field21.c_str(), MIN(FIELD21_SIZE, field21.length()));
}

ossimString ossimNitfUse00aTag::getSunEl()const
{
   return theSunEl;
}

void ossimNitfUse00aTag::setSunEl(ossimString sunEl)
{
   memset(theSunEl, ' ', SUN_EL_SIZE);
   memcpy(theSunEl, sunEl.c_str(), MIN(SUN_EL_SIZE, sunEl.length()));
}

ossimString ossimNitfUse00aTag::getSunAz()const
{
   return theSunAz;
}

void ossimNitfUse00aTag::setSunAz(ossimString sunAz)
{
   memset(theSunAz, ' ', SUN_AZ_SIZE);
   memcpy(theSunAz, sunAz.c_str(), MIN(SUN_AZ_SIZE, sunAz.length()));
}

