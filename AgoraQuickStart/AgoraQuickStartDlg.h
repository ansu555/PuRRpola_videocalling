#pragma once
#include <string>
// Include relevant header files
#include <IAgoraRtcEngine.h>


using namespace agora;
using namespace agora::rtc;
using namespace agora::media;
using namespace agora::media::base;

 
// Define message IDs
#define WM_MSGID(code) (WM_USER+0x200+code)
#define EID_ERROR        0x00000001
#define EID_JOIN_CHANNEL_SUCCESS        0x00000002
#define EID_LEAVE_CHANNEL        0x00000003
#define EID_USER_JOINED        0x00000004
#define EID_USER_OFFLINE        0x00000004

// Define CAgoraQuickStartRtcEngineEventHandler class to handle user joining, leaving channel callbacks
class CAgoraQuickStartRtcEngineEventHandler
    : public IRtcEngineEventHandler
{
public:
    // Set the handle of the message receiving window
    void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }

    // Register onJoinChannelSuccess callback
    // Triggered when the local user successfully joins the channel
    virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) {
        if (m_hMsgHanlder) {
            ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOIN_CHANNEL_SUCCESS), uid, 0);
        }
    }

    // Register onUserJoined callback
    // Triggered when a remote host successfully joins the channel
    virtual void onUserJoined(uid_t uid, int elapsed) {
        if (m_hMsgHanlder) {
            ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_JOINED), uid, 0);
        }
    }

    // Register onUserOffline callback
    // Triggered when a remote host leaves the channel or goes offline
    virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) {
        if (m_hMsgHanlder) {
            ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), uid, 0);
        }
    }

    // Register onLeaveChannel callback
    // Triggered when the local host successfully leaves the channel
    virtual void onLeaveChannel(const RtcStats& stats) {
        if (m_hMsgHanlder) {
            ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
        }
    }

    // Register onError callback
    // Triggered when an error occurs during SDK runtime
    virtual void onError(int err, const char* msg) {
        if (m_hMsgHanlder) {
            char* message = new char[1024] {0};
            memcpy(message, msg, strlen(msg));
            ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_ERROR), (WPARAM)message, err);
        }
    }

private:
    HWND m_hMsgHanlder;
};

// CAgoraQuickStartDlg dialog
class CAgoraQuickStartDlg : public CDialog
{
    // Construction
public:
    CAgoraQuickStartDlg(CWnd* pParent = nullptr);        // Standard constructor

    virtual ~CAgoraQuickStartDlg();

    // Dialog Data
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_AGORAQUICKSTART_DIALOG };
#endif

    // Handling join/leave button click events
    afx_msg void OnBnClickedBtnJoin();
    afx_msg void OnBnClickedBtnLeave();
    afx_msg void OnBnClickedButton1();
    // Handling user joining/leaving channel callback events
    afx_msg LRESULT OnEIDJoinChannelSuccess(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDError(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDLeaveChannel(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserJoined(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnEIDUserOffline(WPARAM wParam, LPARAM lParam);

protected:
    HICON m_hIcon;

    CStatic        m_staRemote;
    CStatic        m_staLocal;
    CEdit        m_edtChannelName;

    // DDX/DDV support
    virtual void DoDataExchange(CDataExchange* pDX);
    virtual BOOL OnInitDialog();
    afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
    afx_msg void OnPaint();
    afx_msg HCURSOR OnQueryDragIcon();
    DECLARE_MESSAGE_MAP()

    std::string cs2utf8(CString str);

private:
    IRtcEngine* m_rtcEngine = nullptr;
    CAgoraQuickStartRtcEngineEventHandler m_eventHandler;
    bool m_initialize = false;
    bool m_remoteRender = false;

    
    afx_msg void OnEnChangeEditChannel();
public:
  
};