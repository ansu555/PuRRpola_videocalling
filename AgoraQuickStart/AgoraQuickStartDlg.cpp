#include "pch.h"
#include "framework.h"
#include "AgoraQuickStart.h"
#include "AgoraQuickStartDlg.h"
#include "afxdialogex.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CAboutDlg dialog used for App "About" menu item
class CAboutDlg : public CDialogEx {
public:
    CAboutDlg();
    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum {
        IDD = IDD_ABOUTBOX
    };
#endif
protected:
    virtual void DoDataExchange(CDataExchange* pDX);
    // DDX/DDV support
    // Implementation
protected:
    DECLARE_MESSAGE_MAP()
};
CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX) {
}
void CAboutDlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
}
BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()
// CAgoraQuickStartDlg dialog for handling main user interactions and callback events
CAgoraQuickStartDlg::CAgoraQuickStartDlg(CWnd* pParent
/*=nullptr*/)
    : CDialog(IDD_AGORAQUICKSTART_DIALOG, pParent) {
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}
CAgoraQuickStartDlg::~CAgoraQuickStartDlg() {
    CDialog::~CDialog();  // Ensure the base class destructor is called
    if (m_isSharingScreen) {
        m_rtcEngine->stopScreenCapture();
    }
    if (m_rtcEngine) {
        m_rtcEngine->release(true);
        m_rtcEngine = NULL;
    }
}

void CAgoraQuickStartDlg::DoDataExchange(CDataExchange* pDX) {
    CDialog::DoDataExchange(pDX);
    // Associate controls and variables for reading and writing data to controls
    DDX_Control(pDX, IDC_EDIT_CHANNEL, m_edtChannelName);
    DDX_Control(pDX, IDC_STATIC_REMOTE, m_staRemote);
    DDX_Control(pDX, IDC_STATIC_LOCAL, m_staLocal);
}
BEGIN_MESSAGE_MAP(CAgoraQuickStartDlg, CDialog)
    // Declare message mappings for handling Windows messages and user events such as joining and leaving channels
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(ID_BTN_JOIN, &CAgoraQuickStartDlg::OnBnClickedBtnJoin)
    ON_BN_CLICKED(ID_BTN_LEAVE, &CAgoraQuickStartDlg::OnBnClickedBtnLeave)
    ON_MESSAGE(WM_MSGID(EID_JOIN_CHANNEL_SUCCESS), CAgoraQuickStartDlg::OnEIDJoinChannelSuccess)
    ON_MESSAGE(WM_MSGID(EID_ERROR), &CAgoraQuickStartDlg::OnEIDError)
    ON_MESSAGE(WM_MSGID(EID_LEAVE_CHANNEL), CAgoraQuickStartDlg::OnEIDLeaveChannel)
    ON_MESSAGE(WM_MSGID(EID_USER_JOINED), &CAgoraQuickStartDlg::OnEIDUserJoined)
    ON_MESSAGE(WM_MSGID(EID_USER_OFFLINE), &CAgoraQuickStartDlg::OnEIDUserOffline)
    ON_EN_CHANGE(IDC_EDIT_CHANNEL, &CAgoraQuickStartDlg::OnEnChangeEditChannel)
    ON_BN_CLICKED(ID_WIZFINISH, &CAgoraQuickStartDlg::OnBnClickedWizfinish)
END_MESSAGE_MAP()
// CAgoraQuickStartDlg message handlers
// Insert your project's App ID obtained from the Agora Console
#define APP_ID "33cceaacefc24f2cae99995c4d172ad5"
// Insert the temporary token obtained from the Agora Console
#define token "007eJxTYHD8KCU84UKs796Oa8KHuia7LGU1WrMsvKzkFtud8nnWy9UUGIyNk5NTExOTU9OSjUzSjJITUy2BwDTZJMXQ3CgxxbRSrT6tIZCRof+qGCsjAwSC+KwMGak5OfkMDABj7B+6"
BOOL CAgoraQuickStartDlg::OnInitDialog() {
    CDialog::OnInitDialog();
    // Add "About..." menu item to the system menu
    // IDM_ABOUTBOX must be in the system command range
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000); 
    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != nullptr) {
        BOOL bNameValid;
        CString strAboutMenu;
        bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
        ASSERT(bNameValid);
        if (!strAboutMenu.IsEmpty()) {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }
    // Set this dialog's icon. The framework will automatically execute this operation when the main application window is not a dialog
    SetIcon(m_hIcon, TRUE);
    // Set big icon
    SetIcon(m_hIcon, FALSE);
    // Set the message receiver
    m_eventHandler.SetMsgReceiver(m_hWnd);
    // Create rtcEngine object
    m_rtcEngine = createAgoraRtcEngine();
    RtcEngineContext context;
    context.appId = APP_ID;
    context.eventHandler = &m_eventHandler;
    // Initialize
    int ret = m_rtcEngine->initialize(context);
    if (ret == 0) {
        m_initialize = true;
    }
    else {
        m_initialize = false;
    }
    // Enable the video module
    m_rtcEngine->enableVideo();
    return TRUE;
    // Unless focus is set to a control, return TRUE
}
void CAgoraQuickStartDlg::OnSysCommand(UINT nID, LPARAM lParam) {
    if ((nID & 0xFFF0) == IDM_ABOUTBOX) {
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else {
        CDialog::OnSysCommand(nID, lParam);
    }
}
void CAgoraQuickStartDlg::OnPaint()
{
}
// If the dialog box has a minimize button, the framework should draw the icon
// This function is called by the framework to obtain the cursor when the user drags the minimized window
HCURSOR CAgoraQuickStartDlg::OnQueryDragIcon() {
    return static_cast<HCURSOR>(m_hIcon);
}
std::string CAgoraQuickStartDlg::cs2utf8(CString str) {
    char szBuf[2 * MAX_PATH] = { 0 };
    WideCharToMultiByte(CP_UTF8, 0, str.GetBuffer(0), str.GetLength(), szBuf, 2 * MAX_PATH, NULL, NULL);
    return szBuf;
}
void CAgoraQuickStartDlg::OnBnClickedBtnJoin() {
    // Join channel
    // Get the channel name
    CString strChannelName;
    m_edtChannelName.GetWindowText(strChannelName);
    if (strChannelName.IsEmpty()) {
        AfxMessageBox(_T("Fill channel name first"));
        return;
    }
    ChannelMediaOptions option;
    // Set channel profile to live broadcasting
    option.channelProfile = CHANNEL_PROFILE_COMMUNICATION;
    // Set user role to broadcaster
    option.clientRoleType = CLIENT_ROLE_BROADCASTER;
    // Automatically subscribe to audio streams in the channel
    option.autoSubscribeAudio = true;
    // Automatically subscribe to video streams in the channel
    option.autoSubscribeVideo = true;
    // Join the channel with the temporary token obtained from the Agora Console
    int ret = m_rtcEngine->joinChannel(token, cs2utf8(strChannelName).c_str(), 0, option);
    // Render local view
    VideoCanvas canvas;
    canvas.renderMode = RENDER_MODE_TYPE::RENDER_MODE_HIDDEN;
    canvas.uid = 0;
    canvas.view = m_staLocal.GetSafeHwnd();
    m_rtcEngine->setupLocalVideo(canvas);
}
void CAgoraQuickStartDlg::OnBnClickedBtnLeave() {
    if (m_isSharingScreen) {
        m_rtcEngine->stopScreenCapture();
        m_isSharingScreen = false;
    }
    m_rtcEngine->leaveChannel();
    // Clear local view
    VideoCanvas canvas;
    canvas.uid = 0;
    m_rtcEngine->setupLocalVideo(canvas);
    m_remoteRender = false;
}

LRESULT CAgoraQuickStartDlg::OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam) {
    // Join channel success callback
    uid_t localUid = wParam;
    return 0;
}
LRESULT CAgoraQuickStartDlg::OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam) {
    // Leave channel callback
    return 0;
}
LRESULT CAgoraQuickStartDlg::OnEIDUserJoined(WPARAM wParam, LPARAM lParam) {
    // Remote user joined callback
    uid_t remoteUid = wParam;
    if (m_remoteRender) {
        return 0;
    }
    // Render remote view
    VideoCanvas canvas;
    canvas.renderMode = RENDER_MODE_TYPE::RENDER_MODE_HIDDEN;
    canvas.uid = remoteUid;
    canvas.view = m_staRemote.GetSafeHwnd();
    m_rtcEngine->setupRemoteVideo(canvas);
    m_remoteRender = true;
    return 0;
}
LRESULT CAgoraQuickStartDlg::OnEIDUserOffline(WPARAM wParam, LPARAM lParam) {
    // Remote user left callback
    uid_t remoteUid = wParam;
    if (!m_remoteRender) {
        return 0;
    }
    // Clear remote view
    VideoCanvas canvas;
    canvas.uid = remoteUid;
    m_rtcEngine->setupRemoteVideo(canvas);
    m_remoteRender = false;
    return 0;
}
LRESULT CAgoraQuickStartDlg::OnEIDError(WPARAM wParam, LPARAM lParam) {
    // Error callback
    return 0;
}


void CAgoraQuickStartDlg::OnEnChangeEditChannel()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CDialog::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
}
 


void CAgoraQuickStartDlg::OnBnClickedButton1()
{

}



void CAgoraQuickStartDlg::OnBnClickedWizfinish()
{
    if (!m_initialize) {
        AfxMessageBox(_T("Please initialize the engine first"));
        return;
    }

    if (!m_isSharingScreen) {
        // Start screen sharing
        agora::rtc::ScreenCaptureParameters captureParams;
        captureParams.dimensions.width = 1920; // Set desired width
        captureParams.dimensions.height = 1080; // Set desired height
        captureParams.frameRate = 15; // Set frame rate
        captureParams.bitrate = 0; // Set bitrate (0 for auto)
        captureParams.captureMouseCursor = true; // Capture mouse cursor
        captureParams.windowFocus = true; // Capture focused window

        m_rtcEngine->startScreenCaptureByDisplayId(0, {}, captureParams);
        AfxMessageBox(_T("Screen sharing started"));
    }
    else {
        // Stop screen sharing
        m_rtcEngine->stopScreenCapture();
        AfxMessageBox(_T("Screen sharing stopped"));
    }

    m_isSharingScreen = !m_isSharingScreen; // Toggle the screen sharing status
}

