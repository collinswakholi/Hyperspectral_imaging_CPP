
// HyperImagerDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "HyperImager.h"
#include "HyperImagerDlg.h"
#include "afxdialogex.h"

#include "envi.h"
//#include "Graph.h"

#include "CTabHardware.h"
#include "CTabStream.h"
#include "CTabHyperCube.h"

#include <Thorlabs.MotionControl.IntegratedStepperMotors.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 응용 프로그램 정보에 사용되는 CAboutDlg 대화 상자입니다.

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

// 구현입니다.
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

UINT streamingThread(LPVOID lParam)
{
	CHyperImagerDlg *pDlg = (CHyperImagerDlg *)lParam;
	UINT ret = 0;

	HANDLE pEvent = (pDlg->m_imageRequestEvent);

	while (pDlg->m_isWorkingThread) {
		ret = WaitForSingleObject(pEvent, 100);

		if (ret == WAIT_FAILED) { //HANDLE이 Invalid 할 경우
			return 0;
		} else if (ret == WAIT_TIMEOUT) { //TIMEOUT시 명령
			PostMessage(pDlg->m_hWnd, WM_IMAGE_RECEIVED, NULL, NULL/*lTime*/);
			continue;
		} else {
			//Event Object: Signaled 상태
			//프로그램 코드
			PostMessage(pDlg->m_hWnd, WM_IMAGE_RECEIVED, NULL, NULL/*lTime*/);
			ResetEvent(pEvent); //Event Object Nonsignaled 상태로 변환
		}
		Sleep(0);
	}

	return ret;
}

// CHyperImagerDlg 대화 상자

CHyperImagerDlg::CHyperImagerDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_HYPERIMAGER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CHyperImagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_SECTION, m_sectionTab);
	DDX_Control(pDX, IDC_SLIDER_POS, linearStagePosSlider);
	DDX_Control(pDX, IDC_EDIT_POS, linearStagePosEdit);
	DDX_Control(pDX, IDC_SPIN_POS, linearStagePosSpin);
	DDX_Control(pDX, IDC_SLIDER_EXP, cameraExpSlider);
	DDX_Control(pDX, IDC_EDIT_EXP, cameraExpEdit);
	DDX_Control(pDX, IDC_SPIN_EXP, cameraExpSpin);
	DDX_Control(pDX, IDC_STATIC_POS, linearStagePosStatic);
}

BEGIN_MESSAGE_MAP(CHyperImagerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_SECTION, &CHyperImagerDlg::OnTcnSelchangeTabSection)
	ON_WM_DESTROY()
	ON_MESSAGE(WM_IMAGE_RECEIVED, &CHyperImagerDlg::updateImage)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_EXP, &CHyperImagerDlg::OnDeltaposSpinExp)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_POS, &CHyperImagerDlg::OnDeltaposSpinPos)
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CHyperImagerDlg 메시지 처리기

BOOL CHyperImagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 시스템 메뉴에 "정보..." 메뉴 항목을 추가합니다.

	// IDM_ABOUTBOX는 시스템 명령 범위에 있어야 합니다.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 이 대화 상자의 아이콘을 설정합니다.  응용 프로그램의 주 창이 대화 상자가 아닐 경우에는
	//  프레임워크가 이 작업을 자동으로 수행합니다.
	SetIcon(m_hIcon, TRUE);			// 큰 아이콘을 설정합니다.
	SetIcon(m_hIcon, FALSE);		// 작은 아이콘을 설정합니다.

	// TODO: 여기에 추가 초기화 작업을 추가합니다.
	m_sectionTab.InsertItem(1, CString(_T("Hardware Setup")));
	m_sectionTab.InsertItem(2, CString(_T("Streaming Image")));
	m_sectionTab.InsertItem(3, CString(_T("Hyperspectral Cube")));

	CRect rect;
	m_sectionTab.GetClientRect(&rect);

	m_hwDlg.Create(IDD_DIALOG_HW, &m_sectionTab);
	m_hwDlg.SetWindowPos(NULL, 12, 33, rect.Width() - 4, rect.Height() - 25, SWP_SHOWWINDOW | SWP_NOZORDER);
	m_hwDlg.SetParent(this);
	m_pwndShow = &m_hwDlg;

	m_streamDlg.Create(IDD_DIALOG_STREAM, &m_sectionTab);
	m_streamDlg.SetWindowPos(NULL, 12, 33, rect.Width() - 4, rect.Height() - 25, /*SWP_SHOWWINDOW |*/ SWP_NOZORDER);
	m_streamDlg.SetParent(this);

	//m_streamDlg.m_streamCtrl.Load(CString(_T("D:\\Workspace\\HyperImager\\HyperImager\\lena_opencv_gray.jpg")));
	//m_pwndShow = &m_streamDlg;
	unsigned char *buffer = new unsigned char[640 * 480];
	memset(buffer, 0, 640 * 480);
	m_streamDlg.setPicture(buffer, 640, 480);
	m_streamDlg.redrawStream();
	delete[] buffer;

	m_hcubeDlg.Create(IDD_DIALOG_HCUBE, &m_sectionTab);
	m_hcubeDlg.SetWindowPos(NULL, 12, 33, rect.Width() - 4, rect.Height() - 25, /*SWP_SHOWWINDOW |*/ SWP_NOZORDER);
	m_hcubeDlg.SetParent(this);
	//m_pwndShow = &m_hcubeDlg;

	m_hcubeDlg.setHCubeDimension(640, 1000, 480);
	float w_labels[480];
	for (int i = 0; i < 480; ++i) w_labels[i] = i;
	m_hcubeDlg.setWavelengthLabel(w_labels, 480);
	m_hcubeDlg.redrawSpectralImage();

	// TODO: comment after setup HW
	imageBuffer = new unsigned char[imageWidth*imageHeight];
	m_imageRequestEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	m_pThread = AfxBeginThread(streamingThread, this);
	// TODO: uncomment after every unit test is done.
	//SetEvent(m_imageRequestEvent);

	m_statusBar.Create(WS_CHILD | WS_VISIBLE | SBT_OWNERDRAW, CRect(0, 0, 0, 0), this, 0);

	int strPartDim[4] = { 180, 300, 300, 450 - 1 };
	m_statusBar.SetParts(4, strPartDim);
	/*
	m_statusBar.SetText(_T("테스트1"), 0, 0);
	m_statusBar.SetText(_T("테스트2"), 1, 0);

	m_StatusBar.SetText("아이콘", 3, SBT_NOBORDERS);
	m_StatusBar.SetIcon(3,
		SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME),
			FALSE));
	*/
	this->linearStagePosSlider.SetRange(0, 100);
	this->linearStagePosSlider.SetPos(0);
	//this->linearStagePosSlider.EnableWindow(FALSE);
	this->linearStagePosEdit.SetWindowTextW(_T("0"));
	/*
	CSliderCtrl *slider = (CSliderCtrl *)GetDlgItem(IDC_SLIDER_EXP);
	slider->SetRange(0, 100);
	slider->SetPos(0);
	GetDlgItem(IDC_EDIT_EXP)->SetWindowTextW(_T("0"));
	*/
	CSliderCtrl *slider = (CSliderCtrl *)GetDlgItem(IDC_SLIDER_POS);
	slider->SetRange(0, 100);
	slider->SetPos(0);
	GetDlgItem(IDC_EDIT_POS)->SetWindowTextW(_T("0"));

	SetWindowText(_T("HyperImager"));

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

void CHyperImagerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CHyperImagerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		//m_streamDlg.redrawStream();
		CDialogEx::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CHyperImagerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CHyperImagerDlg::OnTcnSelchangeTabSection(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	*pResult = 0;
	if (m_pwndShow != NULL) {
		m_pwndShow->ShowWindow(SW_HIDE);
		m_pwndShow = NULL;
	}

	int nIndex = m_sectionTab.GetCurSel();
	switch (nIndex) {
	case 0:
		m_hwDlg.ShowWindow(SW_SHOW);
		m_pwndShow = &m_hwDlg;
		break;
	case 1:
		m_streamDlg.ShowWindow(SW_SHOW);
		m_pwndShow = &m_streamDlg;
		break;
	case 2:
		m_hcubeDlg.ShowWindow(SW_SHOW);
		m_pwndShow = &m_hcubeDlg;
		break;
	}
}

void CHyperImagerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	// TODO: Add your message handler code here
	m_isWorkingThread = false;
	Sleep(0);
	WaitForSingleObject(m_pThread->m_hThread, 5000);

	delete[] imageBuffer;
}

static int imageCount = 0;

LRESULT CHyperImagerDlg::updateImage(WPARAM wParam, LPARAM lParam)
{
	CString text;
	text.Format(_T("Image Count: %d"), imageCount++);

	//SetDlgItemText(IDC_STATIC_IMAGE_COUNT, text);
	m_statusBar.SetText(text, 0, 0);
	//m_statusBar.SetText(_T("테스트2"), 1, 0);

	unsigned char *buffer = new unsigned char[640 * 480];

	for (int i = 0; i < 480; ++i) {
		for (int j = 0; j < 640; ++j) {
			buffer[i * 640 + j] = i * 255 / 480;
		}
	}

	if (m_pwndShow == &m_streamDlg) {
		this->m_streamDlg.setPicture(buffer, 640, 480);
		this->m_streamDlg.redrawStream();
	}

	if (this->m_hcubeDlg.integrationStarted) {
		this->m_hcubeDlg.addLineSpectrum(buffer, 640, 480);
		if (m_pwndShow == &m_hcubeDlg) 
			this->m_hcubeDlg.redrawSpectralImage();
	}

	delete[] buffer;

	// TODO: uncomment after every unit test is done.
	//SetEvent(this->m_imageRequestEvent);
	return 0;
}

void CHyperImagerDlg::OnDeltaposSpinExp(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	int exp = GetDlgItemInt(IDC_EDIT_EXP);
	int value = exp + pNMUpDown->iDelta;

	CSliderCtrl *slider = (CSliderCtrl *)GetDlgItem(IDC_SLIDER_EXP);
	if (value >= slider->GetRangeMin() && value <= slider->GetRangeMax()) {
		SetDlgItemInt(IDC_EDIT_EXP, value);
		slider->SetPos(value);
		// TODO: set the exposure time of camera
	}

	*pResult = 0;
}


void CHyperImagerDlg::OnDeltaposSpinPos(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	int pos = GetDlgItemInt(IDC_EDIT_POS);

	int new_pos;
	CSliderCtrl* slider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_POS);
	if (pNMUpDown->iDelta > 0) {
		new_pos = min(pos + this->m_hwDlg.unitStep, slider->GetRangeMax());
	} else {
		new_pos = max(pos - this->m_hwDlg.unitStep, slider->GetRangeMin());
	}

	if (pos != new_pos) {
		SetDlgItemInt(IDC_EDIT_POS, new_pos);
		slider->SetPos(new_pos);

		double real_pos;
		ISC_GetRealValueFromDeviceUnit(this->m_hwDlg.serialNo, new_pos, &real_pos, 0); // 0: distance, 1: velocity, 2:accerleration
		
		CString s;
		s.Format(_T("Linear Stage : %.3f [mm]"), real_pos);
		GetDlgItem(IDC_STATIC_POS)->SetWindowTextW(s);

		// TODO: set the position of linear stage
	}
	*pResult = 0;
}


void CHyperImagerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	if (IDC_SLIDER_EXP == pScrollBar->GetDlgCtrlID()) {
		CSliderCtrl *slider = (CSliderCtrl *)GetDlgItem(IDC_SLIDER_EXP);
		CString s;
		s.Format(_T("%d"), slider->GetPos());
		GetDlgItem(IDC_EDIT_EXP)->SetWindowTextW(s);
	}

	if (IDC_SLIDER_POS == pScrollBar->GetDlgCtrlID()) {
		CSliderCtrl* slider = (CSliderCtrl*)GetDlgItem(IDC_SLIDER_POS);
		CString s;
		s.Format(_T("%d"), slider->GetPos());
		GetDlgItem(IDC_EDIT_POS)->SetWindowTextW(s);

		double real_pos;
		ISC_GetRealValueFromDeviceUnit(this->m_hwDlg.serialNo, slider->GetPos(), &real_pos, 0); // 0: distance, 1: velocity, 2:accerleration
		s.Format(_T("Linear Stage : %.3f [mm]"), real_pos);
		GetDlgItem(IDC_STATIC_POS)->SetWindowTextW(s);
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
