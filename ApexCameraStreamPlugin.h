/**********************************************************\

  Auto-generated ApexCameraStreamPlugin.h

  This file contains the auto-generated main plugin object
  implementation for the ApexCameraStreamPlugin project

\**********************************************************/
#ifndef H_ApexCameraStreamPluginPLUGIN
#define H_ApexCameraStreamPluginPLUGIN

#include "PluginWindow.h"
#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/DrawingEvents.h"
#include "PluginEvents/AttachedEvent.h"
#include <thread>

#include <windows.h>

extern "C"{
#include <dev/include/libavcodec/avcodec.h>
#include <dev/include/libavutil/avutil.h>
#include <dev/include/libavformat/avformat.h>
}

#include "PluginCore.h"


FB_FORWARD_PTR(ApexCameraStreamPlugin)
class ApexCameraStreamPlugin : public FB::PluginCore
{
public:
    static void StaticInitialize();
    static void StaticDeinitialize();

public:
    ApexCameraStreamPlugin();
    virtual ~ApexCameraStreamPlugin();

public:
    void onPluginReady();
    void shutdown();
    virtual FB::JSAPIPtr createJSAPI();
    // If you want your plugin to always be windowless, set this to true
    // If you want your plugin to be optionally windowless based on the
    // value of the "windowless" param tag, remove this method or return
    // FB::PluginCore::isWindowless()
    virtual bool isWindowless() { return false; }
	 

    BEGIN_PLUGIN_EVENT_MAP()
        EVENTTYPE_CASE(FB::MouseDownEvent, onMouseDown, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseUpEvent, onMouseUp, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseMoveEvent, onMouseMove, FB::PluginWindow)
        EVENTTYPE_CASE(FB::MouseMoveEvent, onMouseMove, FB::PluginWindow)
        EVENTTYPE_CASE(FB::AttachedEvent, onWindowAttached, FB::PluginWindow)
        EVENTTYPE_CASE(FB::DetachedEvent, onWindowDetached, FB::PluginWindow)
		  EVENTTYPE_CASE(FB::RefreshEvent, onDraw, FB::PluginWindow)
    END_PLUGIN_EVENT_MAP()

    /** BEGIN EVENTDEF -- DON'T CHANGE THIS LINE **/
    virtual bool onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *);
    virtual bool onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *);
    virtual bool onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *);
    virtual bool onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *);
    virtual bool onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *);
	 virtual bool onDraw(FB::RefreshEvent *evt, FB::PluginWindow*);
    /** END EVENTDEF -- DON'T CHANGE THIS LINE **/
	

public:
	void connectToServer(const std::string);
	void disconnect();
	std::string getStatus();

private:
	FB::PluginWindow * pluginWindow;

	std::string m_status;
	std::string m_streamUrl;

	std::thread::native_handle_type m_threadHnd = NULL;
	bool m_isConnected = false;
	bool m_receivedDisconnectSignal = false;
	bool m_isPluginDestroyed = false;
	bool m_isPainting = false;

	void changeStatus(std::string);
	bool connectToStream();
	void OnPluginDestoryed();
	int img_convert(AVPicture* dst, AVPixelFormat dst_pix_fmt, AVPicture* src, AVPixelFormat pix_fmt, int width, int height);
	//void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC *hRC);
	//void init();

	AVFormatContext   *pFormatCtx = NULL;
	int               i, videoStream;
	AVCodecContext    *pCodecCtxOrig = NULL;
	AVCodecContext    *pCodecCtx = NULL;
	AVCodec           *pCodec = NULL;
	AVFrame           *pFrame = NULL;
	AVFrame           *pFrameRGB = NULL;
	AVPacket          packet;
	int               frameFinished;
	int               numBytes;
	uint8_t           *buffer = NULL;
	struct SwsContext *sws_ctx = NULL;
};


#endif

