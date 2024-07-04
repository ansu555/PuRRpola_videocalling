
# Agora Video Calling Application with NVIDIA Maxine Integration

## Overview

This project is a video calling application built using the Agora SDK in C++. The application has been further ##aim to integrate NVIDIA Maxine's advanced AR SDK features, such as eye gaze tracking.

## Features

- **Video Calling**: Real-time video calling capabilities using the Agora SDK.
- **NVIDIA Maxine Integration**: Enhanced video communication with advanced features such as eye gaze tracking.
- **Screen Sharing**: Share your screen with other participants during the video call.
- **User Management**: Handle user joining and leaving events efficiently.

## Getting Started

### Prerequisites

- **Visual Studio**: Ensure you have Visual Studio installed (recommended: Visual Studio 2019 or later).
- **Agora SDK**: Download the Agora SDK from the [Agora Developer Portal](https://www.agora.io/en/blog/download/).
- **NVIDIA Maxine AR SDK**: Download the NVIDIA Maxine AR SDK from the [NVIDIA Developer Portal](https://developer.nvidia.com/nvidia-maxine-sdk).

### Installation

1. **Clone the Repository**

   ```sh
   git clone https://github.com/your-username/agora-maxine-video-calling.git
   cd agora-maxine-video-calling
   ```

2. **Setup Agora SDK**

   - Extract the downloaded Agora SDK.
   - Copy the `sdk` folder into the project directory.

3. **Setup NVIDIA Maxine AR SDK**

   - Extract the downloaded NVIDIA Maxine AR SDK.
   - Follow the NVIDIA AR SDK System Guide to properly set up the SDK.

4. **Open the Project**

   - Open the solution file `AgoraQuickStart.sln` in Visual Studio.

5. **Configure App ID and Token**

   - Replace `APP_ID` and `token` placeholders in `AgoraQuickStartDlg.cpp` with your Agora App ID and temporary token.
   
   ```cpp
   #define APP_ID "your-agora-app-id"
   #define token "your-agora-token"
   ```

### Building the Project

1. **Build the Project**

   - In Visual Studio, set the build configuration to `Release` and the target platform to `x64`.
   - Build the solution (Ctrl + Shift + B).

2. **Run the Application**

   - After building the project, run the application (F5).

## Usage

1. **Joining a Channel**

   - Enter the channel name in the provided text box.
   - Click the `Join` button to join the channel.

2. **Leaving a Channel**

   - Click the `Leave` button to leave the channel.

3. **Screen Sharing**

   - Click the `Start/Stop Screen Sharing` button to toggle screen sharing.

4. **NVIDIA Maxine Integration**

   - Ensure the NVIDIA Maxine SDK is properly set up.
   - The eye gaze feature will be automatically applied to the video feed.

## WebRTC Implementation

### Agora SDK Integration

The application utilizes the Agora SDK for WebRTC functionality, enabling real-time video and audio communication. Below is a summary of key components and their roles:

1. **Agora SDK Initialization**

   The Agora SDK is initialized in the `CAgoraQuickStartDlg::OnInitDialog` method. This sets up the SDK context and enables video functionality.

   ```cpp
   BOOL CAgoraQuickStartDlg::OnInitDialog() {
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
       } else {
           m_initialize = false;
       }

       // Enable the video module
       m_rtcEngine->enableVideo();
       return TRUE;
   }
   ```

2. **Joining a Channel**

   When the user clicks the `Join` button, the `OnBnClickedBtnJoin` method is invoked. This method reads the channel name from the UI and uses the Agora SDK to join the specified channel.

   ```cpp
   void CAgoraQuickStartDlg::OnBnClickedBtnJoin() {
       CString strChannelName;
       m_edtChannelName.GetWindowText(strChannelName);
       if (strChannelName.IsEmpty()) {
           AfxMessageBox(_T("Fill channel name first"));
           return;
       }

       ChannelMediaOptions option;
       option.channelProfile = CHANNEL_PROFILE_COMMUNICATION;
       option.clientRoleType = CLIENT_ROLE_BROADCASTER;
       option.autoSubscribeAudio = true;
       option.autoSubscribeVideo = true;

       int ret = m_rtcEngine->joinChannel(token, cs2utf8(strChannelName).c_str(), 0, option);

       VideoCanvas canvas;
       canvas.renderMode = RENDER_MODE_TYPE::RENDER_MODE_HIDDEN;
       canvas.uid = 0;
       canvas.view = m_staLocal.GetSafeHwnd();
       m_rtcEngine->setupLocalVideo(canvas);
   }
   ```

3. **Handling User Events**

   The application handles various user events through the `CAgoraQuickStartRtcEngineEventHandler` class. This class implements the `IRtcEngineEventHandler` interface, which includes callbacks for events such as joining a channel, user joined, user offline, and error events.

   ```cpp
   class CAgoraQuickStartRtcEngineEventHandler : public IRtcEngineEventHandler {
   public:
       void SetMsgReceiver(HWND hWnd) { m_hMsgHanlder = hWnd; }

       virtual void onJoinChannelSuccess(const char* channel, uid_t uid, int elapsed) override {
           if (m_hMsgHanlder) {
               ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_JOIN_CHANNEL_SUCCESS), uid, 0);
           }
       }

       virtual void onUserJoined(uid_t uid, int elapsed) override {
           if (m_hMsgHanlder) {
               ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_JOINED), uid, 0);
           }
       }

       virtual void onUserOffline(uid_t uid, USER_OFFLINE_REASON_TYPE reason) override {
           if (m_hMsgHanlder) {
               ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_USER_OFFLINE), uid, 0);
           }
       }

       virtual void onLeaveChannel(const RtcStats& stats) override {
           if (m_hMsgHanlder) {
               ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_LEAVE_CHANNEL), 0, 0);
           }
       }

       virtual void onError(int err, const char* msg) override {
           if (m_hMsgHanlder) {
               char* message = new char[1024] {0};
               memcpy(message, msg, strlen(msg));
               ::PostMessage(m_hMsgHanlder, WM_MSGID(EID_ERROR), (WPARAM)message, err);
           }
       }

   private:
       HWND m_hMsgHanlder;
   };
   ```

4. **Screen Sharing**

   The application allows users to share their screen by clicking the `Start/Stop Screen Sharing` button, which toggles the screen sharing status using the Agora SDK.

   ```cpp
   void CAgoraQuickStartDlg::OnBnClickedWizfinish() {
       if (!m_initialize) {
           AfxMessageBox(_T("Please initialize the engine first"));
           return;
       }

       if (!m_isSharingScreen) {
           agora::rtc::ScreenCaptureParameters captureParams;
           captureParams.dimensions.width = 1920;
           captureParams.dimensions.height = 1080;
           captureParams.frameRate = 15;
           captureParams.bitrate = 0;
           captureParams.captureMouseCursor = true;
           captureParams.windowFocus = true;

           m_rtcEngine->startScreenCaptureByDisplayId(0, {}, captureParams);
           AfxMessageBox(_T("Screen sharing started"));
       } else {
           m_rtcEngine->stopScreenCapture();
           AfxMessageBox(_T("Screen sharing stopped"));
       }

       m_isSharingScreen = !m_isSharingScreen;
   }
   ```

## Project Structure

- `AgoraQuickStartDlg.h` and `AgoraQuickStartDlg.cpp`: Contains the main dialog class for the application.
- `CAgoraQuickStartRtcEngineEventHandler.h` and `CAgoraQuickStartRtcEngineEventHandler.cpp`: Handles Agora RTC engine events.
- `resource.h`, `stdafx.h`, `stdafx.cpp`: Standard MFC application files.
- `res/`: Contains resources like icons and dialog templates.


## Acknowledgements

- **Agora.io** for providing the real-time communication SDK.
- **NVIDIA** for the advanced AR SDK.

## Contact

For any questions or inquiries, please contact [anik200365@gmail.com](mailto:anik200365@gmail.com).

```

This section provides an overview of how WebRTC functionality is implemented using the Agora SDK in your application. It covers initialization, joining channels, handling user events, and screen sharing, giving users a clear understanding of how the core communication features are integrated and managed.
