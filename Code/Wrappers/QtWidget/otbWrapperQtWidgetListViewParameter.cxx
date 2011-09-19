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
#include "otbWrapperQtWidgetListViewParameter.h"

#include "otbWrapperQtWidgetParameterLabel.h"
#include "otbWrapperQtWidgetParameterFactory.h"

namespace otb
{
namespace Wrapper
{

QtWidgetListViewParameter::QtWidgetListViewParameter(ListViewParameter* param, QtWidgetModel* m)
: QtWidgetParameterBase(m),
  m_ListViewParam(param)
{
}

QtWidgetListViewParameter::~QtWidgetListViewParameter()
{
}

void QtWidgetListViewParameter::DoUpdateGUI()
{
  // remove all the items set before
  while(m_ListView->takeItem(0))
    {
    m_ListView->removeItemWidget( m_ListView->takeItem(0) );
    }

  for (unsigned int i = 0; i < m_ListViewParam->GetNbChoices(); ++i)
    {
    // Add listBox items
    QString key = QString::fromStdString( m_ListViewParam->GetChoiceKey(i) );
    m_ListView->addItem( key);
    }

  // Update the combobox value
  unsigned int value = m_ListViewParam->GetValue( );
  m_ListView->setCurrentRow(value);
}

void QtWidgetListViewParameter::DoCreateWidget()
{
  m_ListView = new QListWidget();
  m_ListView->setToolTip(m_ListViewParam->GetDescription());
  m_ListView->setSelectionMode(QAbstractItemView::MultiSelection);
  connect( m_ListView, SIGNAL(itemSelectionChanged()), this, SLOT(SelectedItems()) );
  connect( GetModel(), SIGNAL(UpdateGui()), this, SLOT(UpdateGUI() ) );

  m_VLayout = new QHBoxLayout;
  m_VLayout->setContentsMargins(0, 0, 0, 0);
  m_VLayout->addWidget(m_ListView);

  this->setLayout(m_VLayout);
}

void QtWidgetListViewParameter::SelectedItems()
{
  // Clear previous item selected
  m_SelectedItems.clear();

  // Item changed (check if selected or not)
  for (int idx = 0; idx < m_ListView->count(); ++idx)
    {
    if (m_ListView->item(idx)->isSelected())
      {
      m_SelectedItems.push_back(idx);
      }
    }
  m_ListViewParam->SetSelectedItems(m_SelectedItems);
}

}
}
