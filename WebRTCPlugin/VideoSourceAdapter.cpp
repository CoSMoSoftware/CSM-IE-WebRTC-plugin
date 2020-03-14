
// ActiveX
#include "stdafx.h"

// std
#include <cstring>

// libwebrtc
#include "rtc_base/logging.h"
#include "modules/video_capture/video_capture_factory.h"
#if defined(  _WIN32 )
#include "rtc_base/win32.h"  // Need this to #include the impl files.
#endif
#include "system_wrappers/include/field_trial.h"
#include "modules/video_capture/video_capture_impl.h"

// this project
#include "VideoSourceAdapter.h"

VideoSourceAdapter::VideoSourceAdapter() : VideoSource(), module_(nullptr), start_thread_(nullptr), uniqueId_(nullptr) {

}

VideoSourceAdapter::~VideoSourceAdapter() {
	if(uniqueId_){
		free(uniqueId_);
	}
	module_ = nullptr;
	start_thread_ = nullptr;
}

bool VideoSourceAdapter::Init(const Source& device) {
	RTC_DCHECK(!start_thread_);
	if (module_) {
		RTC_LOG(LS_ERROR) << "The capturer is already initialized";
		return false;
	}
	
	//id is char[1024]
	uniqueId_ = (char*)malloc(sizeof(char)*1024);
	memcpy(uniqueId_, device.unique_id, 1024);
	
	module_ = webrtc::VideoCaptureFactory::Create(uniqueId_);
	if (!module_) {
		RTC_LOG(LS_ERROR) << "Failed to create capturer for id: " << uniqueId_;
		return false;
	}
	
	std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(webrtc::VideoCaptureFactory::CreateDeviceInfo());
	
	int32_t nb = info->NumberOfCapabilities(uniqueId_);
	defaultCapability_ = webrtc::VideoCaptureCapability();
	if(nb>0){
		info->GetCapability(uniqueId_, 0, defaultCapability_);
	}
	else{
		RTC_LOG(LS_ERROR) << "Failed to find compatible capabilities for device id : " << uniqueId_;
		return false;
	}
	for(int i=0; i<nb; i++){
		webrtc::VideoCaptureCapability cap;
		info->GetCapability(uniqueId_, i, cap);
		RTC_LOG(INFO) << "Capability " << i << ": "
				<< cap.width << "x" << cap.height << "x" << cap.maxFPS
				<< " " << VideoFormat::WebrtcVideoTypeToName(cap.videoType);
	}
	
	RTC_LOG(INFO) << "Video capturer adapter initialized.";
	return true;
}

bool VideoSourceAdapter::Init(const rtc::scoped_refptr<webrtc::VideoCaptureModule>& module) {
	RTC_DCHECK(!start_thread_);
	if (module_) {
		RTC_LOG(LS_ERROR) << "The capturer is already initialized";
		return false;
	}
	if (!module) {
		RTC_LOG(LS_ERROR) << "Invalid VCM supplied";
		return false;
	}
	module_ = module;
	return true;
}

webrtc::MediaSourceInterface::SourceState VideoSourceAdapter::Start(const webrtc::VideoCaptureCapability& requested_cap)
{
  RTC_LOG(LS_INFO) << "VideoSourceAdapter::Start";

  std::unique_lock<std::mutex> lock(myMutex);

  if (!module_) {
	RTC_LOG(LS_ERROR) << "The capturer has not been initialized";
	return webrtc::MediaSourceInterface::kEnded;
  }
  if (start_thread_) {
    RTC_LOG(LS_ERROR) << "The capturer is already running";
    RTC_DCHECK(start_thread_->IsCurrent())
      << "Trying to start capturer on different threads";
    return webrtc::MediaSourceInterface::kLive;
  }

  webrtc::VideoCaptureCapability result;
  if (getBestCapability(requested_cap, result) == -1) {
	RTC_LOG(LS_ERROR) << "Given format isn't valid. Using default one instead.";
	result = defaultCapability_;
  }

  start_thread_ = rtc::Thread::Current();

	module_->RegisterCaptureDataCallback(this);
	if (module_->StartCapture(result) != 0) {
		RTC_LOG(LS_ERROR) << "Camera failed to start.";
		module_->DeRegisterCaptureDataCallback();
		start_thread_ = nullptr;
		return  webrtc::MediaSourceInterface::kEnded;
	}

	RTC_LOG(LS_INFO) << "Camera started with capability: "
			<< "\n- width = " << result.width
			<< "\n- height = " << result.height
			<< "\n- maxFPS = " << result.maxFPS
			<< "\n fourcc = " << VideoFormat::WebrtcVideoTypeToName(result.videoType);

	state_ = webrtc::MediaSourceInterface::kLive;

	return webrtc::MediaSourceInterface::kInitializing;
}

void VideoSourceAdapter::Stop() {
	myMutex.lock();
	RTC_LOG(INFO) << "VideoSourceAdapter::Stop()";

	if (!start_thread_) {
		RTC_LOG(LS_ERROR) << "The capturer is already stopped";
		return;
	}
	RTC_DCHECK(start_thread_);
	RTC_DCHECK(start_thread_->IsCurrent());
	if (IsRunning()) {
		// The module is responsible for OnIncomingCapturedFrame being called, if
		// we stop it we will get no further callbacks.
		module_->StopCapture();
	}
	module_->DeRegisterCaptureDataCallback();

	start_thread_ = nullptr;
	module_ = nullptr;
	state_ = webrtc::MediaSourceInterface::kEnded;
	myMutex.unlock();
}

bool VideoSourceAdapter::IsRunning() {
	return (module_ != NULL && module_->CaptureStarted());
}

void VideoSourceAdapter::OnFrame( const webrtc::VideoFrame& sample) {
	// This can only happen between Start() and Stop().
	RTC_DCHECK(start_thread_);

	rtc::AdaptedVideoTrackSource::OnFrame(sample);
}

bool VideoSourceAdapter::adaptVideoFormat(const VideoFormat& format) {
	if(!IsRunning()){
		RTC_LOG(INFO) << "Capture must be started to adapt its video format (otherwise set video format in QWebRTCProxy).";
		return false;
	}
	if(!enable_video_adapter_){
		RTC_LOG(INFO) << "Video adaptation is disabled.";
		return false;
	}
	
	webrtc::VideoCaptureCapability old = webrtc::VideoCaptureCapability();
	module_->CaptureSettings(old);
	
	webrtc::VideoCaptureCapability requested = format.toCapability();
	webrtc::VideoCaptureCapability result;
	if (getBestCapability(requested, result) == -1) {
		RTC_LOG(LS_ERROR) << "Keeping old format.";
		return  false;
	}
	
	//"Hack" to change the settings
	module_->StopCapture();
	if(module_->StartCapture(result) != 0){
		RTC_LOG(LS_ERROR) << "Camera couldn't start with new format. Restarting with old one.";
		module_->StartCapture(old);
		return  false;
	}
	
	RTC_LOG(LS_INFO) << "Video format modified to: "
			<< "\n- width = " << result.width
			<< "\n- height = " << result.height
			<< "\n- maxFPS = " << result.maxFPS
			<< "\n- videoType = " << VideoFormat::WebrtcVideoTypeToName(result.videoType);
	
	return true;
}

int32_t VideoSourceAdapter::getBestCapability(const webrtc::VideoCaptureCapability& requested, webrtc::VideoCaptureCapability& result)
{
  RTC_LOG(INFO) << "Capability format desired: "
		  << "\n- width = "  << requested.width
		  << "\n- height = " << requested.height
		  << "\n- maxFPS = " << requested.maxFPS
		  << "\n- videoType = " << VideoFormat::WebrtcVideoTypeToName(requested.videoType);

  std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(webrtc::VideoCaptureFactory::CreateDeviceInfo());

  return info->GetBestMatchedCapability(uniqueId_, requested, result);
}
