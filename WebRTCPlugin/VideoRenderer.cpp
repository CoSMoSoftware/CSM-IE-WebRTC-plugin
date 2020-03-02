// VideoRenderer.cpp : Implementation of VideoRenderer
#include "stdafx.h"
#include "WebRTCProxy.h"
#include "VideoRenderer.h"
#undef FOURCC
#include "libyuv.h"


HRESULT VideoRenderer::FinalConstruct()
{
	SetThread(WebRTCProxy::GetEventThread());

	//Done
	return S_OK;
}

void VideoRenderer::OnFrame(const webrtc::VideoFrame& frame) 
{
	//Check if size has changed
	if (videoWidth != frame.width() || videoHeight != frame.height())
	{
		//Update
		videoWidth = frame.width();
		videoHeight = frame.height();

		//Fire event
		variant_t width = videoWidth;
		variant_t height = videoWidth;
		DispatchAsync(onresize,width,height);
	}

	//Clone video frame
	auto clone = new webrtc::VideoFrame(frame);
	
	//Store clone as background frame
	mutex.lock();

	frames[background] = std::shared_ptr<webrtc::VideoFrame>(clone);

	//Move background buffer to foreground
	background = !background;

	mutex.unlock();

	//Redraw
	::InvalidateRect(hwndParent, NULL, 0);
}

HRESULT VideoRenderer::OnDrawAdvanced(ATL_DRAWINFO& di)
{
	RECT* rc = (RECT*)di.prcBounds;
	HDC hdc = di.hdcDraw;

	// Create black brush
	HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));

	//Fill rect with black
	FillRect(hdc, rc, hBrush);

	// Delete black brush
	DeleteObject(hBrush);

	//Lock
	mutex.lock();

	//Get foreground buffer
	std::shared_ptr<webrtc::VideoFrame> frame(frames[!background]);

	//Unlock
	mutex.unlock();

	//Check if we have a frame already
	if (!frame)
		return S_OK;

	//Get width and height
	int width  = frame->width();
	int height = frame->height();

	//Get I420 data
	auto yuv = frame->video_frame_buffer()->ToI420();

	//Create ARGB data
	uint8_t* rgb = (uint8_t*)malloc(width*height * 4);

	//Convert to rgb
	libyuv::I420ToARGB(
		yuv->DataY(),
		yuv->StrideY(),
		yuv->DataU(),
		yuv->StrideU(),
		yuv->DataV(),
		yuv->StrideV(),
		rgb,
		width*4,
		width,
		height); 

	//Set streching mode
	int oldMode = SetStretchBltMode(hdc, HALFTONE);

	// Create bitmap
	BITMAPINFO info;
	info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	info.bmiHeader.biWidth = width;
	info.bmiHeader.biHeight = height; 
	info.bmiHeader.biPlanes = 1;
	info.bmiHeader.biBitCount = 32;
	info.bmiHeader.biCompression = BI_RGB;
	info.bmiHeader.biSizeImage = 0;
	info.bmiHeader.biXPelsPerMeter = 0;
	info.bmiHeader.biYPelsPerMeter = 0;
	info.bmiHeader.biClrUsed = 0;
	info.bmiHeader.biClrImportant = 0;

	//Fill rect
	int xDest = rc->left;
	int yDest = rc->bottom;
	int wDest = rc->right - rc->left;
	int hDest = rc->top - rc->bottom;

	//Copy&strech
	StretchDIBits(
		hdc,
		xDest,
		yDest,
		wDest,
		hDest,
		0,
		0,
		width,
		height,
		rgb,
		&info,
		DIB_RGB_COLORS,
		SRCCOPY
	);

	//Clean rgb data
	free(rgb);
	//Restore streching mode
	SetStretchBltMode(hdc, oldMode);

	return S_OK;
}

STDMETHODIMP VideoRenderer::setTrack(VARIANT track)
{
	//Get dispatch interface
	if (track.vt != VT_DISPATCH)
		return E_INVALIDARG;

	IDispatch* disp = V_DISPATCH(&track);

	if (!disp)
		return E_INVALIDARG;

	//Get atl com object from track.
	CComPtr<ITrackAccess> proxy;
	HRESULT hr = disp->QueryInterface(IID_PPV_ARGS(&proxy));
	if (FAILED(hr))
		return hr;
	
	//Convert to video
	webrtc::VideoTrackInterface* videoTrack = reinterpret_cast<webrtc::VideoTrackInterface*>(proxy->GetTrack().get());

	if (!videoTrack)
		return E_INVALIDARG;

	//Add us as video 
	rtc::VideoSinkWants wanted;
	wanted.rotation_applied = true;
	videoTrack->AddOrUpdateSink(this,wanted);

	return S_OK;
}
