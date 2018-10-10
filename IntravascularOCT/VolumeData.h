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
	// �̸� ���ǵ� Volume Rendering ���� �Լ� �� �÷��Լ�
	enum RenderingPreset {MIP, SKIN, INTRAVASCULAR};

protected:
	// VTK 3���� �̹��� ������
	vtkSmartPointer<vtkImageData> m_ImageData;
	// 3���� ȸ���̵� ����
	vtkSmartPointer<vtkMatrix4x4> m_Orientation;
	// Volume Rendering ��ü
	vtkSmartPointer<vtkVolume> m_VolumeRendering;
	// Volume Rendering ���� �Լ�
	vtkSmartPointer<vtkPiecewiseFunction> m_OpacityFunc;
	// Volume Rendering color �Լ�
	vtkSmartPointer<vtkColorTransferFunction> m_ColorFunc;
	// ���� Volume Rendering Mode
	int m_CurrentPresetMode;
	// Slice Rendering ��ü (Axial, Coronal, Sagittal)
	vtkSmartPointer<vtkImageActor> m_SliceActor[3];
	// Slice Rendering�� ���� Volume �ܸ�
	vtkSmartPointer<vtkImageReslice> m_VolumeSlice[3];
	// Slice Rendering �Ӽ�
	vtkSmartPointer<vtkImageProperty> m_SliceProperty;
	// Slice Index
	int m_SliceIndex[3];

public:
	// VTK Volume ������ ��������/�����ϱ�
	vtkSmartPointer<vtkImageData> GetImgeData() const { return m_ImageData; }
	void SetImageData(vtkSmartPointer<vtkImageData> val) { m_ImageData = val; }
	// Volume �������� ȸ���̵� ���� ��������/�����ϱ�
	vtkSmartPointer<vtkMatrix4x4> GetOrientation() const { return m_Orientation; }
	void SetOrientation(vtkSmartPointer<vtkMatrix4x4> val) { m_Orientation = val; }
	// Volume Rendering ��ü
	vtkSmartPointer<vtkVolume> GetVolumeRendering() const { return m_VolumeRendering; }
	// Volume Rendering �غ�
	void ReadyForVolumeRendering();
	// ���� Volume Rendering ��� ��������/�����ϱ�
	int GetCurrentPresetMode() const { return m_CurrentPresetMode; }
	void SetCurrentPresetMode(int val);
	// Slice ������ ��ü
	vtkSmartPointer<vtkImageActor> GetSliceActor(int sliceType);
	// Slice ������ �ߺ�
	void ReadyForSliceRendering();
	// Slide Type�� Index�� ���� Reslide ���
	vtkSmartPointer<vtkMatrix4x4> GetResliceMatrix(int sliceType, int sliceIndex);
	// ���� slide index
	int GetSliceIndex(int sliceType);
	// Slice �ε��� ����
	void SetSliceIndex(int sliceType, int sliceIndex);
	// CLIPPING
	void ClippingVolume();
	void RestoringVolume();
};

