#pragma once

#include <vtkObject.h>
#include <vtkSmartPointer.h>

#include "VolumeData.h"

#include <opencv2/opencv.hpp>

class ImageLoader : public vtkObject
{
public:
	vtkTypeMacro(ImageLoader, vtkObject);
	static ImageLoader *New() { return new ImageLoader; }

private:
	vtkSmartPointer<vtkImageData> inputImage_ = vtkSmartPointer<vtkImageData>::New();

protected:
	ImageLoader();
	virtual ~ImageLoader();

	// 현재 선택된 Volume Data?
	vtkSmartPointer<VolumeData> m_VolumeData;

public:
	void LoadImagesFromFolder(std::vector<CString> dirPath);
	// Volume Data
	vtkSmartPointer<VolumeData> GetVolumeData() const { return m_VolumeData; }
};

