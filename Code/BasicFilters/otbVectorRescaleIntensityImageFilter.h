/*=========================================================================

  Program:   ORFEO Toolbox
  Language:  C++
  Date:      $Date$
  Version:   $Revision$


  Copyright (c) Centre National d'Etudes Spatiales. All rights reserved.
  See OTBCopyright.txt for details.

  Some parts of this code are derived from ITK. See ITKCopyright.txt
  for details.


     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __otbVectorRescaleIntensityImageFilter_h
#define __otbVectorRescaleIntensityImageFilter_h

#include "itkUnaryFunctorImageFilter.h"
#include "itkVariableLengthVector.h"

namespace otb
{
namespace Functor {  
  /** \class VectorAffineTransform
   *  \brief This functor performs a per band linear transform of its input.
   *  
   *  TInput and TOutput type are supposed to be of type itk::VariableLengthVector.
   *
   */ 
  template< typename TInput, typename  TOutput>
    class VectorAffineTransform
    {
    public:
      /// Real type typedef
      typedef typename itk::NumericTraits< typename TInput::ValueType >::RealType RealType;

      /// Constructor
      VectorAffineTransform() {}
      /// Desctructor
      ~VectorAffineTransform() {}

      /// Accessors
      void SetOutputMaximum( TOutput a ) { m_OutputMaximum = a;}
      void SetOutputMinimum( TOutput a) {m_OutputMinimum = a;}
      void SetInputMinimum(TInput a) {m_InputMinimum = a;}
      void SetInputMaximum(TInput a) {m_InputMaximum = a;}
      TOutput GetOutputMaximum(){return m_OutputMaximum;}
      TOutput GetOutputMinimum(){return m_OutputMinimum;}
      TInput GetInputMinimum(){return m_InputMinimum;}
      TInput GetInputMaximum(){return m_InputMaximum;}
      
     
      bool operator!=( const VectorAffineTransform & other ) const
	{
	  if(m_OutputMaximum.Size()==other.GetOutputMaximum().Size())
	    {
	      for(unsigned int i = 0;i<m_OutputMaximum.Size();++i)
		{
		  if( m_OutputMaximum[i] != other.GetOutputMaximum()[i] )
		    {
		      return true;
		    }
		}
	    }
	  if(m_OutputMaximum.Size()==other.GetOutputMaximum().Size())
	    {
	      for(unsigned int i = 0;i<m_OutputMaximum.Size();++i)
		{
		  if( m_OutputMaximum[i] != other.GetOutputMaximum()[i] )
		    {
		      return true;
		    }
		}
	    }
	  if(m_InputMinimum.Size()==other.GetInputMinimum().Size())
	    {
	      for(unsigned int i = 0;i<m_InputMinimum.Size();++i)
		{
		  if( m_InputMinimum[i] != other.GetInputMinimum()[i] )
		    {
		      return true;
		    }
		}
	    }
	  if(m_InputMaximum.Size()==other.GetInputMaximum().Size())
	    {
	      for(unsigned int i = 0;i<m_InputMaximum.Size();++i)
		{
		  if( m_InputMaximum[i] != other.GetInputMaximum()[i] )
		    {
		      return true;
		    }
		}
	    }
	  return false;
	}
      bool operator==( const VectorAffineTransform & other ) const
	{
	  return !(*this != other);
	}

      // main computation method
      inline TOutput operator()( const TInput & x )
	{
	  // output instantiation
	  TOutput  result;
	  result.SetSize(x.GetSize());

	  // consistency checking
	  if(   result.GetSize()!=m_OutputMaximum.GetSize() 
	     || result.GetSize() != m_OutputMaximum.GetSize() 
	     || result.GetSize() != m_InputMinimum.GetSize()
	     || result.GetSize() != m_InputMaximum.GetSize())
	    {
	      itkGenericExceptionMacro(<<"Pixel size different from scale or shift size !");
	    }
	  
	  // transformation
	  for(unsigned int i=0; i<x.GetSize();++i)
	    {
	      if(x[i]<m_InputMinimum[i])
		{
		  result[i] = m_OutputMinimum[i];
		}
	      else if (x[i]>m_InputMaximum[i])
		{
		  result[i] = m_OutputMaximum[i];
		}
	      else if(m_InputMaximum[i]==m_InputMinimum[i])
		{
		  result[i]=m_OutputMinimum[i];
		}
	      else
		{
		  const RealType scaledComponent = static_cast<RealType>( x[i]-m_InputMinimum[i] ) 
		    * static_cast<RealType> (m_OutputMaximum[i] - m_OutputMinimum[i]) 
		    / static_cast<RealType> (m_InputMaximum[i] - m_InputMinimum[i]);
		  result[i]= static_cast< typename TOutput::ValueType >( scaledComponent+m_OutputMinimum[i] );
		}
	    }
	  return result;
	}
    private:
      TOutput m_OutputMaximum;
      TOutput m_OutputMinimum;
      TInput m_InputMinimum;
      TInput m_InputMaximum;      
    }; 
}  // end namespace functor


/** \class VectorRescaleIntensityImageFilter
 *  \brief
 *
 *  \ingroup IntensityImageFilters
 *  \ingroup MultiThreaded
 */
template <class TInputImage, class TOutputImage=TInputImage>
class ITK_EXPORT VectorRescaleIntensityImageFilter 
  :  public itk::UnaryFunctorImageFilter<TInputImage,TOutputImage, 
                                    Functor::VectorAffineTransform< 
                                    typename TInputImage::PixelType, 
                                    typename TOutputImage::PixelType>   >
{
  public:
  /** Standard class typedefs. */
  typedef VectorRescaleIntensityImageFilter  Self;
  typedef itk::UnaryFunctorImageFilter<TInputImage,TOutputImage, 
  Functor::VectorAffineTransform< 
  typename TInputImage::PixelType, 
  typename TOutputImage::PixelType> >  Superclass;
  typedef itk::SmartPointer<Self>   Pointer;
  typedef itk::SmartPointer<const Self>  ConstPointer;
  
  typedef typename TOutputImage::PixelType OutputPixelType;
  typedef typename TInputImage::PixelType  InputPixelType;
  typedef typename InputPixelType::ValueType      InputValueType;
  typedef typename OutputPixelType::ValueType     OutputValueType;
  typedef typename itk::NumericTraits<InputValueType>::RealType InputRealType;
  typedef typename itk::NumericTraits<OutputValueType>::RealType OutputRealType;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  itkSetMacro( OutputMaximum, OutputPixelType );
  itkGetConstReferenceMacro( OutputMaximum, OutputPixelType );
  itkSetMacro( OutputMinimum, OutputPixelType );
  itkGetConstReferenceMacro( OutputMinimum, OutputPixelType );
  itkSetMacro(ClampThreshold,double);
  itkGetMacro(ClampThreshold,double);
  
  /** Process to execute before entering the multithreaded section */
  void BeforeThreadedGenerateData(void);
  
  /** Generate output information */
  void GenerateOutputInformation(void);
  
  /** Generate input requested region */
  void GenerateInputRequestedRegion(void);

/** Print internal ivars */
void PrintSelf(std::ostream& os, itk::Indent indent) const;

protected:
  VectorRescaleIntensityImageFilter();
  virtual ~VectorRescaleIntensityImageFilter() {};

private:
  VectorRescaleIntensityImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  OutputPixelType        m_OutputMinimum;
  OutputPixelType        m_OutputMaximum;
  double                 m_ClampThreshold;

};
  
} // end namespace otb
  
#ifndef OTB_MANUAL_INSTANTIATION
#include "otbVectorRescaleIntensityImageFilter.txx"
#endif
  
#endif
