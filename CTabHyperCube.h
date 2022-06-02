#pragma once

#include "PictureCtrl.h"

// CTabHyperCube dialog

class CTabHyperCube : public CDialogEx
{
	DECLARE_DYNAMIC(CTabHyperCube)

public:
	CTabHyperCube(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CTabHyperCube();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_HCUBE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	int lineIndex = 0;
	int sampleSize = 640, lineSize = 1000, wavelengthSize = 480;
	unsigned char *spectralCube = NULL;
	unsigned char *spectralImage = NULL;
	float *wavelengthLabels = NULL;
	Gdiplus::Bitmap *drawableBitmap = NULL;
	CPictureCtrl m_spectralView;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();

public:
	void setHCubeDimension(int samples, int lines, int wavelengths);
	void setWavelengthLabel(float labels[], int len);
	void addLineSpectrum(unsigned char *line_spectro, int samples, int wavelengths);
	void redrawSpectralImage();
	void setSpectroImage(int wavelength_index = 0);
	CComboBox m_wavelengthList;
	afx_msg void OnCbnSelendokComboWavelength();
	afx_msg void OnBnClickedCheckStart();
	bool integrationStarted = false;
};
