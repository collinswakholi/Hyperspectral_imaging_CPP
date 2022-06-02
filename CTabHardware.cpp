// CTabHardware.cpp : implementation file
//

#pragma warning(suppress : 4996)
#pragma comment( lib, "SpecSensor" )

#include "pch.h"
#include "HyperImager.h"
#include "CTabHardware.h"
#include "afxdialogex.h"

#include <SI_sensor.h>
#include <SI_errors.h>

#include <Thorlabs.MotionControl.IntegratedStepperMotors.h>

#include "HyperImagerDlg.h"

// VIS/NIR Camera resource
#define LICENSE_PATH L"C:/Users/Public/Documents/Specim/SpecSensor.lic"
#define VISNIR_CAMERA_NAME _T("FX17e")
SI_H g_hDevice = 0;

int SI_IMPEXP_CONV FeatureCallback1(SI_H Hndl, SI_WC* Feature, void* Context)
{
    if (wcscmp(Feature, L"Camera.ExposureTime") == 0) {
        wprintf(L"FeatureCallback1: Camera.ExposureTime\n");
    } else if (wcscmp(Feature, L"Camera.FrameRate") == 0) {
        wprintf(L"FeatureCallback1: Camera.FrameRate\n");
    }

    return 0;
}


int SI_IMPEXP_CONV FeatureCallback2(SI_H Hndl, SI_WC* Feature, void* Context)
{
    if (wcscmp(Feature, L"Camera.ExposureTime") == 0) {
        wprintf(L"FeatureCallback2: Camera.ExposureTime\n");
    }

    return 0;
}


int SI_IMPEXP_CONV onDataCallback(SI_U8* _pBuffer, SI_64 _nFrameSize, SI_64 _nFrameNumber, void* _pContext)
{
    wprintf(L"%d ", _nFrameNumber);
    return 0;
}

// CTabHardware dialog

IMPLEMENT_DYNAMIC(CTabHardware, CDialogEx)

CTabHardware::CTabHardware(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_HW, pParent)
{

}

CTabHardware::~CTabHardware()
{
    if (this->linearStageOpened) {
        // stop polling
        ISC_StopPolling(this->serialNo);
        Sleep(0);
        // close device
        ISC_Close(this->serialNo);
        Sleep(0);
    }
}

void CTabHardware::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_COMBO_STAGE, linearStageList);
}


BEGIN_MESSAGE_MAP(CTabHardware, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON1, &CTabHardware::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, &CTabHardware::OnBnClickedButton2)
    ON_BN_CLICKED(IDC_BUTTON3, &CTabHardware::OnBnClickedButton3)
END_MESSAGE_MAP()


// CTabHardware message handlers


BOOL CTabHardware::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  Add extra initialization here
    // Scan the camera on the system
    int nError = siNoError;
    SI_64 nDeviceCount = 0;
    SI_WC szDeviceName[4096];
    SI_WC szDeviceDescription[4096];

    int cameraIndex = -1;
    CString s, ss;

    // Load SpecSensor and get the device count
    SI_CHK(SI_Load(LICENSE_PATH));
    SI_CHK(SI_GetInt(SI_SYSTEM, L"DeviceCount", &nDeviceCount));

    // Iterate through each devices to print their name and description
    for (int n = 0; n < nDeviceCount; n++) {
        SI_CHK(SI_GetEnumStringByIndex(SI_SYSTEM, L"DeviceName", n, szDeviceName, 4096));
        SI_CHK(SI_GetEnumStringByIndex(SI_SYSTEM, L"DeviceDescription", n, szDeviceDescription, 4096));
        if (wcscmp(szDeviceName, VISNIR_CAMERA_NAME) == 0) {
            s.Format(_T("Vis/NIR Camera - Device : %d/%d, Name: %s, Description: %s \n"), n, nDeviceCount, szDeviceName, szDeviceDescription);
            cameraIndex = n;
            break;
        }
    }

    SI_U8* pFrameBuffer = 0;
    SI_64 nBufferSize = 0;
    SI_64 nFrameSize = 0;
    SI_64 nFrameNumber = 0;

    if (cameraIndex != -1) {
        // Opens the camera handle
        SI_CHK(SI_Open(cameraIndex, &g_hDevice));
        SI_CHK(SI_Command(g_hDevice, L"Initialize"));

        // Sets frame rate and exposure
        SI_CHK(SI_SetFloat(g_hDevice, L"Camera.FrameRate", 25.0));
        SI_CHK(SI_SetFloat(g_hDevice, L"Camera.ExposureTime", 3.0));

        // Creates a buffer to receive the frame data
        SI_CHK(SI_GetInt(g_hDevice, L"Camera.Image.SizeBytes", &nBufferSize));
        SI_CHK(SI_CreateBuffer(g_hDevice, nBufferSize, (void**)&pFrameBuffer));

        // Starts the acquisition, acquires 100 frames and stops the acquisition
        SI_CHK(SI_Command(g_hDevice, L"Acquisition.Start"));

        for (int n = 0; n < 100; n++) {
            SI_CHK(SI_Wait(g_hDevice, pFrameBuffer, &nFrameSize, &nFrameNumber, 1000));
            // Do something interesting with the frame pointer (pFrameBuffer)
            ss.Format(L"Frame number: %d\n", nFrameNumber);
            s += ss;
        }

        SI_CHK(SI_Command(g_hDevice, L"Acquisition.Stop"));

        SI_CHK(SI_RegisterFeatureCallback(g_hDevice, L"Camera.FrameRate", FeatureCallback1, 0));
        SI_CHK(SI_RegisterFeatureCallback(g_hDevice, L"Camera.ExposureTime", FeatureCallback1, 0));
        SI_CHK(SI_RegisterFeatureCallback(g_hDevice, L"Camera.ExposureTime", FeatureCallback2, 0));
        SI_CHK(SI_RegisterDataCallback(g_hDevice, onDataCallback, 0));

        //wprintf(L"Start acquisition");
        //SI_CHK(SI_Command(g_hDevice, L"Acquisition.Start"));

        //wprintf(L"Stop acquisition");
        //SI_CHK(SI_Command(g_hDevice, L"Acquisition.Stop"));
    }

Error:
    if (SI_FAILED(nError)) {
        ss.Format(L"Error loading Camera: %s\n", SI_GetErrorString(nError));
        s += ss;
    }
    GetDlgItem(IDC_STATIC_STAGE)->SetWindowTextW(s);

    // Cleanups the buffer, closes the camera and unloads SpecSensor
    if (pFrameBuffer != 0) SI_DisposeBuffer(g_hDevice, pFrameBuffer);
    if (g_hDevice != 0) SI_Close(g_hDevice);

    // Unload the library
    SI_CHK(SI_Unload());

    // Scan the linear stage on the system
    if (TLI_BuildDeviceList() == 0) {
        // get device list size 
        short n = TLI_GetDeviceListSize();
        // get LTS serial numbers
        char serialNos[100];
        TLI_GetDeviceListByTypeExt(serialNos, 100, 45);

        // output list of matching devices
        {
            char* searchContext = nullptr;
            char* p = strtok_s(serialNos, ",", &searchContext);

            while (p != nullptr) {
                TLI_DeviceInfo deviceInfo;
                // get device info from device
                TLI_GetDeviceInfo(p, &deviceInfo);
                // get strings from device info structure
                char desc[65];
                strncpy_s(desc, deviceInfo.description, 64);
                desc[64] = '\0';
                char serialNo[9];
                //this->serialNo = strtol(serialNo, NULL);
                strncpy_s(serialNo, deviceInfo.serialNo, 8);
                serialNo[8] = '\0';
                // output
                wchar_t wcs_p[100], wcs_desc[65], wcs_serialNo[9];
                mbstowcs(wcs_p, p, 100);
                mbstowcs(wcs_desc, desc, 65);
                mbstowcs(wcs_serialNo, serialNo, 9);
                this->linearStageList.AddString(wcs_serialNo);

                p = strtok_s(nullptr, ",", &searchContext);
            }
        }
        this->linearStageList.EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
        GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
    } else {
        MessageBox(_T("No linear stage found!"), _T("System Error"), MB_ICONHAND | MB_ICONSTOP | MB_ICONERROR);
    }

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

// Button handler for device opeinging of linear stage
void CTabHardware::OnBnClickedButton1()
{
    // TODO: Add your control notification handler code here
    // Scanning the devices of linear stage on the system
    if (this->linearStageList.GetCount() == 0) {
        MessageBox(_T("No linear stage found!"), _T("System Error"), MB_ICONHAND | MB_ICONSTOP | MB_ICONERROR);
        return;
    }

    // Get ther serial number of selected linear stage
    CString s;
    this->linearStageList.GetLBText(this->linearStageList.GetCurSel(), s);
    wcstombs(this->serialNo, s.GetBuffer(), 9);

    // open device
    if (ISC_Open(this->serialNo) == 0) {
        this->linearStageList.EnableWindow(FALSE);
        GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
        Sleep(200);

        /*if (!ISC_LoadSettings(this->serialNo)) {
            MessageBox(_T("Fail to Load Setting of it!"), _T("System Error"), MB_ICONHAND | MB_ICONSTOP | MB_ICONERROR);
            return;
        }*/

        // start the device polling at 200ms intervals
        ISC_StartPolling(this->serialNo, POLLING_MS);
        //Sleep(POLLING_MS);

        ISC_RequestPosition(this->serialNo);
        Sleep(POLLING_MS);

        CString s1, s2;

        int pos = ISC_GetPosition(this->serialNo);
        int min_pos = ISC_GetStageAxisMinPos(this->serialNo); // in DeviceUnit
        int max_pos = ISC_GetStageAxisMaxPos(this->serialNo); // in DeviceUnit

        double min_real_pos, max_real_pos;
        ISC_GetRealValueFromDeviceUnit(this->serialNo, min_pos, &min_real_pos, 0); // 0: distance, 1: velocity, 2:acceleration
        ISC_GetRealValueFromDeviceUnit(this->serialNo, max_pos, &max_real_pos, 0); // 0: distance, 1: velocity, 2:accerleration

        double minPos, maxPos; // in RealWorldUnit (i.e. unit of mm or degree)
        ISC_GetMotorTravelLimits(this->serialNo, &minPos, &maxPos);

        //assert(min_real_pos == minPos && max_real_pos == maxPos);

        ISC_GetDeviceUnitFromRealValue(this->serialNo, 0.5 /* mm */, &this->unitStep, 0);

        CHyperImagerDlg* parent = (CHyperImagerDlg*)this->GetParent();
        s.Format(_T("Linear Stage : %d [mm]"), pos);
        parent->linearStagePosStatic.SetWindowTextW(s);

        parent->linearStagePosSlider.SetRange(min_pos, max_pos);
        parent->linearStagePosSlider.SetPos(pos);
        parent->linearStagePosSlider.EnableWindow(TRUE);

        s.Format(_T("%d"), pos);
        parent->linearStagePosEdit.SetWindowTextW(s);
        parent->linearStagePosSpin.EnableWindow(TRUE);


        // Setup the parameter of linear stage
        unsigned long home_velocity = ISC_GetHomingVelocity(this->serialNo);
        s.Format(_T("%d"), home_velocity);
        GetDlgItem(IDC_EDIT_HOME_V)->SetWindowTextW(s);

        double home_real_velocity;
        ISC_GetRealValueFromDeviceUnit(this->serialNo, home_velocity, &home_real_velocity, 1); // 0: distance, 1: velocity, 2:acceleration
        s.Format(_T("%.3f [mm/s]"), home_real_velocity);
        GetDlgItem(IDC_STATIC_HOME_V)->SetWindowTextW(s);

        // TODO: uncomment to set up the homing velocity
        //ISC_SetHomingVelocity(this->serialNo, 43980465);

        // 
        int max_velocity, current_acc;
        ISC_GetVelParams(this->serialNo, &current_acc, &max_velocity);
        s.Format(_T("%d"), current_acc);
        GetDlgItem(IDC_EDIT_ACC)->SetWindowTextW(s);

        double real_acc;
        ISC_GetRealValueFromDeviceUnit(this->serialNo, current_acc, &real_acc, 2); // 0: distance, 1: velocity, 2:acceleration
        s.Format(_T("%.3f [mm/s^2]"), real_acc);
        GetDlgItem(IDC_STATIC_ACC)->SetWindowTextW(s);

        s.Format(_T("%d"), max_velocity);
        GetDlgItem(IDC_EDIT_VELOCITY)->SetWindowTextW(s);

        double real_velocity;
        ISC_GetRealValueFromDeviceUnit(this->serialNo, max_velocity, &real_velocity, 1); // 0: distance, 1: velocity, 2:acceleration
        s.Format(_T("%.3f [mm/s]"), real_velocity);
        GetDlgItem(IDC_STATIC_VELOCITY)->SetWindowTextW(s);
        
        GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
        this->linearStageOpened = true;

#if 0
        // Home device
        ISC_ClearMessageQueue(this->serialNo);
        ISC_Home(this->serialNo);
        printf("Device %s homing\r\n", this->serialNo);

        // wait for completion
        WORD messageType;
        WORD messageId;
        DWORD messageData;
        ISC_WaitForMessage(this->serialNo, &messageType, &messageId, &messageData);
        while (messageType != 2 || messageId != 0) {
            ISC_WaitForMessage(this->serialNo, &messageType, &messageId, &messageData);
        }

        // set velocity if desired
        if (velocity > 0) {
            int currentVelocity, currentAcceleration;
            ISC_GetVelParams(this->serialNo, &currentAcceleration, &currentVelocity);
            ISC_SetVelParams(this->serialNo, currentAcceleration, velocity);
        }

        // move to position (channel 1)
        ISC_ClearMessageQueue(this->serialNo);
        ISC_MoveToPosition(this->serialNo, position);
        printf("Device %s moving\r\n", this->serialNo);

        // wait for completion
        ISC_WaitForMessage(this->serialNo, &messageType, &messageId, &messageData);
        while (messageType != 2 || messageId != 1)
        {
            ISC_WaitForMessage(testSerialNo, &messageType, &messageId, &messageData);
        }

        // get actual position
        int pos = ISC_GetPosition(this->serialNo);
        printf("Device %s moved to %d\r\n", this->serialNo, pos);

        // stop polling
        ISC_StopPolling(this->serialNo);
        // close device
        ISC_Close(this->serialNo);
#endif
    } else {
        MessageBox(_T("Linear stage fails to open!"), _T("System Error"), MB_ICONHAND | MB_ICONSTOP | MB_ICONERROR);
        return;
    }

}

// Button Handler for Homing the linear stage
void CTabHardware::OnBnClickedButton2()
{
    // TODO: Add your control notification handler code here
    if (!this->linearStageOpened) return;
    if (!ISC_CanHome(this->serialNo)) return;

    // Home device
    ISC_ClearMessageQueue(this->serialNo);
    ISC_Home(this->serialNo);
    //printf("Device %s homing\r\n", this->serialNo);

    // wait for completion
    WORD messageType;
    WORD messageId;
    DWORD messageData;
    do {
        ISC_WaitForMessage(this->serialNo, &messageType, &messageId, &messageData);
    } while (messageType != 2 || messageId != 0);
}


void CTabHardware::OnBnClickedButton3()
{
    // TODO: Add your control notification handler code here
}


void CTabHardware::OnClose()
{
    //TODO: add message processing code here
    if (this->linearStageOpened) {
        // stop polling
        ISC_StopPolling(this->serialNo);
        Sleep(0);
        // close device
        ISC_Close(this->serialNo);
        Sleep(0);
    }
}