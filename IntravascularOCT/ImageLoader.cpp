#include "stdafx.h"
#include "ImageLoader.h"
#include "ImageViewerManger.h"


ImageLoader::ImageLoader()
{
}


ImageLoader::~ImageLoader()
{
}

void ImageLoader::LoadImagesFromFolder(std::vector<CString> path) {

	ImageViewerManager::Mgr()->SetNumImages(path.size());
	unsigned char* dptr = NULL, *drow;
	size_t elem_step = 0;
	for (int z = 0; z < path.size(); z++)
	{
		CT2CA T_CT2CA(path[z]);
		// Construct a std::string using the LPCSTR input
		std::string s_filePath(T_CT2CA);
		cv::Mat TmpImgMat = cv::imread(s_filePath, 0);
		if (z == 0) 	{
			inputImage_->SetDimensions(TmpImgMat.cols, TmpImgMat.rows, path.size());
			inputImage_->SetOrigin(0.0, 0.0, 0.0);
			inputImage_->SetSpacing(1.0, 1.0, 2.0);  //?
			inputImage_->AllocateScalars(VTK_UNSIGNED_CHAR, TmpImgMat.channels());

			dptr = reinterpret_cast<unsigned char*>(inputImage_->GetScalarPointer());
			elem_step = inputImage_->GetIncrements()[1] / sizeof(unsigned char);


			drow = dptr + z * elem_step*TmpImgMat.rows;
			memcpy(drow, TmpImgMat.ptr(), elem_step*TmpImgMat.rows);
		}

		drow = dptr + z * elem_step*TmpImgMat.rows;
		memcpy(drow, TmpImgMat.ptr(), elem_step*TmpImgMat.rows);
		ImageViewerManager::Mgr()->SetCurrentImageIndex(z);
	}

	double range[2];
	inputImage_->GetScalarRange(range);

	m_VolumeData = vtkSmartPointer<VolumeData>::New();
	m_VolumeData->SetImageData(inputImage_);

	// Volume 렌더링 준비
	m_VolumeData->ReadyForVolumeRendering();
	// Slice 렌더링 준비
	m_VolumeData->ReadyForSliceRendering();
	ImageViewerManager::Mgr()->UpdateVTKWindows();


	//const int num_ext = 3;
	//std::vector<CString> _file_names[num_ext];			// 3 - JPG, BMP, PNG
	//// 함수
	//CFileFind FileFinder;
	//
	//// 파일 수 알기 위해 ... JPG, BMP, PNG 파일 수 확인 (추가)
	//CString _path = dirPath + _T("\\*.*");
	//BOOL bWorking = FileFinder.FindFile(_path);

	//int _num_files_jpg = 0;
	//int _num_files_bmp = 0;
	//int _num_files_png = 0;
	//while (bWorking) {
	//	bWorking = FileFinder.FindNextFile();
	//	CString _file_name = FileFinder.GetFileName();
	//	if (!(_file_name == _T(".") || _file_name == _T("..") || _file_name == _T("Thumbs.db"))) {

	//		CString _file_ext = PathFindExtension(_file_name);
	//		if (_file_ext == _T(".jpg")) {
	//			_num_files_jpg++;
	//			_file_names[1].push_back(_file_name);
	//		}
	//		else if (_file_ext == _T(".bmp")) {
	//			_num_files_bmp++;
	//			_file_names[0].push_back(_file_name);
	//		}
	//		else if (_file_ext == _T(".png")) {
	//			_num_files_png++;
	//			_file_names[2].push_back(_file_name);
	//		}

	//	}
	//}
	//
	//FileFinder.Close();

	//// 함수
	//// 파일 확장자, 수 확인
	//int _ext_type_index = 0;
	//int _num_files;
	//CString _ext_type;			// 0 - BMP, 1 - JPG, 2 - PNG
	//if (_num_files_bmp > _num_files_jpg) {
	//	if (_num_files_bmp > _num_files_png) {
	//		_ext_type = L".bmp";
	//		_num_files = _num_files_bmp;
	//		_ext_type_index = 0;
	//	}
	//	else {
	//		if (_num_files_jpg > _num_files_png) {
	//			_ext_type = L".jpg";
	//			_num_files = _num_files_jpg;
	//			_ext_type_index = 1;
	//		}
	//		else {
	//			_ext_type = L".png";
	//			_num_files = _num_files_png;
	//			_ext_type_index = 2;
	//		}
	//	}
	//}
	//else {
	//	if (_num_files_jpg > _num_files_png) {
	//		_ext_type = L".jpg";
	//		_num_files = _num_files_jpg;
	//		_ext_type_index = 1;
	//	}
	//	else {
	//		if (_num_files_bmp > _num_files_png) {
	//			_ext_type = L".bmp";
	//			_num_files = _num_files_bmp;
	//			_ext_type_index = 0;
	//		}
	//		else {
	//			_ext_type = L".png";
	//			_num_files = _num_files_png;
	//			_ext_type_index = 2;
	//		}
	//	}
	//}

	//ImageViewerManager::Mgr()->SetNumImages(_num_files);

	//// File Format 변경
	//// 해당 파일명 정렬
	//std::sort(_file_names[_ext_type_index].begin(), _file_names[_ext_type_index].end());
	//unsigned char* _volume_ptr = NULL, *__volume_ptr;
	//size_t _elem_size;
	//for (int i = 0; i < _num_files; i++) {
	//	//std::stringstream ss;
	//	//ss << dirPath << "\\" << _file_names[_ext_type_index][i];
	//	CString __full_name = dirPath + "\\" + _file_names[_ext_type_index][i];
	//	CT2CA _full_name(__full_name);
	//	std::string full_name(_full_name);
	//	cv::Mat _image = cv::imread(full_name, 0);
	//	
	//	if (i == 0) {
	//		inputImage_->SetDimensions(_image.cols, _image.rows, _num_files);
	//		inputImage_->SetOrigin(0.0, 0.0, 0.0);
	//		inputImage_->SetSpacing(1.0, 1.0, 5.0);							// 0.1???? - ANJNI
	//		inputImage_->AllocateScalars(VTK_UNSIGNED_CHAR, _image.channels());

	//		_volume_ptr = reinterpret_cast<unsigned char*>(inputImage_->GetScalarPointer());
	//		_elem_size = inputImage_->GetIncrements()[1] / sizeof(unsigned char);
	//	}

	//	__volume_ptr = _volume_ptr + i * _elem_size*_image.rows;
	//	memcpy(__volume_ptr, _image.ptr(), _elem_size*_image.rows);						// ANJIN

	//	ImageViewerManager::Mgr()->SetCurrentImageIndex(i);
	//}

	//m_VolumeData = vtkSmartPointer<VolumeData>::New();
	//m_VolumeData->SetImageData(inputImage_);
	//
	//// Volume Rendering 준비
	//m_VolumeData->ReadyForVolumeRendering();
	//// Slice Rendering 준비
	//m_VolumeData->ReadyForSliceRendering();

	//ImageViewerManager::Mgr()->UpdateVTKWindows();
}