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
	// Volume Mapper 준비
	// 다양한 방식의 Volume Rendering을 지원하는데, vtkSmartVolumeMapper Class는 HW 지원 사양에 따라 적합한 방식으로 자동 설정해 주는 Mapper이다. 
	vtkSmartPointer<vtkSmartVolumeMapper> smartMapper = vtkSmartPointer<vtkSmartVolumeMapper>::New();
	smartMapper->SetInputData(m_ImageData);
	
	// 투명도/컬러 함수 준비
	double scalarRange[2];
	m_ImageData->GetScalarRange(scalarRange);									// 값 확인 - ANJIN
	m_OpacityFunc = vtkSmartPointer<vtkPiecewiseFunction>::New();
	m_OpacityFunc->AdjustRange(scalarRange);
	m_ColorFunc = vtkSmartPointer<vtkColorTransferFunction>::New();

	// Volume 속성
	vtkSmartPointer<vtkVolumeProperty> volumeProperty = vtkSmartPointer<vtkVolumeProperty>::New();
	volumeProperty->SetScalarOpacity(m_OpacityFunc);
	volumeProperty->SetColor(m_ColorFunc);
	//volumeProperty->ShadeOn();
	volumeProperty->SetInterpolationTypeToLinear();
		
	//// Volume 회전변환
	//double origin[3];
	//m_ImageData->GetOrigin(origin);
	//vtkSmartPointer<vtkTransform> userTransform = vtkSmartPointer<vtkTransform>::New();
	//userTransform->Translate(origin);
	//userTransform->Concatenate(GetOrientation());
	//userTransform->Translate(-origin[0], -origin[1], -origin[2]);
	//userTransform->Update();

	// Volume Rendering 객체 생성
	m_VolumeRendering = vtkSmartPointer<vtkVolume>::New();
	m_VolumeRendering->SetMapper(smartMapper);
	m_VolumeRendering->SetProperty(volumeProperty);
	
	//m_VolumeRendering->SetUserTransform(userTransform);

	// 최대 밝기값 기준 렌더링 모드 준비
	SetCurrentPresetMode(MIP);
}

void VolumeData::SetCurrentPresetMode(int val) {
	// Volume Rendering 준비 여부
	if (m_VolumeRendering == NULL)
		return;
	// 현재모드 설정
	m_CurrentPresetMode = val;
	// Volume Mapper 가져오기
	vtkSmartPointer<vtkSmartVolumeMapper> volumeMapper = vtkSmartVolumeMapper::SafeDownCast(m_VolumeRendering->GetMapper());
	if (volumeMapper == NULL)
		return;

	// 범위
	double scalarRange[2];
	m_ImageData->GetScalarRange(scalarRange);
	double nMin = scalarRange[0];
	double nMax = scalarRange[1];
	// 초기화
	m_ColorFunc->RemoveAllPoints();
	m_OpacityFunc->RemoveAllPoints();

	// 투명도 함수 및 컬러 함수 설정
	switch (m_CurrentPresetMode) {
	case SKIN:
		// 최대 밝기값 기준 연속적인 투명도 함수 설정
		m_ColorFunc->AddRGBPoint(nMin, 0.0, 0.0, 0.0);
		m_ColorFunc->AddRGBPoint(nMax, 1.0, 1.0, 1.0);
		m_OpacityFunc->AddPoint(nMin, 0.0);
		m_OpacityFunc->AddPoint(nMax, 1.0);

		// 최대 밝기 모드로 블렌드 모드 설정
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

		// 조합 모드로 블렌드 모드 설정
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
	// 슬라이스 이미지 렌더링 속성 초기화
	double range[2];
	m_ImageData->GetScalarRange(range);
	// Volume Index 범위
	int ext[6];
	m_ImageData->GetExtent(ext);				// Extent 값 확인 - ANJIN 

	m_SliceProperty = vtkSmartPointer<vtkImageProperty>::New();
	m_SliceProperty->SetColorLevel((range[0] + range[1]) / 2);
	m_SliceProperty->SetColorWindow(range[1] - range[0]);
	
	// 각 슬라이스 타입에 따라 설정
	for (int sliceType = 0; sliceType < 3; sliceType++) {
		// 슬라이스 인덱스의 중간 위치 계산
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

		// Image Reslice 생성
		m_VolumeSlice[sliceType] = vtkSmartPointer<vtkImageReslice>::New();
		m_VolumeSlice[sliceType]->SetInputData(m_ImageData);
		m_VolumeSlice[sliceType]->SetOutputDimensionality(2);		// 2D slice
		m_VolumeSlice[sliceType]->SetResliceAxes(GetResliceMatrix(sliceType, m_SliceIndex[sliceType]));			// 슬라이드 행렬???
		m_VolumeSlice[sliceType]->Update();

		// Image Actor 생성
		m_SliceActor[sliceType] = vtkSmartPointer<vtkImageActor>::New();
		m_SliceActor[sliceType]->GetMapper()->SetInputData(m_VolumeSlice[sliceType]->GetOutput());
		// 각 슬라이스에 공통된 이미지 속성 정의
		m_SliceActor[sliceType]->SetProperty(m_SliceProperty);
	}
}

vtkSmartPointer<vtkMatrix4x4> VolumeData::GetResliceMatrix(int sliceType, int sliceIdx) {
	if (sliceType < 0 || sliceType >= 3)
		return NULL;

	// Slice Type에 따른 Orientation 행렬
	double sliceMat[3][16] = {
		{
			// Axial 축 행렬
			1, 0, 0, 0,
			0, 1, 0, 0, 
			0, 0, 1, 0, 
			0, 0, 0, 1
		},
		{
			// Coronal 축 행렬 - x축 -90도?
			1, 0, 0, 0,
			0, 0, 1, 0,		// -sin(-90)?
			0, -1, 0, 0,	// sin(-90)
			0, 0, 0, 1
		},
		{
			//// Sagittal 축 행렬 - y축 -90도>?
			//0, 0, -1, 0,	// cos(-90), 0, sin(-90)
			//1, 0, 0, 0,		// 환인필요?????  ANJIN
			//0, -1, 0, 0, 
			//0, 0, 0, 1
			0, 0, -1, 0,
			0, 1, 0, 0,
			1, 0, 0, 0,
			0, 0, 0, 1
		}
	};

	// Slice Matrix 생성
	vtkSmartPointer<vtkMatrix4x4> mat = vtkSmartPointer<vtkMatrix4x4>::New();
	mat->DeepCopy(sliceMat[sliceType]);

	// Volume Data 가져오기
	double origin[3];
	double bounds[6];
	int ext[6];
	m_ImageData->GetOrigin(origin);					// Volume 원점
	m_ImageData->GetBounds(bounds);					// Volume 크기
	m_ImageData->GetExtent(ext);					// Volume Index 범위

	// Slice 행렬의 위치를 원점으로 초기화?????
	for (int i = 0; i < 3; i++)
		mat->SetElement(i, 3, origin[3]);

	// Slice Index에 따른 위치 계산
	double pos;
	switch (sliceType) {
	case AXIAL:
		// 슬라이스 범위를 벗어나면 최대/최소값으로 설정
		if (sliceIdx < ext[4]) sliceIdx = ext[4];
		if (sliceIdx > ext[5]) sliceIdx = ext[5];
		// 슬라이스 index의 실제 위치 계산
		pos = bounds[4] + (bounds[5] - bounds[4]) * (double)(sliceIdx / (double)ext[5]);
		// z축 위치에 해당하는 slice 인덱스의 위치로 설정
		mat->SetElement(2, 3, pos);		// 2, 3
		break;
	case CORONAL:
		// 슬라이스 범위를 벗어나면 최대/최소값으로 설정
		if (sliceIdx < ext[2]) sliceIdx = ext[2];
		if (sliceIdx > ext[3]) sliceIdx = ext[3];
		// 슬라이스 인덱스의 실제 위치 계산
		pos = bounds[2] + (bounds[3] - bounds[2]) * (double)(sliceIdx / (double)ext[3]);
		// y축 위치를 해당하는 슬라이스 위치로 설정
		mat->SetElement(1, 3, pos);
		break;
	case SAGITTAL:
		// 슬라이스 범위를 벗어나면 최대/최소값으로 설정
		if (sliceIdx < ext[0])		sliceIdx = ext[0];
		if (sliceIdx > ext[1])		sliceIdx = ext[1];
		// 슬라이스 인덱스의 실제 위치 계산
		pos = bounds[0] + (bounds[1] - bounds[0]) * (double)(sliceIdx / (double)ext[1]);
		// x축 위치를 해당하는 슬라이스 위치로 설정
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

	// 현재 슬라이스 인덱스 설정
	m_SliceIndex[sliceType] = sliceIndex;

	// 슬라이스 행렬 업데이트
	m_VolumeSlice[sliceType]->SetResliceAxes(GetResliceMatrix(sliceType, m_SliceIndex[sliceType]));
	m_VolumeSlice[sliceType]->Update();
}

void VolumeData::ClippingVolume() {
	// Volume Rendering 준비 여부
	if (m_VolumeRendering == NULL) return;

	// Volume Mapper 가져오기
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
	// Volume Rendering 준비 여부
	if (m_VolumeRendering == NULL) return;

	// Volume Mapper 가져오기
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