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
#include <QDebug>

// VTK includes
#include <vtkCollection.h>
#include <vtkMRMLScene.h>

// SlicerQt includes
#include "qSlicerNeuroendoscopeDemoModuleWidget.h"
#include "ui_qSlicerNeuroendoscopeDemoModule.h"

//-----------------------------------------------------------------------------
/// \ingroup Slicer_QtModules_ExtensionTemplate
class qSlicerNeuroendoscopeDemoModuleWidgetPrivate: public Ui_qSlicerNeuroendoscopeDemoModule
{
public:
  qSlicerNeuroendoscopeDemoModuleWidgetPrivate();
};

//-----------------------------------------------------------------------------
// qSlicerNeuroendoscopeDemoModuleWidgetPrivate methods

//-----------------------------------------------------------------------------
qSlicerNeuroendoscopeDemoModuleWidgetPrivate::qSlicerNeuroendoscopeDemoModuleWidgetPrivate()
{
}

//-----------------------------------------------------------------------------
// qSlicerNeuroendoscopeDemoModuleWidget methods

//-----------------------------------------------------------------------------
qSlicerNeuroendoscopeDemoModuleWidget::qSlicerNeuroendoscopeDemoModuleWidget(QWidget* _parent)
  : Superclass( _parent )
  , d_ptr( new qSlicerNeuroendoscopeDemoModuleWidgetPrivate )
{
  this->CameraNode = NULL;
  this->TransformNode = NULL;
}

//-----------------------------------------------------------------------------
qSlicerNeuroendoscopeDemoModuleWidget::~qSlicerNeuroendoscopeDemoModuleWidget()
{
}

//-----------------------------------------------------------------------------
void qSlicerNeuroendoscopeDemoModuleWidget::setup()
{
  Q_D(qSlicerNeuroendoscopeDemoModuleWidget);
  d->setupUi(this);
  this->Superclass::setup();

  connect(d->TrackingON, SIGNAL(toggled(bool)),
	  this, SLOT(onTrackingONToggled(bool)));

  connect(d->VideoON, SIGNAL(toggled(bool)),
	  this, SLOT(onVideoONToggled(bool)));

}

//-----------------------------------------------------------------------------
void qSlicerNeuroendoscopeDemoModuleWidget::onTrackingONToggled(bool checked)
{
  if(checked)
    {
      std::cerr << "Tracking ON" << std::endl;
  
     // Try to get transform
      vtkCollection* neuroendoscopeTransforms = NULL;
      neuroendoscopeTransforms = this->mrmlScene()->GetNodesByName("Neuroendoscope");
      if(!neuroendoscopeTransforms)
	return;

      if(neuroendoscopeTransforms->GetNumberOfItems() > 0)
	{
	  this->TransformNode = dynamic_cast<vtkMRMLLinearTransformNode*>(neuroendoscopeTransforms->GetItemAsObject(neuroendoscopeTransforms->GetNumberOfItems()-1));
	  if(!this->TransformNode)
	    return;
	}
      
      // Try to get camera node
      vtkCollection* slicerCameras = NULL;
      slicerCameras = this->mrmlScene()->GetNodesByName("Default Scene Camera");
      if(!slicerCameras)
	return;
      
      if(slicerCameras->GetNumberOfItems() > 0)
	{
	  this->CameraNode = dynamic_cast<vtkMRMLCameraNode*>(slicerCameras->GetItemAsObject(slicerCameras->GetNumberOfItems()-1));
	  if(!this->CameraNode)
	    return;
	}
      
      std::cerr << "SetAndObserve" << std::endl;
      this->CameraNode->SetAndObserveTransformNodeID(this->TransformNode->GetID());
    }
  else
    {
      if(this->CameraNode && this->TransformNode)
	{
	  this->CameraNode->SetAndObserveTransformNodeID("");
	}
    }

  this->mrmlScene()->Modified();
}

//-----------------------------------------------------------------------------
void qSlicerNeuroendoscopeDemoModuleWidget::onVideoONToggled(bool checked)
{
  Q_D(qSlicerNeuroendoscopeDemoModuleWidget);

  if(checked)
    {
      CvCapture* capture;

      // Open /dev/video0
      capture = cvCreateCameraCapture(0);
      assert( capture != NULL);

      IplImage* bgr_frame = cvQueryFrame( capture );
      //Size size = cvSize(
      //		   (int)cvGetCaptureProperty( capture,
      //					      CV_CAP_PROP_FRAME_WIDTH),
      //		   (int)cvGetCaptureProperty( capture,
      //					      CV_CAP_PROP_FRAME_HEIGHT)
      //		   );
      cvNamedWindow( "PENTAX Neuroendoscope", CV_WINDOW_AUTOSIZE);

      while( ((bgr_frame = cvQueryFrame( capture )) != NULL) && checked)
	{
	  // Display image on OpenCV window
	  cvShowImage( "PENTAX Neuroendoscope", bgr_frame);
	  char c = cvWaitKey(33);
	  if( c == 27 ) break;
	}

      d->VideoON->setChecked(false);
      d->VideoOFF->setChecked(true);

      cvReleaseCapture(&capture);
      cvDestroyWindow( "PENTAX Neuroendoscope" );
    }
}
