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

// Qt includes
#include <QtPlugin>

// ExtensionTemplate Logic includes
#include <vtkSlicerNeuroendoscopeDemoLogic.h>

// ExtensionTemplate includes
#include "qSlicerNeuroendoscopeDemoModule.h"
#include "qSlicerNeuroendoscopeDemoModuleWidget.h"

//-----------------------------------------------------------------------------
Q_EXPORT_PLUGIN2(qSlicerNeuroendoscopeDemoModule, qSlicerNeuroendoscopeDemoModule);

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerNeuroendoscopeDemoModulePrivate
{
public:
  qSlicerNeuroendoscopeDemoModulePrivate();
};

//-----------------------------------------------------------------------------
// qSlicerNeuroendoscopeDemoModulePrivate methods

//-----------------------------------------------------------------------------
qSlicerNeuroendoscopeDemoModulePrivate::qSlicerNeuroendoscopeDemoModulePrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerNeuroendoscopeDemoModule methods

//-----------------------------------------------------------------------------
qSlicerNeuroendoscopeDemoModule::qSlicerNeuroendoscopeDemoModule(QObject* _parent)
  : Superclass(_parent)
  , d_ptr(new qSlicerNeuroendoscopeDemoModulePrivate)
{
}

//-----------------------------------------------------------------------------
qSlicerNeuroendoscopeDemoModule::~qSlicerNeuroendoscopeDemoModule()
{
}

//-----------------------------------------------------------------------------
QString qSlicerNeuroendoscopeDemoModule::helpText()const
{
  return "This is a loadable module bundled in an extension";
}

//-----------------------------------------------------------------------------
QString qSlicerNeuroendoscopeDemoModule::acknowledgementText()const
{
  return "This work was was partially funded by NIH grant 3P41RR013218-12S1";
}

//-----------------------------------------------------------------------------
QStringList qSlicerNeuroendoscopeDemoModule::contributors()const
{
  QStringList moduleContributors;
  moduleContributors << QString("Jean-Christophe Fillion-Robin (Kitware)");
  return moduleContributors;
}

//-----------------------------------------------------------------------------
QIcon qSlicerNeuroendoscopeDemoModule::icon()const
{
  return QIcon(":/Icons/NeuroendoscopeDemo.png");
}

//-----------------------------------------------------------------------------
QStringList qSlicerNeuroendoscopeDemoModule::categories() const
{
  return QStringList() << "Examples";
}

//-----------------------------------------------------------------------------
QStringList qSlicerNeuroendoscopeDemoModule::dependencies() const
{
  return QStringList();
}

//-----------------------------------------------------------------------------
void qSlicerNeuroendoscopeDemoModule::setup()
{
  this->Superclass::setup();
}

//-----------------------------------------------------------------------------
qSlicerAbstractModuleRepresentation * qSlicerNeuroendoscopeDemoModule::createWidgetRepresentation()
{
  return new qSlicerNeuroendoscopeDemoModuleWidget;
}

//-----------------------------------------------------------------------------
vtkMRMLAbstractLogic* qSlicerNeuroendoscopeDemoModule::createLogic()
{
  return vtkSlicerNeuroendoscopeDemoLogic::New();
}
