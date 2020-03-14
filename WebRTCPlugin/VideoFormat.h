#ifndef VideoFormat_h
#define VideoFormat_h

#include "modules/video_capture/video_capture.h"

class VideoFormat {
public:
  // This needs be kept in synch with webrtc::VideoType
  enum class VideoType {
	kUnknown,
	kI420,
	kIYUV,
	kRGB24,
	kABGR,
	kARGB,
	kARGB4444,
	kRGB565,
	kARGB1555,
	kYUY2,
	kYV12,
	kUYVY,
	kMJPEG,
	kNV21,
	kNV12,
	kBGRA,
  };

  int width;
  int height;
  int fps;
  VideoFormat::VideoType videoType;

  VideoFormat(int width, int height, int fps, const VideoFormat::VideoType& videoType);
  VideoFormat(const webrtc::VideoCaptureCapability& cap);
  VideoFormat();
  ~VideoFormat();

  webrtc::VideoCaptureCapability toCapability() const;

  static std::string VideoTypeToName(const VideoFormat::VideoType& videoType);
  static std::string WebrtcVideoTypeToName(const webrtc::VideoType& videoType);
};

#endif /* VideoFormat_hpp */
