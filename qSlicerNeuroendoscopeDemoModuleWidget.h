/*==============================================================================

  Program: 3D Slicer

  Portions (c) Copyright Brigham and Women's Hospital (BWH) All Rights Reserved.

  See COPYRIGHT.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

==============================================================================*/

#ifndef __qSlicerNeuroendoscopeDemoModuleWidget_h
#define __qSlicerNeuroendoscopeDemoModuleWidget_h

// SlicerQt includes
#include "qSlicerAbstractModuleWidget.h"

#include "qSlicerNeuroendoscopeDemoModuleExport.h"

#include <vtkMRMLCameraNode.h>
#include <vtkMRMLLinearTransformNode.h>

class qSlicerNeuroendoscopeDemoModuleWidgetPrivate;
class vtkMRMLNode;

/// \ingroup Slicer_QtModules_ExtensionTemplate
class Q_SLICER_QTMODULES_NEUROENDOSCOPEDEMO_EXPORT qSlicerNeuroendoscopeDemoModuleWidget :
  public qSlicerAbstractModuleWidget
{
  Q_OBJECT

public:

  typedef qSlicerAbstractModuleWidget Superclass;
  qSlicerNeuroendoscopeDemoModuleWidget(QWidget *parent=0);
  virtual ~qSlicerNeuroendoscopeDemoModuleWidget();

public slots:
  void onTrackingONToggled(bool checked);
  void onVideoONToggled(bool checked);

protected:
  QScopedPointer<qSlicerNeuroendoscopeDemoModuleWidgetPrivate> d_ptr;
  
  virtual void setup();

  vtkMRMLCameraNode* CameraNode;
  vtkMRMLLinearTransformNode* TransformNode;

private:
  Q_DECLARE_PRIVATE(qSlicerNeuroendoscopeDemoModuleWidget);
  Q_DISABLE_COPY(qSlicerNeuroendoscopeDemoModuleWidget);
};

#endif