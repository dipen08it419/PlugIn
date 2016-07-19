/**********************************************************\

  Auto-generated ApexCameraStreamPlugin.cpp

  This file contains the auto-generated main plugin object
  implementation for the ApexCameraStreamPlugin project

  \**********************************************************/

#include "PluginWindow.h"
#include "PluginWindowWin.h"
#include "PluginEvents/MouseEvents.h"
#include "PluginEvents/AttachedEvent.h"
#include "ApexCameraStreamPluginAPI.h"

#include <windows.h>
//#include <gl\gl.h>          // Header File For The OpenGL32 Library
//#include <gl\glu.h>         // Header File For The GLu32 Library

#include "SDL-1.2.15/include/SDL.h"
#include "SDL-1.2.15/include/SDL_thread.h"

extern "C"{
#include <dev/include/libavcodec/avcodec.h>
#include <dev/include/libavutil/avutil.h>
#include <dev/include/libavformat/avformat.h>
#include <dev/include/libswscale/swscale.h>
}

#pragma comment(lib, "dev/lib/avutil.lib")
#pragma comment(lib, "dev/lib/avcodec.lib")
#pragma comment(lib, "dev/lib/avformat.lib")
#pragma comment(lib, "dev/lib/swscale.lib")
#pragma comment(lib, "SDL-1.2.15/lib/x86/SDL.lib")

#include "ApexCameraStreamPlugin.h"

void ApexCameraStreamPlugin::StaticInitialize()
{
	// Place one-time initialization stuff here; As of FireBreath 1.4 this should only
	// be called once per process
}

void ApexCameraStreamPlugin::StaticDeinitialize()
{
	// Place one-time deinitialization stuff here. As of FireBreath 1.4 this should
	// always be called just before the plugin library is unloaded
}

ApexCameraStreamPlugin::ApexCameraStreamPlugin()
{
}

ApexCameraStreamPlugin::~ApexCameraStreamPlugin()
{
	// This is optional, but if you reset m_api (the shared_ptr to your JSAPI
	// root object) and tell the host to free the retained JSAPI objects then
	// unless you are holding another shared_ptr reference to your JSAPI object
	// they will be released here.
	releaseRootJSAPI();
	m_host->freeRetainedObjects();
}

void ApexCameraStreamPlugin::onPluginReady()
{
	// When this is called, the BrowserHost is attached, the JSAPI object is
	// created, and we are ready to interact with the page and such.  The
	// PluginWindow may or may not have already fire the AttachedEvent at
	// this point.
}

void ApexCameraStreamPlugin::shutdown()
{
	// This will be called when it is time for the plugin to shut down;
	// any threads or anything else that may hold a shared_ptr to this
	// object should be released here so that this object can be safely
	// destroyed. This is the last point that shared_from_this and weak_ptr
	// references to this object will be valid
}

FB::JSAPIPtr ApexCameraStreamPlugin::createJSAPI()
{
	// m_host is the BrowserHost
	return boost::make_shared<ApexCameraStreamPluginAPI>(FB::ptr_cast<ApexCameraStreamPlugin>(shared_from_this()), m_host);
}

bool ApexCameraStreamPlugin::onMouseDown(FB::MouseDownEvent *evt, FB::PluginWindow *)
{
	//printf("Mouse down at: %d, %d\n", evt->m_x, evt->m_y);
	return false;
}

bool ApexCameraStreamPlugin::onMouseUp(FB::MouseUpEvent *evt, FB::PluginWindow *)
{
	//printf("Mouse up at: %d, %d\n", evt->m_x, evt->m_y);
	return false;
}

bool ApexCameraStreamPlugin::onMouseMove(FB::MouseMoveEvent *evt, FB::PluginWindow *)
{
	//printf("Mouse move at: %d, %d\n", evt->m_x, evt->m_y);
	return false;
}

bool ApexCameraStreamPlugin::onWindowAttached(FB::AttachedEvent *evt, FB::PluginWindow *piw)
{
	// The window is attached; act appropriately
	
	//FB::PluginWindowWin *wnd = dynamic_cast<FB::PluginWindowWin*>(pluginWindow);
	//EnableOpenGL(wnd->getHWND(), &hDC, &hRC);
	//init();
	
	pluginWindow = piw;
	FB::PluginWindowWin *wnd = dynamic_cast<FB::PluginWindowWin*>(pluginWindow);
	HWND handleWnd = wnd->getHWND();

	char windowid[20] = { 0 };
	char env[100] = { 0 };
	if (handleWnd != NULL) {
		SDL_ulltoa((Uint64)handleWnd, windowid, 10);
		sprintf(env, "SDL_WINDOWID=%s", windowid);
		SDL_putenv(env);
		//return 0;
	}

	m_status = "initialized";

	return false;
}

bool ApexCameraStreamPlugin::onWindowDetached(FB::DetachedEvent *evt, FB::PluginWindow *)
{
	// The window is about to be detached; act appropriately
	m_isPluginDestroyed = true;
	if (m_threadHnd){
		TerminateThread(m_threadHnd, 0);
	}
	pluginWindow = NULL;
	return false;
}


std::string ApexCameraStreamPlugin::getStatus()
{
	return m_status;
}

void ApexCameraStreamPlugin::connectToServer(std::string streamUrl)
{
	if (!m_isConnected){
		m_streamUrl = streamUrl;

		boost::thread connectionThread(boost::bind(&ApexCameraStreamPlugin::connectToStream, this));
		m_threadHnd = connectionThread.native_handle();
	}
}

void ApexCameraStreamPlugin::disconnect()
{
	if (m_isConnected){
		changeStatus("Disconnecting");
		m_receivedDisconnectSignal = true;
	}
}

//code gto connect to camera stream
unsigned int texture;
int i = 0;
bool ApexCameraStreamPlugin::connectToStream(){
	if (!m_isConnected)
	{
		changeStatus("Connecting to " + m_streamUrl);

		//initializing winsocket
		// Register all formats and codecs
		av_register_all();

		//Open video file
		int failureCount = 0,
			connected = false; 
		while (!m_isPluginDestroyed && !connected && failureCount < 100){
			if (avformat_open_input(&pFormatCtx, m_streamUrl.c_str(), NULL, NULL) != 0){
				changeStatus("reconnecting...");
				failureCount++;
				std::this_thread::sleep_for(std::chrono::duration<long long, std::milli>(2000));
			}else{
				connected = true;
			}
		}

		if (!connected){
			changeStatus("Couldn't connect to server");
			return false;
		}

		// Retrieve stream information
		if (avformat_find_stream_info(pFormatCtx, NULL) < 0){
			changeStatus("Couldn't find stream information");
			return false;
		}

		// Find the first video stream
		videoStream = -1;
		for (i = 0; i < pFormatCtx->nb_streams; i++)
			if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
				videoStream = i;
				break;
			}

		if (videoStream == -1){
			changeStatus("Couldn't find video stream");
			return false;
		}

		// Get a pointer to the codec context for the video stream
		pCodecCtxOrig = pFormatCtx->streams[videoStream]->codec;
		// Find the decoder for the video stream
		pCodec = avcodec_find_decoder(pCodecCtxOrig->codec_id);
		if (pCodec == NULL) {
			changeStatus("Codec not found");
			return false;
		}

		// Copy context
		pCodecCtx = avcodec_alloc_context3(pCodec);
		if (avcodec_copy_context(pCodecCtx, pCodecCtxOrig) != 0) {
			changeStatus("Couldn't copy codec context");
			return false;
		}

		// Open codec
		if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0){
			changeStatus("Could not open codec");
			return false;
		}

		// Allocate video frame
		pFrame = av_frame_alloc();

	
		// Determine required buffer size and allocate buffer
		numBytes = avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width,
			pCodecCtx->height);
		buffer = (uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

		// initialize SWS context for software scaling
		sws_ctx = sws_getContext(pCodecCtx->width,
			pCodecCtx->height,
			pCodecCtx->pix_fmt,
			pCodecCtx->width,
			pCodecCtx->height,
			AV_PIX_FMT_RGB24,
			SWS_BILINEAR,
			NULL,
			NULL,
			NULL
			);

		m_isConnected = true;
		m_receivedDisconnectSignal = false;
		changeStatus("Connected");
		
		FB::PluginWindowWin *wnd = dynamic_cast<FB::PluginWindowWin*>(pluginWindow);
		uint32_t screenWidth = wnd->getWindowWidth();
		uint32_t screenHeight = wnd->getWindowHeight();
		SDL_Surface *screen = SDL_SetVideoMode(screenWidth, screenHeight, 0, 0);
		SDL_Overlay *bmp;
		if (screen) {
			bmp = SDL_CreateYUVOverlay(pCodecCtx->width, pCodecCtx->height, SDL_YV12_OVERLAY, screen);
			//bmp = SDL_CreateYUVOverlay(screenWidth, screenHeight, SDL_YV12_OVERLAY, screen);
		}
		//else{
			//changeStatus("SDL ERROR: could not set video mode.");
			//return false;
		//}

		while (!m_isPluginDestroyed && av_read_frame(pFormatCtx, &packet) >= 0) {
			// Is this a packet from the video stream?
			if (packet.stream_index == videoStream) {
				// Decode video frame
				avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished, &packet);

				// Did we get a video frame?
				SDL_Rect rect;
				if (frameFinished && !m_isPluginDestroyed) {
					// Convert the image from its native format to RGB
					/*sws_scale(sws_ctx, (uint8_t const * const *)pFrame->data,
					pFrame->linesize, 0, pCodecCtx->height,
					pFrameRGB->data, pFrameRGB->linesize);*/
					uint32_t cScreenWidth = wnd->getWindowWidth();
					uint32_t cScreenHeight = wnd->getWindowHeight();

					if (screenWidth != cScreenWidth || screenHeight != cScreenHeight){
						screenWidth = cScreenWidth;
						screenHeight = cScreenHeight;

						screen = SDL_SetVideoMode(screenWidth, screenHeight, 0, 0);
						//bmp = SDL_CreateYUVOverlay(screenWidth, screenHeight, SDL_YV12_OVERLAY, screen);
						bmp = SDL_CreateYUVOverlay(pCodecCtx->width, pCodecCtx->height, SDL_YV12_OVERLAY, screen);
					}

					// Convert the image into YUV format that SDL uses
					SDL_LockYUVOverlay(bmp);

					AVPicture pict;
					pict.data[0] = bmp->pixels[0];
					pict.data[1] = bmp->pixels[2];
					pict.data[2] = bmp->pixels[1];

					pict.linesize[0] = bmp->pitches[0];
					pict.linesize[1] = bmp->pitches[2];
					pict.linesize[2] = bmp->pitches[1];

					// Convert the image into YUV format that SDL uses
					img_convert(&pict, AV_PIX_FMT_YUV420P,
						(AVPicture *)pFrame, pCodecCtx->pix_fmt,
						pCodecCtx->width, pCodecCtx->height);

					SDL_UnlockYUVOverlay(bmp);
					rect.x = 0;
					rect.y = 0;
					//rect.w = pCodecCtx->width;
					//rect.h = pCodecCtx->height;					
					rect.w = screenWidth;
					rect.h = screenHeight;
					SDL_DisplayYUVOverlay(bmp, &rect);
				}
			}
		}

		if (!m_isPluginDestroyed){
			// Free the packet that was allocated by av_read_frame
			av_free_packet(&packet);

			// Free the RGB image
			av_free(buffer);
			
			// Free the YUV frame
			av_frame_free(&pFrame);

			// Close the codecs
			avcodec_close(pCodecCtx);
			avcodec_close(pCodecCtxOrig);

			// Close the video file
			avformat_close_input(&pFormatCtx);
		}

		m_isConnected = false;
		m_receivedDisconnectSignal = false;

		changeStatus("Disconnected");
	}
}

int ApexCameraStreamPlugin::img_convert(AVPicture* dst, AVPixelFormat dst_pix_fmt, AVPicture* src, AVPixelFormat pix_fmt, int width, int height)
{

	int av_log = av_log_get_level();
	av_log_set_level(AV_LOG_QUIET);
	FB::PluginWindowWin *wnd = dynamic_cast<FB::PluginWindowWin*>(pluginWindow);
	if (wnd){
		//SwsContext *img_convert_ctx = sws_getContext(width, height, pix_fmt, wnd->getWindowWidth(), wnd->getWindowHeight(), dst_pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);
		SwsContext *img_convert_ctx = sws_getContext(width, height, pix_fmt, width, height, dst_pix_fmt, SWS_BICUBIC, NULL, NULL, NULL);
		int result = sws_scale(img_convert_ctx, src->data, src->linesize, 0, height, dst->data, dst->linesize);
		sws_freeContext(img_convert_ctx);
		av_log_set_level(av_log);
		return result;
	}
	return -1;
}

void ApexCameraStreamPlugin::changeStatus(std::string newStatus){
	m_status = newStatus;

	if (pluginWindow){
		FB::PluginWindowWin *wnd = dynamic_cast<FB::PluginWindowWin*>(pluginWindow);
		wnd->InvalidateWindow();
	}
}

bool ApexCameraStreamPlugin::onDraw(FB::RefreshEvent *evt, FB::PluginWindow* win)
{
#if FB_WIN
	m_isPainting = true;
	FB::Rect pos = win->getWindowPosition();
	HDC hDC;
	FB::PluginWindowWin *wnd = dynamic_cast<FB::PluginWindowWin*>(win);
	PAINTSTRUCT ps;
	if (!wnd) {
		return false;
	}

	hDC = BeginPaint(wnd->getHWND(), &ps);
	pos.right -= pos.left;
	pos.left = 0;
	pos.bottom -= pos.top;
	pos.top = 0;

	//SwapBuffers(hDC);

	::SetTextAlign(hDC, TA_CENTER | TA_BASELINE);
	//LPCTSTR pszText = _T("Apex Camera Stream Plugin");
	std::wstring ws;
	ws.assign(m_status.begin(), m_status.end());
	LPCWSTR pszText = &ws[0];
	::TextOut(hDC, pos.left + (pos.right - pos.left) / 2, pos.top + (pos.bottom - pos.top) / 2, pszText, lstrlen(pszText));

	// Release the device context
	EndPaint(wnd->getHWND(), &ps);

	m_isPainting = false;
#endif
	return true;
}
//
//void ApexCameraStreamPlugin::EnableOpenGL(HWND hwnd, HDC* hdc, HGLRC *hRC)
//{
//	//PIXELFORMATDESCRIPTOR pfd;
//	//int format;
//
//	//// get the device context (DC)
//	//*hdc = GetDC(hwnd);
//
//	//// set the pixel format for the DC
//	//ZeroMemory(&pfd, sizeof(pfd));
//	//pfd.nSize = sizeof(pfd);
//	//pfd.nVersion = 1;
//	//pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
//	//pfd.iPixelType = PFD_TYPE_RGBA;
//	//pfd.cColorBits = 24;
//	//pfd.cDepthBits = 16;
//	//pfd.iLayerType = PFD_MAIN_PLANE;
//	//format = ChoosePixelFormat(*hdc, &pfd);
//	//SetPixelFormat(*hdc, format, &pfd);
//
//	//// create and enable the render context (RC)
//	//*hRC = wglCreateContext(*hdc);
//	//wglMakeCurrent(*hdc, *hRC);
//
//	//hDC = *hdc;
//}
//
//void ApexCameraStreamPlugin::init()
//{
//	//GLubyte pixels[] =
//	//{
//	//	0, 0, 0, 255, 255, 255,
//	//	255, 255, 255, 0, 0, 0,
//	//};
//	//glEnable(GL_TEXTURE_2D);
//	//glGenTextures(1, &texture);
//	//glBindTexture(GL_TEXTURE_2D, texture);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	//glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
//	//glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 2, 2, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
//
//	//glViewport(0, 0, 40, 40);                       // Reset The Current Viewport
//	//glMatrixMode(GL_PROJECTION);                        // Select The Projection Matrix
//	//glLoadIdentity();                                   // Reset The Projection Matrix
//}
