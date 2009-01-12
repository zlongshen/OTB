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
#ifndef __otbVectorData_txx
#define __otbVectorData_txx

#include "otbVectorData.h"
#include "itkPreOrderTreeIterator.h"
#include "otbMetaDataKey.h"

namespace otb
{

template <class TPrecision, unsigned int VDimension>
VectorData<TPrecision,VDimension>
::VectorData()
{
  m_DataTree = DataTreeType::New();
  DataNodePointerType root = DataNodeType::New();
  root->SetNodeId("Root");
  m_DataTree->SetRoot(root);
}

template<class TPrecision, unsigned int VDimension>
    void
        VectorData<TPrecision,VDimension>
  ::SetProjectionRef(std::string projectionRef)
{
  itk::MetaDataDictionary & dict = this->GetMetaDataDictionary();

  itk::EncapsulateMetaData<std::string>(dict, MetaDataKey::ProjectionRefKey, projectionRef );
  this->Modified();
}

template<class TPrecision, unsigned int VDimension>
    std::string
        VectorData<TPrecision,VDimension>
  ::GetProjectionRef() const
{
  const itk::MetaDataDictionary & dict = this->GetMetaDataDictionary();

  std::string projectionRef;
  itk::ExposeMetaData<std::string>(dict, MetaDataKey::ProjectionRefKey, projectionRef );

  return projectionRef;
}

template <class TPrecision, unsigned int VDimension>
bool
VectorData<TPrecision,VDimension>
::Clear()
{
  return m_DataTree->Clear();
}

template <class TPrecision, unsigned int VDimension>
    int
        VectorData<TPrecision,VDimension>
  ::Size() const
{
  return m_DataTree->Count();
}

template <class TPrecision, unsigned int VDimension>
void
VectorData<TPrecision,VDimension>
::PrintSelf(std::ostream& os, itk::Indent indent) const
{
  Superclass::PrintSelf(os,indent);
  os<<std::endl;

  itk::PreOrderTreeIterator<DataTreeType> it(m_DataTree);
  it.GoToBegin();

  while(!it.IsAtEnd())
    {
      itk::PreOrderTreeIterator<DataTreeType> itParent = it;
      bool goesOn = true;
      while(itParent.HasParent() && goesOn )
	{
	  os<<indent;
	  goesOn = itParent.GoToParent();
	}
      os<<"+"<<it.Get()->GetNodeTypeAsString()<<std::endl;
      ++it;
    }
}
} // end namespace otb

#endif
