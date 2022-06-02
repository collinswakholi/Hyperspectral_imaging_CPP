#pragma once


#define POLLING_MS     100 // [ms]
#define ACCELATION     100 
#define MAX_VELOCITY   100 // []
#define HOME_VELOCITY  100


// CTabHardware dialog

class CTabHardware : public CDialogEx
{
	DECLARE_DYNAMIC(CTabHardware)

public:
	CTabHardware(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CTabHardware();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_HW };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

public:
	char serialNo[9] = { 0, };
	int position = 0;
	int velocity = 0;
	int unitStep = 0;
	bool linearStageOpened = false;

	virtual BOOL OnInitDialog();
	CComboBox linearStageList;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	void OnClose();
};
