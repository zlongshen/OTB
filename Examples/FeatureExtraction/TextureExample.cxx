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
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

#ifdef __BORLANDC__
#define ITK_LEAN_AND_MEAN
#endif

//  Software Guide : BeginCommandLineArgs
//    INPUTS: {ADS40RoiSmall.png}
//    OUTPUTS: {TextureOutput.tif}, {pretty_TextureOutput.png}
//    2 1 1
//  Software Guide : EndCommandLineArgs

#include "itkExceptionObject.h"
#include "otbImage.h"
#include "otbImageFileReader.h"
#include "otbImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"



// Software Guide : BeginLatex
//
// This example illustrates the use of the
// \doxygen{otb}{ContrastTextureFunctor}, and more generally it
// demonstrates how to compute Haralick's textural features.
// \relatedClasses
//  \begin{itemize}
//  \item \doxygen{otb}{EnergyTextureFunctor}
//  \item \doxygen{otb}{EntropyTextureFunctor}
//  \item \doxygen{otb}{InverseDifferenceMomentTextureFunctor}
//  \item \doxygen{otb}{AngularSecondMomentumTextureFunctor}
//  \item \doxygen{otb}{VarianceTextureFunctor}
//  \item \doxygen{otb}{CorrelationTextureFunctor}
//  \item \doxygen{otb}{SumAverageTextureFunctor}
//  \item \doxygen{otb}{DifferenceEntropyTextureFunctor}
//  \item \doxygen{otb}{SumEntropyTextureFunctor}
//  \item \doxygen{otb}{SumVarianceTextureFunctor}
//  \item \doxygen{otb}{DifferenceVarianceTextureFunctor}
//  \item \doxygen{otb}{InformationMeasureOfCorrelation1TextureFunctor}
//  \item \doxygen{otb}{InformationMeasureOfCorrelation2TextureFunctor}
//  \item \doxygen{otb}{ClusterShadeTextureFunctor}
//  \item \doxygen{otb}{ClusterProminenceTextureFunctor}
//  \item \doxygen{otb}{MeanTextureFunctor}
//  \end{itemize}
//
// The first step required to use this filter is to include its header file.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
#include "otbContrastTextureFunctor.h"
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// Although including the individual header file for a texture functor
// is O.K., OTB provides a single header file which will include all
// other texture functors. Since often we might want to compute
// several texture parameters, it is more efficient to use this single
// header file.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
#include "otbTextureFunctors.h"
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// The texture functors compute a texture feature for a given
// neighborhood. Since we want here to compute the texture features
// for all pixels ofthe image, we will use a filter which will apply
// the functor to every pixel in the image. This filter will take care
// of building the neighborhoods with the needed offsets for texture
// computation.
//
// Software Guide : EndLatex

// Software Guide : BeginCodeSnippet
#include "otbUnaryFunctorNeighborhoodWithOffsetImageFilter.h"
// Software Guide : EndCodeSnippet
// Software Guide : BeginLatex
//
// Please note that if you want to compute a single scalar texture
// feature for the whole image or for a whole image region, the most
// efficient way to do that is using the
// \doxygen{otb}{TextureImageFunction} class which provides the
// \code{Evaluate} and \code{EvaluateAtIndex} methods for this purpose.
//
// Software Guide : EndLatex



int main(int argc, char * argv[])
{
  // Parse command line parameters
  if ( argc != 7 )
  {
    std::cerr << "Usage: " << argv[0] << " <inputImage> ";
    std::cerr << " <outputImage> <outputRescaled> ";
    std::cerr << " <radius> <xOffset> <yOffset> ";
    std::cerr << std::endl;
    return EXIT_FAILURE;
  }

  const char* infname   = argv[1];
  const char* outfname  = argv[2];
  const char* outprettyfname  = argv[3];

  const unsigned int radius  =  static_cast<unsigned int>(atoi(argv[4]));
  const unsigned int xOffset =  static_cast<unsigned int>(atoi(argv[5]));
  const unsigned int yOffset =  static_cast<unsigned int>(atoi(argv[6]));


  typedef double PixelType;
  const int Dimension = 2;
  typedef otb::Image<PixelType,Dimension> ImageType;

  // Software Guide : BeginLatex
//
// After defining the types for the pixels and the images used in the
// example, we define the type for the texture functor. It is
// templated by the input and output pixel types.
//
// Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  typedef otb::Functor::ContrastTextureFunctor<PixelType, PixelType>
                                                                FunctorType;
  // Software Guide : EndCodeSnippet
  // Software Guide : BeginLatex
//
// The filter for computing the texture features for a complete image
// is templated by the input and output image types and, of course,
// the functor type.
//
// Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  typedef otb::UnaryFunctorNeighborhoodWithOffsetImageFilter<ImageType,
                                          ImageType, FunctorType> FilterType;
  // Software Guide : EndCodeSnippet
  typedef otb::ImageFileReader<ImageType>  ReaderType;
  typedef otb::ImageFileWriter<ImageType> WriterType;

  ReaderType::Pointer reader  = ReaderType::New();
  WriterType::Pointer writer = WriterType::New();

  reader->SetFileName(infname);
  writer->SetFileName(outfname);

    // Software Guide : BeginLatex
//
// We can now instatiate the filter.
//
// Software Guide : EndLatex

  // Software Guide : BeginCodeSnippet
  FilterType::Pointer textureFilter = FilterType::New();
  // Software Guide : EndCodeSnippet
  // Software Guide : BeginLatex
//
// The texture filter takes 2 parameters: the radius of the
// neighborhood on which the texture will be computed and the offset
// used. Texture features are bivariate statistics, that is, they are
// computed using pair of pixels. Each texture feature is defined for
// an offset defining the pixel pair.
//
// The radius parameter can be passed to the filter as a scalar
// parameter if the neighborhood is square, or as \code{SizeType} in
// any case.
//
// The offset is always an array of N values, where N is the number of
// dimensions of the image.
//
// Software Guide : EndLatex
  // Software Guide : BeginCodeSnippet
  textureFilter->SetRadius(radius);

  typedef ImageType::OffsetType OffsetType;
  OffsetType offset;
  offset[0] =  xOffset;
  offset[1] =  yOffset;

  textureFilter->SetOffset(offset);
  // Software Guide : EndCodeSnippet
  // Software Guide : BeginLatex
//
// We can now plug the pipeline and trigger the execution by calling
// the \code{Update} method of the writer.
//
// Software Guide : EndLatex
  // Software Guide : BeginCodeSnippet
  textureFilter->SetInput(reader->GetOutput());
  writer->SetInput(textureFilter->GetOutput());

  writer->Update();
  // Software Guide : EndCodeSnippet

  //  Software Guide : BeginLatex
  // Figure~\ref{fig:TEXTUREFUNCTOR} shows the result of applying
  // the contrast texture computation.
  // \begin{figure}
  // \center
  // \includegraphics[width=0.40\textwidth]{ADS40RoiSmall.eps}
  // \includegraphics[width=0.40\textwidth]{pretty_TextureOutput.eps}
  // \itkcaption[Texture Functor]{Result of applying the
  // \doxygen{otb}{ContrastTextureFunctor} to an image. From left to right :
  // original image, contrast.}
  // \label{fig:TEXTUREFUNCTOR}
  // \end{figure}
  //
  //  Software Guide : EndLatex

 // Pretty image creation for printing

  typedef otb::Image<unsigned char, Dimension>                                           OutputPrettyImageType;
  typedef otb::ImageFileWriter<OutputPrettyImageType>                                    WriterPrettyOutputType;
  typedef itk::RescaleIntensityImageFilter< ImageType, OutputPrettyImageType>      RescalerOutputType;

  RescalerOutputType::Pointer     outputRescaler     = RescalerOutputType::New();
  WriterPrettyOutputType::Pointer prettyOutputWriter = WriterPrettyOutputType::New();
  outputRescaler->SetInput( textureFilter->GetOutput() );
  outputRescaler->SetOutputMinimum(0);
  outputRescaler->SetOutputMaximum(255);
  prettyOutputWriter->SetFileName( outprettyfname );
  prettyOutputWriter->SetInput( outputRescaler->GetOutput() );

  prettyOutputWriter->Update();
  return EXIT_SUCCESS;
}
