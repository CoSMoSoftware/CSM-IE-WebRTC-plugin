//
//  VideoSource.hpp
//  
//
//  Created by cosmo on 16/7/19.
//

#ifndef VideoSource_hpp
#define VideoSource_hpp

// libwebrtc
#include "modules/video_capture/video_capture.h"
#include "media/base/adapted_video_track_source.h"
#include "rtc_base/logging.h"

// this project
#include "Source.h"
#include "VideoFormat.h"

//Abstract class
class VideoSource : public rtc::AdaptedVideoTrackSource, public rtc::VideoSinkInterface<webrtc::VideoFrame>
{
public:
	VideoSource();
	virtual ~VideoSource();
	
	virtual webrtc::MediaSourceInterface::SourceState Start(const webrtc::VideoCaptureCapability& requested_cap) = 0;
	virtual void Stop() = 0;
	virtual bool IsRunning() = 0;
	virtual bool IsScreencast() const = 0;
	bool is_screencast() const override { return IsScreencast(); }
	absl::optional<bool> needs_denoising() const override { return false; }
	bool remote() const override { return false; }
	webrtc::MediaSourceInterface::SourceState state() const override;
	
	// If true, run video adaptation. By default, video adaptation is enabled
	// and users must call video_adapter()->OnOutputFormatRequest()
	// to receive frames.
	bool enable_video_adapter() const { return enable_video_adapter_; }
	void set_enable_video_adapter(bool enable_video_adapter) {
		enable_video_adapter_ = enable_video_adapter;
	}
	
	virtual bool adaptVideoFormat(const VideoFormat& format) = 0;
	
protected:
	RTC_DISALLOW_COPY_AND_ASSIGN(VideoSource);
	
	webrtc::MediaSourceInterface::SourceState state_;
	bool enable_video_adapter_;
};

#endif /* VideoSource_hpp */
