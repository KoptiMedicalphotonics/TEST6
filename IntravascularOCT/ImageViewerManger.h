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
private:													// private���� ���� Mgr() �Լ��� ���� ����????
	ImageViewerManager();
	virtual ~ImageViewerManager();								// virtual �߰�. ��? å�� ���� ���� ���� .. 

	static ImageViewerManager* m_Instance;
	static void Destroy() { delete m_Instance; m_Instance = NULL; }

public:
	// ���� �ų��� ��ü??
	static ImageViewerManager* Mgr();

	// �� ������? FormView ����??
public:
	// View Type
	static const int NUM_VIEW = 4;
	enum ViewType { VIEW_AXIAL, VIEW_CORONAL, VIEW_SAGITTAL, VIEW_3D};

protected:
	vtkSmartPointer<vtkRenderWindow>	m_vtkWindow[NUM_VIEW];
	vtkSmartPointer<ImageLoader>		m_ImageLoader;

public:
	vtkSmartPointer<vtkRenderWindow> GetVtkWindow(int viewType);
	void InitVtkWindow(int viewType, void* hWnd);				// VTK Window �ʱ�ȭ
	void ResizeVtkWindow(int viewType, int width, int height);	// window ũ�� ����

	vtkSmartPointer<ImageLoader> GetImageLoader();

	// ȭ�� UPDATE �� ����ȭ (�� �ڵ� �Լ���: OnSelectDicomGroup)
	void UpdateVTKWindows();

	// ViewType�� ���� VTK ������
	vtkSmartPointer<vtkRenderer> GetRenderer(int viewType);
	// Volume �׸��� �ֱ�ȭ
	void ClearVolumeDisplay();
	// Volume �׸��� ������Ʈ
	void UpdateVolumeDisplay(bool bCameraReset = false,
		double pos_x = 0.f, double pos_y = 0.f, double pos_z = 0.f,
		double up_x = 0.f, double up_y = 0.f, double up_z = 0.f);

	// ��ũ�ѹٸ� ���� �����̽� �ε��� ����
	void ScrollSliceIndex(int viewType, int pos);


	void ClippingVolume();
	void RestoringVolume();

	// Loading file status
	void SetNumImages(int num_images_arg);
	void SetCurrentImageIndex(int index_arg);
	
};

