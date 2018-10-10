#include "stdafx.h"
#include "VolumeData.h"


VolumeData::VolumeData()
{
	m_CurrentPresetMode = MIP;
	for (int i = 0; i < 3; i++)
		m_SliceIndex[i] = 0;
}


VolumeData::~VolumeData()
{
}

void VolumeData::ReadyForVolumeRendering() {
	// Volume Mapper �غ�
	// �پ��� ����� Volume Rendering�� �����ϴµ�, vtkSmartVolumeMapper Class�� HW ���� ��翡 ���� ������ ������� �ڵ� ������ �ִ� Mapper�̴�. 
	vtkSmartPointer<vtkSmartVolumeMapper> smartMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
	smartMapper->SetInputData(m_ImageData);
	
	// ����/�÷� �Լ� �غ�
	double scalarRange[2];
	m_ImageData->GetScalarRange(scalarRange);									// �� Ȯ�� - ANJIN
	m_OpacityFunc = vtkSmartPointer<vtkPiecewiseFunction>::New();
	m_OpacityFunc->AdjustRange(scalarRange);
	m_ColorFunc = vtkSmartPointer<vtkColorTransferFunction>::New();

	// Volume �Ӽ�
	vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
	volumeProperty->SetScalarOpacity(m_OpacityFunc);
	volumeProperty->SetColor(m_ColorFunc);
	//volumeProperty->ShadeOn();
	volumeProperty->SetInterpolationTypeToLinear();
		
	//// Volume ȸ����ȯ
	//double origin[3];
	//m_ImageData->GetOrigin(origin);
	//vtkSmartPointer<vtkTransform> userTransform = vtkSmartPointer<vtkTransform>::New();
	//userTransform->Translate(origin);
	//userTransform->Concatenate(GetOrientation());
	//userTransform->Translate(-origin[0], -origin[1], -origin[2]);
	//userTransform->Update();

	// Volume Rendering ��ü ����
	m_VolumeRendering = vtkSmartPointer<vtkVolume>::New();
	m_VolumeRendering->SetMapper(smartMapper);
	m_VolumeRendering->SetProperty(volumeProperty);
	
	//m_VolumeRendering->SetUserTransform(userTransform);

	// �ִ� ��Ⱚ ���� ������ ��� �غ�
	SetCurrentPresetMode(MIP);
}

void VolumeData::SetCurrentPresetMode(int val) {
	// Volume Rendering �غ� ����
	if (m_VolumeRendering == NULL)
		return;
	// ������ ����
	m_CurrentPresetMode = val;
	// Volume Mapper ��������
	vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper = vtkSmartVolumeMapper::SafeDownCast(m_VolumeRendering->GetMapper());
	if (volumeMapper == NULL)
		return;

	// ����
	double scalarRange[2];
	m_ImageData->GetScalarRange(scalarRange);
	double nMin = scalarRange[0];
	double nMax = scalarRange[1];
	// �ʱ�ȭ
	m_ColorFunc->RemoveAllPoints();
	m_OpacityFunc->RemoveAllPoints();

	// ���� �Լ� �� �÷� �Լ� ����
	switch (m_CurrentPresetMode) {
	case SKIN:
		// �ִ� ��Ⱚ ���� �������� ���� �Լ� ����
		m_ColorFunc->AddRGBPoint(nMin, 0.0, 0.0, 0.0);
		m_ColorFunc->AddRGBPoint(nMax, 1.0, 1.0, 1.0);
		m_OpacityFunc->AddPoint(nMin, 0.0);
		m_OpacityFunc->AddPoint(nMax, 1.0);

		// �ִ� ��� ���� ���� ��� ����
		volumeMapper->SetBlendModeToMaximumIntensity();
		break;
	case MIP:
		//double dPointA = 50;// 10.0;
		double dPointA = nMin;// 10.0;
		double dPointB = 100;// nMax;// 180.0;
		double dAlpha = 0.5;
		m_ColorFunc->AddRGBPoint(dPointA + (dPointB - dPointA) * (3 / 255.0), 1.0, 1 / 255.0, 0);
		m_ColorFunc->AddRGBPoint(dPointA + (dPointB - dPointA) * (190 / 255.0), 1.0, 235 / 255.0, 38 / 255.0);
		m_ColorFunc->AddRGBPoint(dPointA + (dPointB - dPointA) * (252 / 255.0), 1.0, 253 / 255.0, 251 / 255.0);
		m_OpacityFunc->AddPoint(dPointA + (dPointB - dPointA) * (1 / 255.0), 0.0 * dAlpha);
		m_OpacityFunc->AddPoint(dPointA + (dPointB - dPointA) * (100 / 255.0), 0.15 * dAlpha);
		m_OpacityFunc->AddPoint(dPointA + (dPointB - dPointA) * (144 / 255.0), 0.31 * dAlpha);
		m_OpacityFunc->AddPoint(dPointA + (dPointB - dPointA) * (231 / 255.0), 0.78 * dAlpha);
		m_OpacityFunc->AddPoint(dPointA + (dPointB - dPointA) * (252 / 255.0), 1.0 * dAlpha);

		// ���� ���� ���� ��� ����
		volumeMapper->SetBlendModeToComposite();
		break;
	}

}

vtkSmartPointer<vtkImageActor> VolumeData::GetSliceActor(int sliceType) {
	if (sliceType < 0 || sliceType>3)
		return NULL;

	return m_SliceActor[sliceType];
}

void VolumeData::ReadyForSliceRendering() {
	// �����̽� �̹��� ������ �Ӽ� �ʱ�ȭ
	double range[2];
	m_ImageData->GetScalarRange(range);
	// Volume Index ����
	int ext[6];
	m_ImageData->GetExtent(ext);				// Extent �� Ȯ�� - ANJIN 

	m_SliceProperty = vtkSmartPointer<vtkImageProperty>::New();
	m_SliceProperty->SetColorLevel((range[0] + range[1]) / 2);
	m_SliceProperty->SetColorWindow(range[1] - range[0]);
	
	// �� �����̽� Ÿ�Կ� ���� ����
	for (int sliceType = 0; sliceType < 3; sliceType++) {
		// �����̽� �ε����� �߰� ��ġ ���
		switch (sliceType) {
		case AXIAL:
			m_SliceIndex[sliceType] = (ext[4] + ext[5]) / 2;
			break;
		case CORONAL:
			m_SliceIndex[sliceType] = (ext[2] + ext[3]) / 2;
			break;
		case SAGITTAL:
			m_SliceIndex[sliceType] = (ext[0] + ext[1]) / 2;
			break;
		}

		// Image Reslice ����
		m_VolumeSlice[sliceType] = vtkSmartPointer<vtkImageReslice>::New();
		m_VolumeSlice[sliceType]->SetInputData(m_ImageData);
		m_VolumeSlice[sliceType]->SetOutputDimensionality(2);		// 2D slice
		m_VolumeSlice[sliceType]->SetResliceAxes(GetResliceMatrix(sliceType, m_SliceIndex[sliceType]));			// �����̵� ���???
		m_VolumeSlice[sliceType]->Update();

		// Image Actor ����
		m_SliceActor[sliceType] = vtkSmartPointer<vtkImageActor>::New();
		m_SliceActor[sliceType]->GetMapper()->SetInputData(m_VolumeSlice[sliceType]->GetOutput());
		// �� �����̽��� ����� �̹��� �Ӽ� ����
		m_SliceActor[sliceType]->SetProperty(m_SliceProperty);
	}
}

vtkSmartPointer<vtkMatrix4x4> VolumeData::GetResliceMatrix(int sliceType, int sliceIdx) {
	if (sliceType < 0 || sliceType >= 3)
		return NULL;

	// Slice Type�� ���� Orientation ���
	double sliceMat[3][16] = {
		{
			// Axial �� ���
			1, 0, 0, 0,
			0, 1, 0, 0, 
			0, 0, 1, 0, 
			0, 0, 0, 1
		},
		{
			// Coronal �� ��� - x�� -90��?
			1, 0, 0, 0,
			0, 0, 1, 0,		// -sin(-90)?
			0, -1, 0, 0,	// sin(-90)
			0, 0, 0, 1
		},
		{
			//// Sagittal �� ��� - y�� -90��>?
			//0, 0, -1, 0,	// cos(-90), 0, sin(-90)
			//1, 0, 0, 0,		// ȯ���ʿ�?????  ANJIN
			//0, -1, 0, 0, 
			//0, 0, 0, 1
			0, 0, -1, 0,
			0, 1, 0, 0,
			1, 0, 0, 0,
			0, 0, 0, 1
		}
	};

	// Slice Matrix ����
	vtkSmartPointer<vtkMatrix4x4> mat = vtkSmartPointer<vtkMatrix4x4>::New();
	mat->DeepCopy(sliceMat[sliceType]);

	// Volume Data ��������
	double origin[3];
	double bounds[6];
	int ext[6];
	m_ImageData->GetOrigin(origin);					// Volume ����
	m_ImageData->GetBounds(bounds);					// Volume ũ��
	m_ImageData->GetExtent(ext);					// Volume Index ����

	// Slice ����� ��ġ�� �������� �ʱ�ȭ?????
	for (int i = 0; i < 3; i++)
		mat->SetElement(i, 3, origin[3]);

	// Slice Index�� ���� ��ġ ���
	double pos;
	switch (sliceType) {
	case AXIAL:
		// �����̽� ������ ����� �ִ�/�ּҰ����� ����
		if (sliceIdx < ext[4]) sliceIdx = ext[4];
		if (sliceIdx > ext[5]) sliceIdx = ext[5];
		// �����̽� index�� ���� ��ġ ���
		pos = bounds[4] + (bounds[5] - bounds[4]) * (double)(sliceIdx / (double)ext[5]);
		// z�� ��ġ�� �ش��ϴ� slice �ε����� ��ġ�� ����
		mat->SetElement(2, 3, pos);		// 2, 3
		break;
	case CORONAL:
		// �����̽� ������ ����� �ִ�/�ּҰ����� ����
		if (sliceIdx < ext[2]) sliceIdx = ext[2];
		if (sliceIdx > ext[3]) sliceIdx = ext[3];
		// �����̽� �ε����� ���� ��ġ ���
		pos = bounds[2] + (bounds[3] - bounds[2]) * (double)(sliceIdx / (double)ext[3]);
		// y�� ��ġ�� �ش��ϴ� �����̽� ��ġ�� ����
		mat->SetElement(1, 3, pos);
		break;
	case SAGITTAL:
		// �����̽� ������ ����� �ִ�/�ּҰ����� ����
		if (sliceIdx < ext[0])		sliceIdx = ext[0];
		if (sliceIdx > ext[1])		sliceIdx = ext[1];
		// �����̽� �ε����� ���� ��ġ ���
		pos = bounds[0] + (bounds[1] - bounds[0]) * (double)(sliceIdx / (double)ext[1]);
		// x�� ��ġ�� �ش��ϴ� �����̽� ��ġ�� ����
		mat->SetElement(0, 3, pos);
		break;
	}

	return mat;
}

int VolumeData::GetSliceIndex(int sliceType) {
	if (sliceType < 0 || sliceType >= 3)
		return 0;

	return m_SliceIndex[sliceType];
}

void VolumeData::SetSliceIndex(int sliceType, int sliceIndex) {
	if (sliceType < 0 || sliceType >= 3)
		return;

	// ���� �����̽� �ε��� ����
	m_SliceIndex[sliceType] = sliceIndex;

	// �����̽� ��� ������Ʈ
	m_VolumeSlice[sliceType]->SetResliceAxes(GetResliceMatrix(sliceType, m_SliceIndex[sliceType]));
	m_VolumeSlice[sliceType]->Update();
}

void VolumeData::ClippingVolume() {
	// Volume Rendering �غ� ����
	if (m_VolumeRendering == NULL) return;

	// Volume Mapper ��������
	vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper =
		vtkSmartVolumeMapper::SafeDownCast(m_VolumeRendering->GetMapper());
	if (volumeMapper == NULL) return;

	double center[3];
	m_ImageData->GetCenter(center);
	vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
	plane->SetOrigin(center);
	//plane->SetNormal(-1.0, 1.0, -1.0);
	plane->SetNormal(1.0, 0.0, 0.0);
	volumeMapper->SetBlendModeToComposite();
	volumeMapper->AddClippingPlane(plane);
	
	vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
	volumeProperty->SetScalarOpacity(m_OpacityFunc);
	volumeProperty->SetColor(m_ColorFunc);
	volumeProperty->SetInterpolationTypeToLinear();

	m_VolumeRendering = vtkSmartPointer<vtkVolume>::New();
	m_VolumeRendering->SetMapper(volumeMapper);
	m_VolumeRendering->SetProperty(volumeProperty);

	SetCurrentPresetMode(MIP);
}

void VolumeData::RestoringVolume() {
	// Volume Rendering �غ� ����
	if (m_VolumeRendering == NULL) return;

	// Volume Mapper ��������
	vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper =
		vtkSmartVolumeMapper::SafeDownCast(m_VolumeRendering->GetMapper());
	if (volumeMapper == NULL) return;

	volumeMapper->RemoveAllClippingPlanes();

	vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
	volumeProperty->SetScalarOpacity(m_OpacityFunc);
	volumeProperty->SetColor(m_ColorFunc);
	volumeProperty->SetInterpolationTypeToLinear();

	m_VolumeRendering = vtkSmartPointer<vtkVolume>::New();
	m_VolumeRendering->SetMapper(volumeMapper);
	m_VolumeRendering->SetProperty(volumeProperty);

	SetCurrentPresetMode(MIP);
}