//
//  VideoSource.cpp
//  
//
//  Created by cosmo on 16/7/19.
//

#include "stdafx.h"

#include "rtc_base/logging.h"

#if defined(  _WIN32 )
#include "rtc_base/win32.h"  // Need this to #include the impl files.
#endif
#include "system_wrappers/include/field_trial.h"

#include "VideoSource.h"

VideoSource::VideoSource(){
	enable_video_adapter_ = true;
}

VideoSource::~VideoSource() {}

webrtc::MediaSourceInterface::SourceState VideoSource::state() const
{
	return state_;
}
