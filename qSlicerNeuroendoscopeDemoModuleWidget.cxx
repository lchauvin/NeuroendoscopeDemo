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
#include <vtkSmartPointer.h>
#include <vtkMatrix4x4.h>
#include <vtkEventQtSlotConnect.h>

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
  this->FilteredTransform = vtkMRMLLinearTransformNode::New();
  this->RawTransform = NULL;
  this->w_cutoff = 7.5;
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
    // Try to get transform
    vtkCollection* neuroendoscopeTransforms = NULL;
    neuroendoscopeTransforms = this->mrmlScene()->GetNodesByName("Neuroendoscope");
    if(!neuroendoscopeTransforms)
      return;

    if(neuroendoscopeTransforms->GetNumberOfItems() > 0)
      {
      this->RawTransform = dynamic_cast<vtkMRMLLinearTransformNode*>(neuroendoscopeTransforms->GetItemAsObject(neuroendoscopeTransforms->GetNumberOfItems()-1));
      if(!this->RawTransform)
        return;

      // TODO: Observe RawTransform modified event then apply smoothing filter
      vtkEventQtSlotConnect* eventToSlot = vtkEventQtSlotConnect::New();
      eventToSlot->Connect(this->RawTransform, vtkMRMLLinearTransformNode::TransformModifiedEvent,
                           this, SLOT(onTrackerCoordinatesReceived()));
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
 
    if(this->FilteredTransform->GetScene() == NULL)
	{
	  std::cerr << "Adding FilteredTransform to the scene" << std::endl;
	  this->FilteredTransform->SetName("Filtered_Neuroendoscope");	    
	  this->mrmlScene()->AddNode(this->FilteredTransform);
	}
 
    this->CameraNode->SetAndObserveTransformNodeID(this->FilteredTransform->GetID());
    }
  else
    {
    if(this->CameraNode)
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
    capture = cvCaptureFromCAM(0);

    assert( capture != NULL);

    IplImage* bgr_frame = cvQueryFrame( capture );
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 240);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 320);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FPS,30);

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

//-----------------------------------------------------------------------------
void qSlicerNeuroendoscopeDemoModuleWidget::SmoothingFilter(vtkMRMLLinearTransformNode* input, vtkMRMLLinearTransformNode* output)
{
  double UnfiltPosOr[6], FiltPosOr[6];
  double dt = 0.015; // 15ms

  // Get Matrixes
  vtkSmartPointer<vtkMatrix4x4> inputMatrix =
    input->GetMatrixTransformToParent();

  vtkSmartPointer<vtkMatrix4x4> outputMatrix =
    output->GetMatrixTransformToParent();

  // Filter
  UnfiltPosOr[0] = inputMatrix->GetElement(0,3);
  UnfiltPosOr[1] = inputMatrix->GetElement(1,3);
  UnfiltPosOr[2] = inputMatrix->GetElement(2,3);

  UnfiltPosOr[4] =
    atan2(-inputMatrix->GetElement(2,0),pow(pow(inputMatrix->GetElement(0,0),2)+pow(inputMatrix->GetElement(1,0),2),0.5)
    ); //beta

  UnfiltPosOr[5] =
    atan2(inputMatrix->GetElement(2,1)/cos(UnfiltPosOr[4]),inputMatrix->GetElement(2,2)/cos(UnfiltPosOr[4]));
  // gamma

  UnfiltPosOr[3] =
    atan2(inputMatrix->GetElement(1,0)/cos(UnfiltPosOr[4]),inputMatrix->GetElement(0,0)/cos(UnfiltPosOr[4]));
  //alpha


  FiltPosOr[0] = outputMatrix->GetElement(0,3);
  FiltPosOr[1] = outputMatrix->GetElement(1,3);
  FiltPosOr[2] = outputMatrix->GetElement(2,3);

  FiltPosOr[4] =
    atan2(-outputMatrix->GetElement(2,0),pow(pow(outputMatrix->GetElement(0,0),2)+pow(outputMatrix->GetElement(1,0),2),0.5)
    ); //beta

  FiltPosOr[5] =
    atan2(outputMatrix->GetElement(2,1)/cos(FiltPosOr[4]),outputMatrix->GetElement(2,2)/cos(FiltPosOr[4]));
  // gamma

  FiltPosOr[3] =
    atan2(outputMatrix->GetElement(1,0)/cos(FiltPosOr[4]),outputMatrix->GetElement(0,0)/cos(FiltPosOr[4]));
  // alpha


  for(int i = 0; i < 6; i++)
    {
    // Low pass filter with w_cutoff frequency
    FiltPosOr[i]  = (FiltPosOr[i] + dt*this->w_cutoff*UnfiltPosOr[i])/(1 + this->w_cutoff*dt);
    }

  double a = FiltPosOr[3];
  double b = FiltPosOr[4];
  double g = FiltPosOr[5];

  // Create the transformation again
  vtkSmartPointer<vtkMatrix4x4> TransformationMatrix =
    vtkSmartPointer<vtkMatrix4x4> ::New();

  TransformationMatrix->SetElement(0,0,cos(a)*cos(b));
  TransformationMatrix->SetElement(0,1,cos(a)*sin(b)*sin(g)-sin(a)*cos(g));
  TransformationMatrix->SetElement(0,2,cos(a)*sin(b)*cos(g)+sin(a)*sin(g));
  TransformationMatrix->SetElement(0,3,FiltPosOr[0]);

  TransformationMatrix->SetElement(1,0,sin(a)*cos(b));
  TransformationMatrix->SetElement(1,1,sin(a)*sin(b)*sin(g)+cos(a)*cos(g));
  TransformationMatrix->SetElement(1,2,sin(a)*sin(b)*cos(g)-cos(a)*sin(g));
  TransformationMatrix->SetElement(1,3,FiltPosOr[1]);

  TransformationMatrix->SetElement(2,0,-sin(b));
  TransformationMatrix->SetElement(2,1,cos(b)*sin(g));
  TransformationMatrix->SetElement(2,2,cos(b)*cos(g));
  TransformationMatrix->SetElement(2,3,FiltPosOr[2]);

  TransformationMatrix->SetElement(3,0,0);
  TransformationMatrix->SetElement(3,1,0);
  TransformationMatrix->SetElement(3,2,0);
  TransformationMatrix->SetElement(3,3,1);

  // Setting the TransformNode
  output->SetAndObserveMatrixTransformToParent(TransformationMatrix);
}

//-----------------------------------------------------------------------------
void qSlicerNeuroendoscopeDemoModuleWidget::onTrackerCoordinatesReceived()
{
  if(this->RawTransform && this->FilteredTransform)
    {
      this->SmoothingFilter(this->RawTransform, this->FilteredTransform);
    }
}
