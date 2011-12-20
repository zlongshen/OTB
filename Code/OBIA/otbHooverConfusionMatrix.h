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

#ifndef __otbHooverConfusionMatrix_h
#define __otbHooverConfusionMatrix_h

#include "itkLabelMapFilter.h"
#include "itkVariableSizeMatrix.h"

namespace otb
{
/** \class HooverConfusionMatrix
 *
 * \brief This class computes the confusion matrix from two LabelMapObject
 *
 * The confusion matrix stores the number of pixel inside the intersection between 
 * couples of region. These couples are made from a ground truth segmentation and 
 * a machine segmentation. The line number gives the index of the ground truth region. The 
 * column number gives the index of the machine segmentation region.
 */

template< class TLabelMap >
class ITK_EXPORT HooverConfusionMatrix :
    public itk::LabelMapFilter< TLabelMap, TLabelMap >
{
public:
  /** Standard class typedefs. */
  typedef HooverConfusionMatrix   Self;
  typedef itk::LabelMapFilter< TLabelMap, TLabelMap >      Superclass;
  typedef itk::SmartPointer<Self>       Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;

  /** Standard New method. */
  itkNewMacro(Self);
  /** Run-time type information (and related methods). */
  itkTypeMacro(HooverConfusionMatrix, LabelMapFilter);

  /** Some convenient typedefs. */
  typedef TLabelMap                                     InputLabelMapType;
  typedef typename InputLabelMapType::LabelObjectType   LabelObjectType;
  typedef typename InputLabelMapType::LabelVectorType   LabelVectorType;

  typedef typename LabelObjectType::LineContainerType   LineContainerType;
  typedef typename LabelObjectType::IndexType           IndexType;
  typedef typename LabelObjectType::LabelType           LabelType;
  
  typedef unsigned long                                 CoefficientType;
  typedef itk::VariableSizeMatrix<CoefficientType>      MatrixType;
  
  /** Set the ground truth label map */
  void SetGroundTruthLabelMap(const InputLabelMapType *gt);
  
  /** Set the machine segmentation label map */
  void SetMachineSegmentationLabelMap(const InputLabelMapType *ms);
  
  /** Get the ground truth label map */
  const InputLabelMapType* GetGroundTruthLabelMap();
  
  /** Get the machine segmentation label map */
  const InputLabelMapType* GetMachineSegmentationLabelMap();

  /** Get the output Hoover confusion matrix */
  MatrixType & GetHooverConfusionMatrix()
  {
    return m_Matrix;
  }

protected:
  /** Constructor */
  HooverConfusionMatrix();
  
  ~HooverConfusionMatrix() {};
  
  /** BeforeThreadedGenerateData
   *  Resize the matrix 
   */
  virtual void BeforeThreadedGenerateData();

  /** Real action of the filter : fill the line of the confusion matrix corresponding to 
   * the given label object 
   */
  virtual void ThreadedProcessLabelObject( LabelObjectType * labelObject );

private:
  
  /** number of label objects found in the ground truth (GT) label maps */
  unsigned long     m_NumberOfRegionsGT;
  
  /** number of label objects found in the machine segmentation (MS) label maps */
  unsigned long     m_NumberOfRegionsMS;
  
  /** List of labels in GT label map */
  LabelVectorType   m_LabelsGT;
  
  /** Hoover confusion matrix */
  MatrixType        m_Matrix;
};


}

#ifndef OTB_MANUAL_INSTANTIATION
#include "otbHooverConfusionMatrix.txx"
#endif

#endif
