#include "stdafx.h"
#include "ImageViewerManger.h"

#include "MainFrm.h"
#include "ChildView.h"

ImageViewerManager* ImageViewerManager::m_Instance = NULL;

ImageViewerManager::ImageViewerManager()
{
}


ImageViewerManager::~ImageViewerManager()
{
}

ImageViewerManager* ImageViewerManager::Mgr() {
	if (m_Instance == NULL) {
		m_Instance = new ImageViewerManager();
		atexit(Destroy);							// ??????????????????
	}
	return m_Instance;
}

vtkSmartPointer<vtkRenderWindow> ImageViewerManager::GetVtkWindow(int viewType) {
	if (viewType < 0 || viewType >= NUM_VIEW)
		return NULL;

	// viewType�� vtkRenderWindow ��ȯ
	return m_vtkWindow[viewType];
}

void ImageViewerManager::InitVtkWindow(int viewType, void* hWnd) {
	if (viewType < 0 || viewType >= NUM_VIEW)
		return;

	// vtk Render Window ����
	if (m_vtkWindow[viewType] == NULL) {
		// Interactor ����
		vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
		// Renderer ����
		vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
		renderer->SetBackground(0.0, 0.0, 0.0);				// ������ ���

		// 3D View ����
		if (viewType == VIEW_3D) {
			// Trackball Camera ���ͷ��� ��Ÿ�� ����
			interactor->SetInteractorStyle(vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New());
			vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();
			//camera->ParallelProjectionOn();					
			camera->ParallelProjectionOff();
			//camera->SetPosition(0.0, -1.0, 0.0);			// ī�޶� ��ġ
			//camera->SetViewUp(0.0, 0.0, 1.0);				// ī�޶� up ����
			camera->SetPosition(0.0, 0.0, -1.0);
			camera->SetViewUp(0.0, 1.0, 0.0);
			camera->SetViewAngle(20);
			
			double dA[3], dB;
			camera->GetFocalPoint(dA);
			dB = camera->GetViewAngle();
			CString msg;
			msg.Format(L"%f, %f, %f (%f)", dA[0], dA[1], dA[2], dB);
			//AfxMessageBox(msg);
		}
		// 2D View ����
		else {
			interactor->SetInteractorStyle(vtkSmartPointer<vtkInteractorStyleImage>::New());
			vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();
			camera->ParallelProjectionOn();
			camera->SetPosition(0.0, 0.0, -1.0);
			camera->SetViewUp(0.0, -1.0, 0.0);

		}

		// RenderWindow ���� �� Dialog �ڵ�, Interactor, Renderer ����
		m_vtkWindow[viewType] = vtkSmartPointer<vtkRenderWindow>::New();
		m_vtkWindow[viewType]->SetParentId(hWnd);
		m_vtkWindow[viewType]->SetInteractor(interactor);
		m_vtkWindow[viewType]->AddRenderer(renderer);
		
		m_vtkWindow[viewType]->Render();
	}
}

void ImageViewerManager::ResizeVtkWindow(int viewType, int width, int height) {
	if (viewType < 0 || viewType >= NUM_VIEW)
		return;

	if (m_vtkWindow[viewType] == NULL)
		return;

	// �ش� vtkRenderWindow ũ�� ����
	m_vtkWindow[viewType]->SetSize(width, height);
}


vtkSmartPointer<ImageLoader> ImageViewerManager::GetImageLoader() {
	if (m_ImageLoader == NULL)
		m_ImageLoader = vtkSmartPointer<ImageLoader>::New();

	return m_ImageLoader;
}

void ImageViewerManager::UpdateVTKWindows() {
	// ������ �ʱ�ȭ
	ClearVolumeDisplay();
	// Volume Rendering ������Ʈ
	UpdateVolumeDisplay();
	// �⺻ View ������ ���
	CChildView* mainView = ((CMainFrame*)AfxGetMainWnd())->GetWndView();
	if (mainView == NULL)
		return;

	// 2D View Scrollbar ������Ʈ
	for (int viewType = VIEW_AXIAL; viewType <= VIEW_SAGITTAL; viewType++) {
		mainView->GetDlgVTKView(viewType)->update_scroll_bar();
	}
}

vtkSmartPointer<vtkRenderer> ImageViewerManager::GetRenderer(int viewType) {
	if (viewType < 0 || viewType >= NUM_VIEW)
		return NULL;
	// vtkRenderingWindow ���� ���� �˻�
	if (m_vtkWindow[viewType] == NULL)
		return NULL;
	// �ش� View Ÿ���� vtkRenderWindow���� ù��° Renderer ��ȯ?
	return m_vtkWindow[viewType]->GetRenderers()->GetFirstRenderer();
}

void ImageViewerManager::ClearVolumeDisplay() {
	// Volume Data�˻�
	vtkSmartPointer<VolumeData> volumeData = GetImageLoader()->GetVolumeData();
	if (volumeData == NULL)
		return;
	// 3D View ���� Volume Rendering ����
	GetRenderer(VIEW_3D)->RemoveViewProp(volumeData->GetVolumeRendering());
	// Slice View�� �� Slice Actor ����
	for (int viewType = VIEW_AXIAL; viewType <= VIEW_SAGITTAL; viewType++) {
		GetRenderer(viewType)->RemoveActor(volumeData->GetSliceActor(viewType));
	}
}

void ImageViewerManager::UpdateVolumeDisplay(bool bCameraReset,
	double pos_x, double pos_y, double pos_z,
	double up_x, double up_y, double up_z) {
	vtkSmartPointer<VolumeData> volumeData = GetImageLoader()->GetVolumeData();
	if (volumeData == NULL)
		return;

	// 3D View�� Volume Rendering �߰�
	GetRenderer(VIEW_3D)->AddViewProp(volumeData->GetVolumeRendering());
	if (!bCameraReset) {
		GetRenderer(VIEW_3D)->ResetCamera();			// ī�޶� �缳��
		GetRenderer(VIEW_3D)->GetActiveCamera()->Zoom(2.5);
	}
	else {
		vtkSmartPointer<vtkCamera> camera = vtkSmartPointer<vtkCamera>::New(); //GetRenderer(VIEW_3D)->GetActiveCamera();
		camera->SetPosition(pos_x, pos_y, pos_z);
		camera->SetViewUp(up_x, up_y, up_z);
		GetRenderer(VIEW_3D)->SetActiveCamera(camera);
		GetRenderer(VIEW_3D)->ResetCamera();
		GetRenderer(VIEW_3D)->GetActiveCamera()->Zoom(2.5);
		//m_vtkWindow[VIEW_3D]->RemoveRenderer(GetRenderer(VIEW_3D));
		//vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
		//renderer->SetBackground(0.0, 0.0, 0.0);
		//vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();
		//camera->ParallelProjectionOff();
		//camera->SetPosition(pos_x, pos_y, pos_z);
		//camera->SetViewUp(up_x, up_y, up_z);
		//m_vtkWindow[VIEW_3D]->AddRenderer(renderer);
	}
	m_vtkWindow[VIEW_3D]->Render();					// ȭ�� ����

	for (int viewType = VIEW_AXIAL; viewType <= VIEW_SAGITTAL; viewType++) {
		GetRenderer(viewType)->AddActor(volumeData->GetSliceActor(viewType));
		GetRenderer(viewType)->ResetCamera();
		m_vtkWindow[viewType]->Render();
	}
}

void ImageViewerManager::ScrollSliceIndex(int viewType, int pos) {
	vtkSmartPointer<VolumeData> volumeData = GetImageLoader()->GetVolumeData();
	volumeData->SetSliceIndex(viewType, pos);

	m_vtkWindow[viewType]->Render();
}

void ImageViewerManager::ClippingVolume() {
	ClearVolumeDisplay();

	vtkSmartPointer<VolumeData> volumeData = GetImageLoader()->GetVolumeData();
	if (volumeData == NULL)
		return;

	volumeData->ClippingVolume();

	UpdateVolumeDisplay(true, -1.0, 0.0, 0.0, 0.0, 1.0, 0.0);
}

void ImageViewerManager::RestoringVolume() {
	ClearVolumeDisplay();
	vtkSmartPointer<VolumeData> volumeData = GetImageLoader()->GetVolumeData();
	if (volumeData == NULL)
		return;

	volumeData->RestoringVolume();

	UpdateVolumeDisplay(true, 0.0, 0.0, -1.0, 0.0, 1.0, 0.0);
}

void ImageViewerManager::SetNumImages(int num_images_arg) {
	CMainFrame* mainView = (CMainFrame*)AfxGetMainWnd();
	if (mainView == NULL)
		return;
	mainView->SetNumImages(num_images_arg);
}
void ImageViewerManager::SetCurrentImageIndex(int index_arg) {
	CMainFrame* mainView = (CMainFrame*)AfxGetMainWnd();
	if (mainView == NULL)
		return;
	mainView->SetCurrentImageIndex(index_arg);
}