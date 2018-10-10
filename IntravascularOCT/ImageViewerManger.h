#pragma once

#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkInteractorStyleImage.h>
#include <vtkRendererCollection.h>
#include <vtkCornerAnnotation.h>

//#include "VolumeData.h"
#include "ImageLoader.h"

// Image Viewer Manager
class ImageViewerManager
{
private:													// private으로 수정 Mgr() 함수를 통해 접근????
	ImageViewerManager();
	virtual ~ImageViewerManager();								// virtual 추가. 왜? 책이 이유 설명 없음 .. 

	static ImageViewerManager* m_Instance;
	static void Destroy() { delete m_Instance; m_Instance = NULL; }

public:
	// 전역 매너저 객체??
	static ImageViewerManager* Mgr();

	// 왜 나누지? FormView 관리??
public:
	// View Type
	static const int NUM_VIEW = 4;
	enum ViewType { VIEW_AXIAL, VIEW_CORONAL, VIEW_SAGITTAL, VIEW_3D};

protected:
	vtkSmartPointer<vtkRenderWindow>	m_vtkWindow[NUM_VIEW];
	vtkSmartPointer<ImageLoader>		m_ImageLoader;

public:
	vtkSmartPointer<vtkRenderWindow> GetVtkWindow(int viewType);
	void InitVtkWindow(int viewType, void* hWnd);				// VTK Window 초기화
	void ResizeVtkWindow(int viewType, int width, int height);	// window 크기 조절

	vtkSmartPointer<ImageLoader> GetImageLoader();

	// 화면 UPDATE 및 최적화 (원 코드 함수명: OnSelectDicomGroup)
	void UpdateVTKWindows();

	// ViewType에 따른 VTK 렌더러
	vtkSmartPointer<vtkRenderer> GetRenderer(int viewType);
	// Volume 그리기 최기화
	void ClearVolumeDisplay();
	// Volume 그리기 업데이트
	void UpdateVolumeDisplay(bool bCameraReset = false,
		double pos_x = 0.f, double pos_y = 0.f, double pos_z = 0.f,
		double up_x = 0.f, double up_y = 0.f, double up_z = 0.f);

	// 스크롤바를 통한 슬라이스 인덱스 변경
	void ScrollSliceIndex(int viewType, int pos);


	void ClippingVolume();
	void RestoringVolume();

	// Loading file status
	void SetNumImages(int num_images_arg);
	void SetCurrentImageIndex(int index_arg);
	
};

