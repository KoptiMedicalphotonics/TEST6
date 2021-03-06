// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해
// 추가적으로 제공되는 내용입니다.
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.cpp: CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "IntravascularOCT.h"

#include "MainFrm.h"
#include "ImageViewerManger.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWndEx)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
//	ON_COMMAND(ID_BUTTON_LOAD_IMAGES, &CMainFrame::OnButtonLoadImages)
//	ON_COMMAND(ID_BUTTON_VIEW_CHANGE, &CMainFrame::OnButtonViewChange)
	ON_COMMAND(ID_BUTTON_LOAD_IMAGES, &CMainFrame::OnButtonLoadImages)
	//ON_COMMAND(ID_BUTTON_VIEWPOINT_CHANGE, &CMainFrame::OnButtonViewpointChange)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_VIEWPOINT_CHANGE, &CMainFrame::OnUpdateButtonViewpointChange)
	ON_COMMAND(ID_BUTTON_TEST__, &CMainFrame::OnButtonTest)
END_MESSAGE_MAP()

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
	is_front_or_side_ = true;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;

	// 프레임의 클라이언트 영역을 차지하는 뷰를 만듭니다.
	if (!m_wndView.Create(nullptr, nullptr, AFX_WS_DEFAULT_VIEW, CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, nullptr))
	{
		TRACE0("뷰 창을 만들지 못했습니다.\n");
		return -1;
	}

	m_wndRibbonBar.Create(this);
	m_wndRibbonBar.LoadFromResource(IDR_RIBBON);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("상태 표시줄을 만들지 못했습니다.\n");
		return -1;      // 만들지 못했습니다.
	}

	CString strTitlePane1;
	CString strTitlePane2;
	bNameValid = strTitlePane1.LoadString(IDS_STATUS_PANE1);
	ASSERT(bNameValid);
	bNameValid = strTitlePane2.LoadString(IDS_STATUS_PANE2);
	ASSERT(bNameValid);
	m_wndStatusBar.AddElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE1, strTitlePane1, TRUE), strTitlePane1);
	m_wndStatusBar.AddExtendedElement(new CMFCRibbonStatusBarPane(ID_STATUSBAR_PANE2, strTitlePane2, TRUE), strTitlePane2);
	m_LoadingProgress = new CMFCRibbonProgressBar(IDC_LOADING_STATUS, 400);
	m_wndStatusBar.AddExtendedElement(m_LoadingProgress, L"");

	// Visual Studio 2005 스타일 도킹 창 동작을 활성화합니다.
	CDockingManager::SetDockingMode(DT_SMART);
	// Visual Studio 2005 스타일 도킹 창 자동 숨김 동작을 활성화합니다.
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWndEx::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CMainFrame 진단

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 메시지 처리기

void CMainFrame::OnSetFocus(CWnd* /*pOldWnd*/)
{
	// 뷰 창으로 포커스를 이동합니다.
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// 뷰에서 첫째 크랙이 해당 명령에 나타나도록 합니다.
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// 그렇지 않으면 기본 처리합니다.
	return CFrameWndEx::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CMainFrame::OnButtonLoadImages()
{
	// TODO: Add your command handler code here
	// TODO: 여기에 명령 처리기 코드를 추가합니다.
	//CFolderPickerDialog folderDlg(_T(""), 0, NULL, 0);					// ANJIN
	//
	//if (IDOK == folderDlg.DoModal()) {
	//	CString _path = folderDlg.GetPathName();
	//		
	//	ImageViewerManager::Mgr()->GetImageLoader()->LoadImagesFromFolder(_path);
	//}

	std::vector<CString> path;
	CString szFilters = L"All Files (*.*)|*.*|BMP (*.bmp)|*.bmp|JPG (*.jpg)|*.jpg|PNG (*.png)|*.png||";
	CFileDialog fileDlg(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, szFilters, this);

	// No matter if multiple section is set, it retrieves all selected (full path and) file names in one single buffer, which 
	CString buffer;
	// 경로를 저장하는 포인터
	fileDlg.m_ofn.lpstrFile = buffer.GetBuffer(1000 * 256);	//버퍼 포인터
	fileDlg.m_ofn.nMaxFile = 1000*256;				//버퍼 크기

	if (fileDlg.DoModal() == IDOK) 	{
		for (POSITION pos = fileDlg.GetStartPosition(); pos != NULL;) 	{
			path.push_back(fileDlg.GetNextPathName(pos));
		}
		ImageViewerManager::Mgr()->GetImageLoader()->LoadImagesFromFolder(path);
	}
}


void CMainFrame::OnButtonViewpointChange()
{
	//// TODO: Add your command handler code here
	//ImageViewerManager::Mgr()->ClippingVolume();
}


void CMainFrame::OnUpdateButtonViewpointChange(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable();
}


void CMainFrame::OnButtonTest()
{
	// TODO: Add your command handler code here
	if (is_front_or_side_) {
		ImageViewerManager::Mgr()->ClippingVolume();
		is_front_or_side_ = false;
	}
	else {
		ImageViewerManager::Mgr()->RestoringVolume();
		is_front_or_side_ = true;
	}
}


// Loading file status
void CMainFrame::SetNumImages(int num_images_arg) {

	m_LoadingProgress->SetRange(0, num_images_arg);

}
void CMainFrame::SetCurrentImageIndex(int index_arg) {

	m_LoadingProgress->SetPos(index_arg);

}