#pragma once

#include "PictureCtrl.h"
#include "ChartViewer.h"

// CTabStream dialog

class CTabStream : public CDialogEx
{
	DECLARE_DYNAMIC(CTabStream)

public:
	CTabStream(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CTabStream();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_STREAM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void setPicture(unsigned char *buffer, int w, int h);

	void redrawStream();
private:
	BYTE *m_pImageData = NULL, *m_pBackground = NULL;
	UINT m_imageWidth = 0, m_imageHeight = 0;
	UINT m_ProfileX = 0, m_ProfileY = 0;
	CPictureCtrl m_streamCtrl;
	CChartViewer m_spectralProfileView;
	CChartViewer m_spatialProfileView;
	virtual BOOL OnInitDialog();
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	void setSpatrialProfile(double *value, int n, const char *labels[] = NULL);
	void setSpectralProfile(double *value, int n, const char *labels[] = NULL);
public:
	afx_msg void OnDeltaposSpinXcoord(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDeltaposSpinYcoord(NMHDR *pNMHDR, LRESULT *pResult);
};
