// CTabHyperCube.cpp : implementation file
//

#include "pch.h"
#include "HyperImager.h"
#include "CTabHyperCube.h"
#include "afxdialogex.h"

#include "PictureCtrl.h"

// CTabHyperCube dialog

IMPLEMENT_DYNAMIC(CTabHyperCube, CDialogEx)

CTabHyperCube::CTabHyperCube(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_HCUBE, pParent)
{

}

CTabHyperCube::~CTabHyperCube()
{
	if (this->spectralCube) delete[] this->spectralCube;
	if (this->spectralImage) delete[] this->spectralImage;
	if (this->drawableBitmap) delete this->drawableBitmap;
	if (this->wavelengthLabels) delete[] this->wavelengthLabels;
}

void CTabHyperCube::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_HCUBE, m_spectralView);
	DDX_Control(pDX, IDC_COMBO_WAVELENGTH, m_wavelengthList);
}

BEGIN_MESSAGE_MAP(CTabHyperCube, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CTabHyperCube::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CTabHyperCube::OnBnClickedButton2)
	ON_CBN_SELENDOK(IDC_COMBO_WAVELENGTH, &CTabHyperCube::OnCbnSelendokComboWavelength)
	ON_BN_CLICKED(IDC_CHECK_START, &CTabHyperCube::OnBnClickedCheckStart)
END_MESSAGE_MAP()


// CTabHyperCube message handlers

void CTabHyperCube::setHCubeDimension(int samples, int lines, int wavelengths)
{
	if (this->spectralCube) delete[] this->spectralCube;
	if (this->spectralImage) delete[] this->spectralImage;
	if (this->drawableBitmap) delete this->drawableBitmap;
	if (this->wavelengthLabels) delete[] this->wavelengthLabels;

	this->spectralCube = new unsigned char[samples * lines * wavelengths];
	/*
	this->spectralImage = new unsigned char[samples * lines * 4];
	this->drawableBitmap = new Gdiplus::Bitmap(lines, samples, 4 * lines, PixelFormat32bppARGB, this->spectralImage);
	*/
	this->spectralImage = new unsigned char[samples * lines];
	this->drawableBitmap = new Gdiplus::Bitmap(lines, samples, lines, PixelFormat8bppIndexed, this->spectralImage);
	this->wavelengthLabels = new float[wavelengths];

	int palsize = this->drawableBitmap->GetPaletteSize();
	Gdiplus::ColorPalette *palette = (Gdiplus::ColorPalette*)new BYTE[palsize];
	this->drawableBitmap->GetPalette(palette, palsize);
	for (int i = 0; i < palette->Count; i++)
		palette->Entries[i] = Gdiplus::Color::MakeARGB(0xff, i, i, i);
	this->drawableBitmap->SetPalette(palette);
	delete[] palette;

	this->sampleSize = samples;
	this->lineSize = lines;
	this->wavelengthSize = wavelengths;

	/*
	for (int i = 0; i < this->lineSize; ++i) {
		for (int j = 0; j < this->wavelengthSize; ++j) {
			for (int k = 0; k < this->sampleSize; ++k) {
				this->spectralCube[i * wavelengths * samples + j * samples + k] = j * 255 / (this->wavelengthSize - 1);
			}
		}
	}
	*/
	CProgressCtrl *progressbar = (CProgressCtrl *)GetDlgItem(IDC_PROGRESS1);
	progressbar->SetRange(0, lines);

	CString s;
	s.Format(_T("%d"), samples);
	GetDlgItem(IDC_EDIT_SAMPLE)->SetWindowText(s);
	s.Format(_T("%d"), lines);
	GetDlgItem(IDC_EDIT_LINES)->SetWindowText(s);
	s.Format(_T("%d"), wavelengths);
	GetDlgItem(IDC_EDIT_BAND)->SetWindowText(s);

}

void CTabHyperCube::addLineSpectrum(unsigned char *line_spectro, int samples, int wavelengths)
{
	//assert(samples == this->sampleSize);
	//assert(wavelengths == this->wavelengthSize);

	int wavelength_index = this->m_wavelengthList.GetCurSel();
	if (this->lineIndex < this->lineSize) {
		int image_len = samples * wavelengths;
		memcpy(this->spectralCube + image_len * this->lineIndex, line_spectro, image_len);
		for (int i = 0; i < this->sampleSize; ++i) {
			/*
			int value = line_spectro[wavelength_index * samples + i];
			this->spectralImage[(i*this->lineSize + this->lineIndex) * 4 + 3] = 255;
			this->spectralImage[(i*this->lineSize + this->lineIndex) * 4 + 2] = value;
			this->spectralImage[(i*this->lineSize + this->lineIndex) * 4 + 1] = value;
			this->spectralImage[(i*this->lineSize + this->lineIndex) * 4 + 0] = value;
			*/
			this->spectralImage[i*this->lineSize + this->lineIndex] = line_spectro[wavelength_index * samples + i];
		}
		++this->lineIndex;
		this->redrawSpectralImage();
		CProgressCtrl *progressbar = (CProgressCtrl *)GetDlgItem(IDC_PROGRESS1);
		progressbar->SetPos(this->lineIndex);
	} else {
		CButton *button = (CButton *)GetDlgItem(IDC_CHECK_START);
		button->SetCheck(false);
		button->SetWindowTextW(_T("START"));
		this->integrationStarted = false;
	}
}

void CTabHyperCube::setSpectroImage(int wavelength_index)
{
	//int wavelength_index = this->m_wavelengthList.GetCurSel();
	int line_image_len = this->sampleSize * this->wavelengthSize;

	//unsigned char *image_data = new unsigned char[this->sampleSize * this->lineSize * 4];
	for (int i = 0; i < this->sampleSize; ++i) {
		for (int j = 0; j < this->lineSize; ++j) {
			/*
			int value = this->spectralCube[j * line_image_len + wavelength_index * this->sampleSize + i];
			this->spectralImage[(i*this->lineSize + j) * 4 + 3] = 255;
			this->spectralImage[(i*this->lineSize + j) * 4 + 2] = value;
			this->spectralImage[(i*this->lineSize + j) * 4 + 1] = value;
			this->spectralImage[(i*this->lineSize + j) * 4 + 0] = value;
			*/
			this->spectralImage[i*this->lineSize + j] = this->spectralCube[j * line_image_len + wavelength_index * this->sampleSize + i];
		}
	}
}

void CTabHyperCube::redrawSpectralImage()
{
	//Gdiplus::Bitmap bmp(this->lineSize, this->sampleSize, 4 * this->lineSize, PixelFormat32bppARGB, this->spectralImage);
	IStream* istream = nullptr;
	CreateStreamOnHGlobal(NULL, TRUE, &istream);

	CLSID clsid_png;
	CLSIDFromString(L"{557cf406-1a04-11d3-9a73-0000f81ef32e}", &clsid_png);
	//Gdiplus::Status status = bmp.Save(istream, &clsid_png);
	Gdiplus::Status status = this->drawableBitmap->Save(istream, &clsid_png);
	if (status == Gdiplus::Status::Ok)
		this->m_spectralView.Load(istream);

	istream->Release();
}

void CTabHyperCube::setWavelengthLabel(float labels[], int len)
{
	for (int i = 0; i < len; ++i) {
		this->wavelengthLabels[i] = labels[i];
		CString s;
		s.Format(_T("%06.1f nm"), labels[i]);
		this->m_wavelengthList.AddString(s);
	}
	this->m_wavelengthList.SetCurSel(0);
}

void CTabHyperCube::OnCbnSelendokComboWavelength()
{
	// TODO: Add your control notification handler code here
	this->setSpectroImage(this->m_wavelengthList.GetCurSel());
	this->redrawSpectralImage();
}

void CTabHyperCube::OnBnClickedCheckStart()
{
	// TODO: Add your control notification handler code here
	CButton *button = (CButton *)GetDlgItem(IDC_CHECK_START);
	if (button->GetCheck()) {
		button->SetWindowTextW(_T("STOP"));
		GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
		//memset(this->spectralCube, 0, this->sampleSize * this->lineSize * this->wavelengthSize);
		//memset(this->spectralImage, 0, this->sampleSize * this->lineSize);
		//this->lineIndex = 0;
		this->integrationStarted = true;
	} else {
		button->SetWindowTextW(_T("START"));
		GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
		//this->lineIndex = 0;
		this->integrationStarted = false;
	}
}

void CTabHyperCube::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	memset(this->spectralCube, 200, this->sampleSize * this->lineSize * this->wavelengthSize);
	memset(this->spectralImage, 200, this->sampleSize * this->lineSize);
	this->lineIndex = 0;
	// TODO: reset the linear stage 
}

#include "envi.h"

void CTabHyperCube::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	char strFilter[] = { "IMG Files (*.IMG)|*.IMG|" };
	CFileDialog FileDlg(FALSE, CString(_T("*.IMG")), NULL, /*OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY*/NULL, CString(strFilter));

	if (FileDlg.DoModal() == IDOK) {
		CString agendaName = FileDlg.GetFileName(); //filename
		CString agendaPath = FileDlg.GetFolderPath(); //filepath (folders)

		CFileStatus FileOn;
		if (CFile::GetStatus(agendaPath + _T("\\") + agendaName, FileOn)) {
			if (MessageBox(_T("A file with the specified name already exists. Overwrite?"),
				TEXT("File exists"),
				MB_YESNO) != 6) {// user clicked NO (do not overwrite file)
				return;
			}
		}
		envi_header_t *envi_hdr = envi_header_new();
		CString t = _T("ENVI File, Created[") +
			CTime::GetCurrentTime().Format("%Y - %m - %d %H:%M:%S") +
			_T("]");

		char buf[256];
		size_t len;
		wcstombs_s(&len, buf, t.GetBuffer(), t.GetLength());
		envi_header_set_description(envi_hdr, buf);
		envi_header_set_dimension(envi_hdr, this->sampleSize, this->lineSize, this->wavelengthSize);
		envi_header_set_file_type(envi_hdr, "ENVI Standard");
		envi_header_set_sensor_type(envi_hdr, "Specim FX 15");
		for (int i = 0; i < this->wavelengthSize; ++i) {
			sprintf_s(buf, "%.1f", this->wavelengthLabels[i]);
			envi_header_set_wavelength(envi_hdr, i, buf);
		}
		envi_hdr->data_type = 1; // 8bit
		envi_hdr->interleave[0] = 'B';
		envi_hdr->interleave[1] = 'I';
		envi_hdr->interleave[2] = 'L';

		CString full_path = agendaPath + _T("\\") + agendaName + _T(".hdr");
		wcstombs_s(&len, buf, full_path.GetBuffer(), full_path.GetLength());
		envi_header_write_file(envi_hdr, buf);
		envi_header_destroy(envi_hdr);

		full_path = agendaPath + _T("\\") + agendaName;
		CFile file(full_path, CFile::modeWrite | CFile::modeCreate);
		file.Write(spectralCube, this->sampleSize*this->wavelengthSize*this->lineSize);
		file.Close();
		wcstombs_s(&len, buf, full_path.GetBuffer(), full_path.GetLength());

	}
}
