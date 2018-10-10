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

	// viewType별 vtkRenderWindow 변환
	return m_vtkWindow[viewType];
}

void ImageViewerManager::InitVtkWindow(int viewType, void* hWnd) {
	if (viewType < 0 || viewType >= NUM_VIEW)
		return;

	// vtk Render Window 생성
	if (m_vtkWindow[viewType] == NULL) {
		// Interactor 생성
		vtkSmartPointer<vtkRenderWindowInteractor> interactor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
		// Renderer 생성
		vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
		renderer->SetBackground(0.0, 0.0, 0.0);				// 검은색 배경

		// 3D View 설정
		if (viewType == VIEW_3D) {
			// Trackball Camera 인터랙션 스타일 적용
			interactor->SetInteractorStyle(vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New());
			vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();
			//camera->ParallelProjectionOn();					
			camera->ParallelProjectionOff();
			//camera->SetPosition(0.0, -1.0, 0.0);			// 카메라 위치
			//camera->SetViewUp(0.0, 0.0, 1.0);				// 카메라 up 벡터
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
		// 2D View 설정
		else {
			interactor->SetInteractorStyle(vtkSmartPointer<vtkInteractorStyleImage>::New());
			vtkSmartPointer<vtkCamera> camera = renderer->GetActiveCamera();
			camera->ParallelProjectionOn();
			camera->SetPosition(0.0, 0.0, -1.0);
			camera->SetViewUp(0.0, -1.0, 0.0);

		}

		// RenderWindow 생성 후 Dialog 핸들, Interactor, Renderer 생성
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

	// 해당 vtkRenderWindow 크기 조절
	m_vtkWindow[viewType]->SetSize(width, height);
}


vtkSmartPointer<ImageLoader> ImageViewerManager::GetImageLoader() {
	if (m_ImageLoader == NULL)
		m_ImageLoader = vtkSmartPointer<ImageLoader>::New();

	return m_ImageLoader;
}

void ImageViewerManager::UpdateVTKWindows() {
	// 렌더링 초기화
	ClearVolumeDisplay();
	// Volume Rendering 업데이트
	UpdateVolumeDisplay();
	// 기본 View 윈도우 얻기
	CChildView* mainView = ((CMainFrame*)AfxGetMainWnd())->GetWndView();
	if (mainView == NULL)
		return;

	// 2D View Scrollbar 업데이트
	for (int viewType = VIEW_AXIAL; viewType <= VIEW_SAGITTAL; viewType++) {
		mainView->GetDlgVTKView(viewType)->update_scroll_bar();
	}
}

vtkSmartPointer<vtkRenderer> ImageViewerManager::GetRenderer(int viewType) {
	if (viewType < 0 || viewType >= NUM_VIEW)
		return NULL;
	// vtkRenderingWindow 생성 여부 검사
	if (m_vtkWindow[viewType] == NULL)
		return NULL;
	// 해당 View 타입의 vtkRenderWindow에서 첫번째 Renderer 반환?
	return m_vtkWindow[viewType]->GetRenderers()->GetFirstRenderer();
}

void ImageViewerManager::ClearVolumeDisplay() {
	// Volume Data검사
	vtkSmartPointer<VolumeData> volumeData = GetImageLoader()->GetVolumeData();
	if (volumeData == NULL)
		return;
	// 3D View 상의 Volume Rendering 제거
	GetRenderer(VIEW_3D)->RemoveViewProp(volumeData->GetVolumeRendering());
	// Slice View에 각 Slice Actor 제거
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

	// 3D View에 Volume Rendering 추가
	GetRenderer(VIEW_3D)->AddViewProp(volumeData->GetVolumeRendering());
	if (!bCameraReset) {
		GetRenderer(VIEW_3D)->ResetCamera();			// 카메라 재설정
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
	m_vtkWindow[VIEW_3D]->Render();					// 화면 갱신

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