#ifndef VideoCapturerAdapter_h
#define VideoCapturerAdapter_h

// std
#include <mutex>

// libwebrtc
#include "rtc_base/thread.h"

// this project
#include "VideoSource.h"

// WebRTC-based implementation of VideoCapturer.
class VideoSourceAdapter : public VideoSource
{
public:
	VideoSourceAdapter();
	virtual ~VideoSourceAdapter();
	
	bool Init(const Source& device);
	bool Init(const rtc::scoped_refptr<webrtc::VideoCaptureModule>& module);
	
	webrtc::MediaSourceInterface::SourceState Start(const webrtc::VideoCaptureCapability& requested_cap) override;
	void Stop() override;
	bool IsRunning() override;
	bool IsScreencast() const override { return false; }
	
	RTC_DISALLOW_COPY_AND_ASSIGN(VideoSourceAdapter);

	// Callback when a frame is captured by camera.
	void OnFrame(const webrtc::VideoFrame& frame) override;
	
	// Used to signal captured frames on the same thread as invoked Start().
	// With WebRTC's current VideoCapturer implementations, this will mean a
	// thread hop, but in other implementations (e.g. Chrome) it will be called
	// directly from OnIncomingCapturedFrame.
	void SignalFrameCapturedOnStartThread(const webrtc::VideoFrame& frame);

	bool adaptVideoFormat(const VideoFormat& format) override;
	
protected:
	int32_t getBestCapability(const webrtc::VideoCaptureCapability& requested, webrtc::VideoCaptureCapability& result);
	
	rtc::scoped_refptr<webrtc::VideoCaptureModule> module_;
	std::vector<uint8_t> capture_buffer_;
	rtc::Thread* start_thread_;  // Set in Start(), unset in Stop();
	std::mutex myMutex;
	char* uniqueId_;
	webrtc::VideoCaptureCapability defaultCapability_;
};

#endif /* VideoCapturerAdapter_h */
