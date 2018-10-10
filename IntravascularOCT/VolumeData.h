#pragma once

#include <vtkObject.h>
#include <vtkSmartPointer.h>

#include <vtkImageStack.h>
#include <vtkImageImport.h>
#include <vtkImageSliceMapper.h>

#include <vtkImageData.h>

#include <vtkTransform.h>
#include <vtkMatrix4x4.h>
#include <vtkVolume.h>
#include <vtkVolumeProperty.h>
#include <vtkSmartVolumeMapper.h>
#include <vtkPiecewiseFunction.h>
#include <vtkColorTransferFunction.h>					// ?????????????????????????????
#include <vtkImageReslice.h>
#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>
#include <vtkImageProperty.h>
#include <vtkExtractVOI.h>
#include <vtkPlane.h>




class VolumeData: public vtkObject
{
public:
	vtkTypeMacro(VolumeData, vtkObject);
	static VolumeData *New() { return new VolumeData; }

protected:
	VolumeData();
	virtual ~VolumeData();
	


public:
	enum SliceType {AXIAL, CORONAL, SAGITTAL};
	// 미리 정의된 Volume Rendering 투명도 함수 및 컬러함수
	enum RenderingPreset {MIP, SKIN, INTRAVASCULAR};

protected:
	// VTK 3차원 이미지 데이터
	vtkSmartPointer<vtkImageData> m_ImageData;
	// 3차원 회전이동 정보
	vtkSmartPointer<vtkMatrix4x4> m_Orientation;
	// Volume Rendering 객체
	vtkSmartPointer<vtkVolume> m_VolumeRendering;
	// Volume Rendering 투명도 함수
	vtkSmartPointer<vtkPiecewiseFunction> m_OpacityFunc;
	// Volume Rendering color 함수
	vtkSmartPointer<vtkColorTransferFunction> m_ColorFunc;
	// 현재 Volume Rendering Mode
	int m_CurrentPresetMode;
	// Slice Rendering 객체 (Axial, Coronal, Sagittal)
	vtkSmartPointer<vtkImageActor> m_SliceActor[3];
	// Slice Rendering을 위한 Volume 단면
	vtkSmartPointer<vtkImageReslice> m_VolumeSlice[3];
	// Slice Rendering 속성
	vtkSmartPointer<vtkImageProperty> m_SliceProperty;
	// Slice Index
	int m_SliceIndex[3];

public:
	// VTK Volume 데이터 가져오기/설정하기
	vtkSmartPointer<vtkImageData> GetImgeData() const { return m_ImageData; }
	void SetImageData(vtkSmartPointer<vtkImageData> val) { m_ImageData = val; }
	// Volume 데이터의 회전이동 정보 가져오기/설정하기
	vtkSmartPointer<vtkMatrix4x4> GetOrientation() const { return m_Orientation; }
	void SetOrientation(vtkSmartPointer<vtkMatrix4x4> val) { m_Orientation = val; }
	// Volume Rendering 객체
	vtkSmartPointer<vtkVolume> GetVolumeRendering() const { return m_VolumeRendering; }
	// Volume Rendering 준비
	void ReadyForVolumeRendering();
	// 현재 Volume Rendering 모드 가져오기/설정하기
	int GetCurrentPresetMode() const { return m_CurrentPresetMode; }
	void SetCurrentPresetMode(int val);
	// Slice 렌더링 객체
	vtkSmartPointer<vtkImageActor> GetSliceActor(int sliceType);
	// Slice 렌더링 중비
	void ReadyForSliceRendering();
	// Slide Type과 Index에 따른 Reslide 행렬
	vtkSmartPointer<vtkMatrix4x4> GetResliceMatrix(int sliceType, int sliceIndex);
	// 현재 slide index
	int GetSliceIndex(int sliceType);
	// Slice 인덱스 설정
	void SetSliceIndex(int sliceType, int sliceIndex);
	// CLIPPING
	void ClippingVolume();
	void RestoringVolume();
};

