// CTabStream.cpp : implementation file
//

#include "pch.h"
#include "HyperImager.h"
#include "CTabStream.h"
#include "afxdialogex.h"


// CTabStream dialog

IMPLEMENT_DYNAMIC(CTabStream, CDialogEx)

CTabStream::CTabStream(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_STREAM, pParent)
{

}

CTabStream::~CTabStream()
{
}

void CTabStream::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_STREAM, m_streamCtrl);
	DDX_Control(pDX, IDC_STATIC_SPECTRAL, m_spectralProfileView);
	DDX_Control(pDX, IDC_STATIC_SPATIAL, m_spatialProfileView);
}


BEGIN_MESSAGE_MAP(CTabStream, CDialogEx)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_XCOORD, &CTabStream::OnDeltaposSpinXcoord)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_YCOORD, &CTabStream::OnDeltaposSpinYcoord)
END_MESSAGE_MAP()


// CTabStream message handlers


BOOL CTabStream::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here

	double data[] = { 30, 28, 40, 55, 75, 68, 54, 60, 50, 62, 75, 65, 75, 91, 60, 55, 53, 35, 50, 66,
		56, 48, 52, 65, 62 };

	// The labels for the line chart
	const char *labels[] = { "0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "11", "12",
		"13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23", "24" };

	this->setSpatrialProfile(data, 25, labels);
	this->setSpectralProfile(data, 25, labels);

	GetDlgItem(IDC_EDIT_XCOORD)->SetWindowText(_T("0"));
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_XCOORD))->SetRange(0, 640 - 1);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_XCOORD))->SetPos(0);

	GetDlgItem(IDC_EDIT_YCOORD)->SetWindowText(_T("0"));
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_YCOORD))->SetRange(0, 480 - 1);
	((CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_YCOORD))->SetPos(0);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

void CTabStream::setPicture(unsigned char *buffer, int w, int h)
{
	if (m_pBackground == NULL || m_pImageData == NULL || m_imageWidth != w || m_imageHeight != h) {
		if (m_pBackground) delete[] m_pBackground;
		m_pBackground = new BYTE[w * 4 * h];
		if (m_pImageData) delete[] m_pImageData;
		m_pImageData = new BYTE[w * 4 * h];
		m_imageWidth = w;
		m_imageHeight = h;
	}

	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			*(m_pBackground + (i * w + j) * 4 + 3) = 255;
			*(m_pBackground + (i * w + j) * 4 + 2) = *(buffer + i * w + j); // Red
			*(m_pBackground + (i * w + j) * 4 + 1) = *(buffer + i * w + j); // Green
			*(m_pBackground + (i * w + j) * 4 + 0) = *(buffer + i * w + j); // Blue
		}
	}
	/*
	CSpinButtonCtrl *spin;
	spin = (CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_XCOORD);
	spin->SetRange(0, w-1);

	spin = (CSpinButtonCtrl *)GetDlgItem(IDC_SPIN_YCOORD);
	spin->SetRange(0, h-1);
	*/
}

void CTabStream::redrawStream()
{
	memcpy(m_pImageData, m_pBackground, m_imageWidth * 4 * m_imageHeight);

	Gdiplus::Bitmap bmp(m_imageWidth, m_imageHeight, 4 * m_imageWidth, PixelFormat32bppARGB, m_pImageData);

	Gdiplus::Graphics *g = Gdiplus::Graphics::FromImage(&bmp);
	Gdiplus::Pen p(Gdiplus::Color::Red, (float)3);
	p.SetDashStyle(Gdiplus::DashStyle::DashStyleDot);
	g->DrawLine(&p, m_ProfileX, 0, m_ProfileX, m_imageHeight - 1);
	g->DrawLine(&p, 0, m_ProfileY, m_imageWidth - 1, m_ProfileY);
	delete g;

	IStream* istream = nullptr;
	CreateStreamOnHGlobal(NULL, TRUE, &istream);

	CLSID clsid_png;
	CLSIDFromString(L"{557cf406-1a04-11d3-9a73-0000f81ef32e}", &clsid_png);
	Gdiplus::Status status = bmp.Save(istream, &clsid_png);
	if (status == Gdiplus::Status::Ok) {
		m_streamCtrl.Load(istream);
		// Draw spatial profile
		double *data = (double *)malloc(max(m_imageWidth, m_imageHeight) * sizeof(double));
		for (int i = 0; i < m_imageWidth; ++i) 
			data[i] = (double)m_pBackground[(m_ProfileY * m_imageWidth + i) * 4 + 0];
		this->setSpatrialProfile(data, m_imageWidth);

		for (int i = 0; i < m_imageHeight; ++i)
			data[i] = (double)m_pBackground[(i * m_imageWidth + m_ProfileX) * 4 + 0];
		this->setSpectralProfile(data, m_imageHeight);

		delete[] data;
	}
		
	istream->Release();
}

void CTabStream::setSpatrialProfile(double *data, int n, const char *labels[])
{
	CRect rect;
	GetDlgItem(IDC_STATIC_SPATIAL_FRAME)->GetClientRect(&rect);

	XYChart *c = new XYChart(rect.Width(), rect.Height()); //차트의 전체 크기를 정한다
	c->setPlotArea(10, 0, rect.Width() - 20, rect.Height() - 30); //차트위 위치와 높이 넓이를 정한다.
	c->setBackground(0xf0f0f0);

	// Add a line chart layer using the given data
	c->addLineLayer(DoubleArray(data, n), 0xff0000);

	if (labels) {
		c->xAxis()->setLabels(StringArray(labels, n));
		c->xAxis()->setLabelStep(n/10);
	} else {
		char **num_labels = new char*[n];
		
		char buf[256];
		for (int i = 0; i < n; ++i) {
			sprintf_s(buf, "%d", i);
			num_labels[i] = _strdup(buf);
		}
		c->xAxis()->setLabels(StringArray(num_labels, n));
		c->xAxis()->setLabelStep(n/10);
		//c->xAxis()->setTickDensity(10);

		for (int i = 0; i < n; ++i) free(num_labels[i]);
		delete[] num_labels;
	}
	c->yAxis()->setLabelStyle("", 0);
	c->yAxis()->setLinearScale(0, 255);
	c->yAxis()->setTickDensity(10);

	m_spatialProfileView.setChart(c);  //m_chartView에 Chart를 보여주기 위한 코드
	delete c;
}

void CTabStream::setSpectralProfile(double *data, int n, const char *labels[])
{
	CRect rect;
	GetDlgItem(IDC_STATIC_SPECTRAL_FRAME)->GetClientRect(&rect);

	XYChart *c = new XYChart(rect.Width(), rect.Height());
	c->setPlotArea(0, 12, rect.Width() - 35, rect.Height() - 25);
	c->setBackground(0xf0f0f0);

	// Add a line chart layer using the given data
	c->addLineLayer(DoubleArray(data, n), 0xff0000);
	if (labels) {
		c->xAxis()->setLabels(StringArray(labels, n));
		c->xAxis()->setLabelStep(n/10);
	} else {
		char **num_labels = new char*[n];

		char buf[256];
		for (int i = 0; i < n; ++i) {
			sprintf_s(buf, "%d", i);
			num_labels[i] = _strdup(buf);
		}
		c->xAxis()->setLabels(StringArray(num_labels, n));
		c->xAxis()->setLabelStep(n/10);
		//c->xAxis()->setTickDensity(10);

		for (int i = 0; i < n; ++i) free(num_labels[i]);
		delete[] num_labels;
	}
	c->yAxis()->setLabelStyle("", 0);
	c->yAxis()->setLinearScale(0, 255);
	c->yAxis()->setTickDensity(10);

	c->swapXY();
	c->setYAxisOnRight();
	c->setXAxisOnTop();
	// Reverse the x axis so it is pointing downwards
	c->xAxis()->setReverse();
	c->yAxis()->setReverse();

	m_spectralProfileView.setChart(c);
	delete c;
}

void CTabStream::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	GetDlgItem(IDC_STATIC_STREAM)->GetWindowRect(&rect);
	this->ScreenToClient(&rect);
	if (PtInRect(&rect, point)) {
		CPoint client_point = point - rect.TopLeft();
		client_point.x = client_point.x * m_imageWidth / rect.Width();
		client_point.y = client_point.y * m_imageHeight / rect.Height();
		CString s;
		s.Format(_T("(%04d, %04d)"), client_point.x, client_point.y);
		GetDlgItem(IDC_STATIC_XYCOORD)->SetWindowText(s);
	}

	CDialogEx::OnMouseMove(nFlags, point);
}

void CTabStream::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	CRect rect;
	GetDlgItem(IDC_STATIC_STREAM)->GetWindowRect(&rect);
	this->ScreenToClient(&rect);
	if (PtInRect(&rect, point)) {
		if (m_pImageData) {
			CPoint client_point = point - rect.TopLeft();
			m_ProfileX = client_point.x * m_imageWidth / rect.Width();
			m_ProfileY = client_point.y * m_imageHeight / rect.Height();
			this->redrawStream();

			CString s;
			s.Format(_T("%d"), m_ProfileX);
			GetDlgItem(IDC_EDIT_XCOORD)->SetWindowTextW(s);
			s.Format(_T("%d"), m_ProfileY);
			GetDlgItem(IDC_EDIT_YCOORD)->SetWindowTextW(s);
		}
	}
	CDialogEx::OnLButtonDown(nFlags, point);
}


void CTabStream::OnDeltaposSpinXcoord(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	if (m_pImageData) {
		int value = m_ProfileX + pNMUpDown->iDelta;

		if (value >= 0 && value < m_imageWidth) {
			CString s;
			s.Format(_T("%d"), value);
			GetDlgItem(IDC_EDIT_XCOORD)->SetWindowTextW(s);
			m_ProfileX = value;
			this->redrawStream();
		}
	}

	*pResult = 0;
}

void CTabStream::OnDeltaposSpinYcoord(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);
	// TODO: Add your control notification handler code here
	if (m_pImageData) {
		int value = m_ProfileY + pNMUpDown->iDelta;

		if (value >= 0 && value < m_imageHeight) {
			CString s;
			s.Format(_T("%d"), value);
			GetDlgItem(IDC_EDIT_YCOORD)->SetWindowTextW(s);
			m_ProfileY = value;
			this->redrawStream();
		}
	}
	*pResult = 0;
}
