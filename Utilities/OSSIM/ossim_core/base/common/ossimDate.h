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
// Author: Garrett Potts (gpotts@imagelinks)
//
//*************************************************************************
// $Id: ossimDate.h,v 1.12 2005/07/13 12:31:06 gpotts Exp $
#ifndef ossimDate_HEADER
#define ossimDate_HEADER
#include <iostream>
#include <ctime>
#include <base/common/ossimConstants.h>

class OSSIMDLLEXPORT ossimLocalTm : public std::tm
{
public:
   friend  std::ostream& operator<< (std::ostream& out, const ossimLocalTm& src);
   friend int operator== (const ossimLocalTm& t1, const ossimLocalTm& src);
   friend int operator!= (const ossimLocalTm& t1, const ossimLocalTm& src);
   friend int operator<  (const ossimLocalTm& t1, const ossimLocalTm& src);
   friend int operator<= (const ossimLocalTm& t1, const ossimLocalTm& src);
   friend int operator>  (const ossimLocalTm& t1, const ossimLocalTm& src);
   friend int operator>=  (const ossimLocalTm& t1, const ossimLocalTm& src);

   ossimLocalTm (time_t t=0);     // Set to time, 0 is magic for 'now'
   ossimLocalTm (tm const & t); // Copy constructor
   
   ossimLocalTm & operator= (tm const & t);            // Assignment
   operator time_t (void) const;        // Conversion operator
   
   int isValid (void) const;             // Test for validity
   
   int compare (ossimLocalTm const & t) const;      // Compare times
   int compare (std::time_t const tt) const;       // Compare times
   
   enum ossimLocalTmFormat     // Date format flags
   {
      ossimLocalTmFormatYear          = (int)0x0001,            // Print year
      ossimLocalTmFormatYearShort     = (int)0x0002, // Print last two digits
      ossimLocalTmFormatPadYear       = (int)0x0004,    // Pad year to 2 or 4
      ossimLocalTmFormatZeroYear      = (int)0x0008,        // Zero fill year
      ossimLocalTmFormatMonth         = (int)0x0010,           // Print month
      ossimLocalTmFormatMonText       = (int)0x0020,   // Print month in text
      ossimLocalTmFormatPadMon        = (int)0x0040, // Pad to 2 (trunc to 3)
      ossimLocalTmFormatZeroMon       = (int)0x0080,       // Zero fill month
      ossimLocalTmFormatDay           = (int)0x0100,            // Print date
      ossimLocalTmFormatDayOfWeek     = (int)0x0200,     // Print day of week
      ossimLocalTmFormatPadDay        = (int)0x0400,         // Pad date to 2
      ossimLocalTmFormatZeroDay       = (int)0x0800,         // Zero fill day
      ossimLocalTmFormatDMY           = (int)0x0111, // Print date, mth, year
      ossimLocalTmFormatPadDMY        = (int)0x0444,         // Pad all three
      ossimLocalTmFormatZeroDMY       = (int)0x0888,   // Zero fill all three
      ossimLocalTmFormatYearFirst     = (int)0x1000,      // Print year first
      ossimLocalTmFormatMonFirst      = (int)0x2000,     // Print month first
      ossimLocalTmFormatSepChar       = (int)0x4000,// Separate fields datech
      ossimLocalTmFormatSepSpace      = (int)0x8000,  // Separate fields space
      ossimLocalTmFormatInternational = (int)(ossimLocalTmFormatDMY|ossimLocalTmFormatMonText|ossimLocalTmFormatPadMon|
                                                ossimLocalTmFormatSepSpace),
      ossimLocalTmFormatIntlShort     = (int)(ossimLocalTmFormatDMY|ossimLocalTmFormatMonText|ossimLocalTmFormatPadMon|
                                                ossimLocalTmFormatSepSpace|ossimLocalTmFormatYearShort),
      ossimLocalTmFormatUsa           = (int)(ossimLocalTmFormatDMY|ossimLocalTmFormatMonFirst|ossimLocalTmFormatPadDMY|
                                                ossimLocalTmFormatZeroDMY|ossimLocalTmFormatYearShort|ossimLocalTmFormatSepChar),
      ossimLocalTmFormatEnglish       = (int)(ossimLocalTmFormatDMY|ossimLocalTmFormatYearShort|ossimLocalTmFormatPadDMY|
                                                ossimLocalTmFormatZeroDMY|ossimLocalTmFormatSepChar),
      ossimLocalTmFormatJapanese      = (int)(ossimLocalTmFormatDMY|ossimLocalTmFormatYearFirst|ossimLocalTmFormatPadDMY|
                                                ossimLocalTmFormatZeroDMY|ossimLocalTmFormatYearShort|ossimLocalTmFormatMonFirst|
                                                ossimLocalTmFormatSepChar),
      ossimLocalTmFormatFull          = (int)(ossimLocalTmFormatDMY|ossimLocalTmFormatDayOfWeek|ossimLocalTmFormatMonText|
                                                ossimLocalTmFormatSepSpace)
   };
   
   enum ossimTimeFormat  // Time format (nb: time zones not implemented)
      {
         ossimTimeFormatSecs          = (int)0x0001,         // Print seconds
         ossimTimeFormatZeroSecs      = (int)0x0002,     // Zero fill seconds
         ossimTimeFormatPadSecs       = (int)0x0004,      // Pad seconds to 2
         ossimTimeFormatSecsAll       = (int)0x0007,
         ossimTimeFormatTimeZone      = (int)0x0008,        // Print timezone
         ossimTimeFormatMins          = (int)0x0010,         // Print minutes
         ossimTimeFormatZeroMins      = (int)0x0020,     // Zero fill minutes
         ossimTimeFormatPadMins       = (int)0x0040,      // Pad minutes to 2
         ossimTimeFormatMinsAll       = (int)0x0070,
         ossimTimeFormatTZNumeric     = (int)0x0080,      // Print numeric TZ
         ossimTimeFormatHour          = (int)0x0100,            // Print hour
         ossimTimeFormatZeroHour      = (int)0x0200,        // Zero fill hour
         ossimTimeFormatPadHour       = (int)0x0400,  // Pad hour to 2 digits
         ossimTimeFormatHourAll       = (int)0x0700,
         ossimTimeFormat24hour        = (int)0x0800,          // 24hour clock
         ossimTimeFormatSepChar       = (int)0x1000, // Separate field timech
         ossimTimeFormatSepSpace      = (int)0x2000, // Separate fields space
         ossimTimeFormatSepAbbrev     = (int)0x4000,     // Add abbreviations
         ossimTimeFormatAmPm          = (int)0x8000,       // Add 'am' or 'pm'
         ossimTimeFormatInternational = (int)(ossimTimeFormatHourAll|ossimTimeFormatMinsAll|ossimTimeFormatSecsAll|
                                              ossimTimeFormat24hour|ossimTimeFormatSepChar),
         ossimTimeFormatShortTime     = (int)(ossimTimeFormatHourAll|ossimTimeFormatMinsAll|ossimTimeFormat24hour|
                                              ossimTimeFormatSepChar),
         ossimTimeFormatClockTime     = (int)(ossimTimeFormatHour|ossimTimeFormatPadHour|ossimTimeFormatMinsAll|
                                              ossimTimeFormatAmPm|ossimTimeFormatSepChar),
         ossimTimeFormatLongTime      = (int)(ossimTimeFormatHour|ossimTimeFormatPadHour|ossimTimeFormatMinsAll|
                                              ossimTimeFormatSecsAll|ossimTimeFormatSepAbbrev|ossimTimeFormatSepSpace),
         ossimTimeFormatMillitary     = (int)(ossimTimeFormatHourAll|ossimTimeFormatMinsAll|ossimTimeFormat24hour)
      };
   
   
   static char timech;    // Character used for time separator
   static char datech;    // Character used for date separator
   static int datefmt;                  // Default date format
   static int timefmt;                  // Default time format
   
   // Output methods
   std::ostream& print(std::ostream & os,
                       int df =datefmt,
                       int tf =timefmt) const;
   std::ostream& printTime(std::ostream & os, int f =timefmt) const;
   std::ostream& printDate(std::ostream & os, int f =datefmt) const;


    int getYear()const;

    int getShortYear()const;
    
    /*!
     * This is one based i.e. values are returned between 1-12
     */
    int getMonth()const;
    
    /*!
     * returns the day 1-31
     */
    int getDay()const;
    double getJulian()const;
    double getModifiedJulian()const;

    ossimLocalTm& setDay(int day);
   /*!
    * The month is a number bewteen 1 and 12.  We will shift it to the internal
    * representation
    */
    ossimLocalTm& setMonth(int month);
    ossimLocalTm& setYear(int year);
    ossimLocalTm& setDateFromJulian(double jd);
    ossimLocalTm& setDateFromModifiedJulian(double mjd);

    void setFractionalDay(double fractionalDay);
    static void extractHmsFromFractionalDay(double fractionalDay,
                                            int &h,
                                            int &m,
                                            int &s);
    int getHour()const;
    int getMin()const;
    int getSec()const;
    ossimLocalTm& setHour(int h);
    ossimLocalTm& setMin(int m);
    ossimLocalTm& setSec(int s);
    

    
 protected:
   // Time suffix
   void tSfx(std::ostream & os, int fmt, char ch) const;
   // Time field formatters
   void pHour(std::ostream & os, int fmt) const;
   void pMins(std::ostream & os, int fmt) const;
   void pSecs(std::ostream & os, int fmt) const;
   // Date suffix
   void dSfx(std::ostream & os, int fmt) const;
   // Date field formatters
   void pDate(std::ostream & os, int fmt) const;
   void pMonth(std::ostream & os, int fmt) const;
   void pYear(std::ostream & os, int fmt) const;
   
};


class OSSIMDLLEXPORT ossimDate : public ossimLocalTm
{
  public:
   friend std::ostream& operator<<(std::ostream& out, const ossimDate& src);

   ossimDate(int datefmt =ossimLocalTm::datefmt)
      :ossimLocalTm(0), _fmt(datefmt)
      {}
   ossimDate (ossimLocalTm const & t,
              int dtfmt =ossimLocalTm::datefmt)
      : ossimLocalTm (t), _fmt(dtfmt)
      {}
    ossimDate (time_t t, int dtfmt =ossimLocalTm::datefmt)
        : ossimLocalTm (t), _fmt(dtfmt)
       {}
   ossimDate(int month, int day, int year,int dtfmt=ossimLocalTm::datefmt)
         :ossimLocalTm (0), _fmt(dtfmt)
      {
         setMonth(month);
         setDay(day);
         setYear(year);
      }
    int fmt(int f)                 {   return _fmt = f;    }
    int fmt(void) const            {   return _fmt;        }

    std::ostream & print (std::ostream & os) const;
   
   
  private:

    int _fmt;

};

class OSSIMDLLEXPORT ossimTime :public ossimLocalTm
{
 public:
   friend std::ostream& operator <<(std::ostream& out, const ossimTime& src);
   ossimTime(int tmfmt =ossimLocalTm::timefmt);
   ossimTime (ossimTime const & t,
              int tmfmt =ossimLocalTm::timefmt);
   ossimTime (time_t t, int tmfmt =ossimLocalTm::timefmt);
   
   int fmt (int f);
   int fmt (void) const;
   
   std::ostream& print (std::ostream & os) const;
   
private:
      int _fmt;   
};
#endif
