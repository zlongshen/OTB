/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.


     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/


#include "itkMacro.h"
#include <iostream>

#include "otbVectorImage.h"
#include "otbReciprocalCovarianceToCoherencyDegreeImageFilter.h"

int otbReciprocalCovarianceToCoherencyDegreeImageFilterNew(int itkNotUsed(argc), char * itkNotUsed(argv)[])
{
  const unsigned int Dimension = 2;

  typedef std::complex<double>   PixelType;
  typedef otb::VectorImage<PixelType, Dimension> ImageType;


  typedef otb::ReciprocalCovarianceToCoherencyDegreeImageFilter<ImageType, ImageType> FilterType;

  FilterType::Pointer filter = FilterType::New();

  return EXIT_SUCCESS;
}
