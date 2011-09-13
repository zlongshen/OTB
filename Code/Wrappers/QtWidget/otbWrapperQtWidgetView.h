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
#ifndef __otbWrapperQtWidgetView_h
#define __otbWrapperQtWidgetView_h

#include <QtGui>
#include <QObject>
#include "otbWrapperApplication.h"
#include "otbWrapperQtWidgetModel.h"

namespace otb
{
namespace Wrapper
{

/** \class
 * \brief
 */
class QtWidgetView : public QWidget
{
  Q_OBJECT
public:
  QtWidgetView(Application* app);
  virtual ~QtWidgetView();

  void CreateGui();

  QtWidgetModel* GetModel()
  {
    return m_Model;
  }

public slots:
  void CloseSlot();

signals:
  void QuitSignal();
  

private:
  QtWidgetView(const QtWidgetView&); //purposely not implemented
  void operator=(const QtWidgetView&); //purposely not implemented

  QWidget* CreateHeader();

  QWidget* CreateFooter();

  QWidget* CreateInputWidgets();

  Application::Pointer m_Application;

  QtWidgetModel* m_Model;

  QPushButton* m_ExecButton;
  QPushButton* m_QuitButton;
};


}
}

#endif
