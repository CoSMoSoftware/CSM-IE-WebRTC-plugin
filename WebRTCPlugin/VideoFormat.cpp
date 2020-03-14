#include "stdafx.h"

#include "VideoFormat.h"

#include "rtc_base/arraysize.h"

static webrtc::VideoType VideoFormatTypeToCapabilityType(VideoFormat::VideoType videoType) {
  return static_cast<webrtc::VideoType>(videoType);
}

static VideoFormat::VideoType CapabilityTypeToVideoFormatType(webrtc::VideoType webrtcType) {
  return static_cast<VideoFormat::VideoType>(webrtcType);
}

struct VideoTypeName {
	const char* name;
	VideoFormat::VideoType videoType;
};

static VideoTypeName kSupportedVideoTypes[] = {
	{"I420", VideoFormat::VideoType::kI420},   // 12 bpp, no conversion.
	{"YV12", VideoFormat::VideoType::kYV12},   // 12 bpp, no conversion.
	{"YUY2", VideoFormat::VideoType::kYUY2},   // 16 bpp, fast conversion.
	{"UYVY", VideoFormat::VideoType::kUYVY},   // 16 bpp, fast conversion.
	{"NV12", VideoFormat::VideoType::kNV12},   // 12 bpp, fast conversion.
	{"NV21", VideoFormat::VideoType::kNV21},   // 12 bpp, fast conversion.
	{"MJPG", VideoFormat::VideoType::kMJPEG},  // compressed, slow conversion.
	{"ARGB", VideoFormat::VideoType::kARGB},   // 32 bpp, slow conversion.
	{"24BG", VideoFormat::VideoType::kRGB24},  // 24 bpp, slow conversion.
	{"BGRA", VideoFormat::VideoType::kBGRA},
	{"IYUV", VideoFormat::VideoType::kIYUV},
	{"ABGR", VideoFormat::VideoType::kABGR},
	{"RGBP", VideoFormat::VideoType::kRGB565},
	{"R444", VideoFormat::VideoType::kARGB4444},
	{"RGBO", VideoFormat::VideoType::kARGB1555},
	{"ABGR", VideoFormat::VideoType::kABGR},
	{"ANY", VideoFormat::VideoType::kUnknown}
};

VideoFormat::VideoFormat(int width, int height, int fps, const VideoFormat::VideoType& videoType):
  width(width), height(height), fps(fps), videoType(videoType) {
}

VideoFormat::VideoFormat(const webrtc::VideoCaptureCapability& cap):
  width(cap.width), height(cap.height), fps(cap.maxFPS),
  videoType(CapabilityTypeToVideoFormatType(cap.videoType)) {
};

VideoFormat::VideoFormat():
  width(0), height(0), fps(0), videoType(VideoFormat::VideoType::kUnknown) {
}


VideoFormat::~VideoFormat() {
}


webrtc::VideoCaptureCapability VideoFormat::toCapability() const {
  webrtc::VideoCaptureCapability cap;
  cap.width = width;
  cap.height = height;
  cap.maxFPS = fps;
  cap.videoType = VideoFormatTypeToCapabilityType(videoType);
  return cap;
}

// static
std::string VideoFormat::VideoTypeToName(const VideoFormat::VideoType& videoType) {
  for (size_t i = 0; i < arraysize(kSupportedVideoTypes); ++i) {
    if (kSupportedVideoTypes[i].videoType == videoType) {
      return kSupportedVideoTypes[i].name;
    }
  }
  // this should never be reached
  return "INVALID";
}

// static
std::string VideoFormat::WebrtcVideoTypeToName(const webrtc::VideoType& videoType) {
  return VideoFormat::VideoTypeToName(CapabilityTypeToVideoFormatType(videoType));
}
