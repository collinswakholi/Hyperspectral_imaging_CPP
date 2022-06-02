
// HyperImagerDlg.h: 헤더 파일
//

#pragma once

//#include "Graph.h"
#include "PictureCtrl.h"

#include "CTabHardware.h"
#include "CTabStream.h"
#include "CTabHyperCube.h"

#define WM_IMAGE_RECEIVED     (WM_USER + 0)

// CHyperImagerDlg 대화 상자
class CHyperImagerDlg : public CDialogEx
{
// 생성입니다.
public:
	CHyperImagerDlg(CWnd* pParent = nullptr);	// 표준 생성자입니다.

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_HYPERIMAGER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 지원입니다.


// 구현입니다.
protected:
	HICON m_hIcon;

	// 생성된 메시지 맵 함수
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTcnSelchangeTabSection(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDestroy();
	LRESULT updateImage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

	CTabCtrl m_sectionTab;
	CTabHardware m_hwDlg;
	CTabStream m_streamDlg;
	CTabHyperCube m_hcubeDlg;
	CWnd *m_pwndShow = NULL;
	//static UINT streamingThread(LPVOID lParam);
	CWinThread *m_pThread;

public:
	HANDLE m_imageRequestEvent;
	bool m_isWorkingThread = true;
	int imageWidth = 640;
	int imageHeight = 480;
	unsigned char *imageBuffer;
	CStatusBarCtrl m_statusBar;
	afx_msg void OnDeltaposSpinExp(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinPos(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CSliderCtrl linearStagePosSlider;
	CEdit linearStagePosEdit;
	CSpinButtonCtrl linearStagePosSpin;
	CSliderCtrl cameraExpSlider;
	CEdit cameraExpEdit;
	CSpinButtonCtrl cameraExpSpin;
	CStatic linearStagePosStatic;
};
