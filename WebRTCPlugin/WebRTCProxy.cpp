
// WebRTCProxy.cpp : Implementation of WebRTCProxy
#include "stdafx.h"
#include <atlsafe.h>

// this project
#include "JSObject.h"
#include "WebRTCProxy.h"
#include "RTCPeerConnection.h"
#include "MediaStreamTrack.h"
#include "VideoSourceAdapter.h"

// libwebrtc
#include "rtc_base/ssl_adapter.h"
#include "api/peer_connection_interface.h"
#include "api/create_peerconnection_factory.h"
#include "api/audio_codecs/builtin_audio_decoder_factory.h"
#include "api/audio_codecs/builtin_audio_encoder_factory.h"
#include "api/video_codecs/builtin_video_decoder_factory.h"
#include "api/video_codecs/builtin_video_encoder_factory.h"

// Normal Device Capture
#include "modules/video_capture/video_capture_factory.h"

bool WebRTCProxy::inited = false;
std::shared_ptr<rtc::Thread> WebRTCProxy::signalingThread;
std::shared_ptr<rtc::Thread> WebRTCProxy::eventThread;
std::shared_ptr<rtc::Thread> WebRTCProxy::workingAndNetworkThread;

// WebRTCProxy
HRESULT WebRTCProxy::FinalConstruct()
{
  if (!inited)
  {
    //rtc::LogMessage::ConfigureLogging("sensitive debug");
	  rtc::LogMessage::ConfigureLogging("error");
    rtc::InitializeSSL();
    rtc::InitRandom(rtc::Time());

  taskQueueFactory        = webrtc::CreateDefaultTaskQueueFactory();

	signalingThread         = std::shared_ptr<rtc::Thread>(rtc::Thread::Create().release());
	eventThread             = std::shared_ptr<rtc::Thread>(rtc::Thread::Create().release());
	workingAndNetworkThread = std::shared_ptr<rtc::Thread>(rtc::Thread::CreateWithSocketServer().release());

	signalingThread->SetName("signaling_thread", NULL);
	eventThread->SetName("event_thread", NULL);
	workingAndNetworkThread->SetName("working_and_network_thread", NULL);

	if (!signalingThread->Start() || !eventThread->Start() || !workingAndNetworkThread->Start())
		return false;

  //Initialize things on working thread thread
  auto ret = workingAndNetworkThread->Invoke<bool>(RTC_FROM_HERE, [this]() {
    //Create audio module
    adm = webrtc::AudioDeviceModule::Create(webrtc::AudioDeviceModule::kPlatformDefaultAudio, taskQueueFactory.get());

    //Check
    if (!adm)
    {
      RTC_LOG(INFO) << "Audio Device Module creation failed.";
      return false;
    }

    RTC_LOG(INFO) << "Audio Device Module created";
    adm->Init();
    return true;
  });

	//Initialize things on event thread
	eventThread->Invoke<void>(RTC_FROM_HERE, []() {
		CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
	});

    inited = true;
  }

  //Create peer connection factory
  peer_connection_factory_ =
    webrtc::CreatePeerConnectionFactory(
      workingAndNetworkThread.get(),  // network_thread
      workingAndNetworkThread.get(),  // worker_thread
      signalingThread.get(),          // signaling_thread
      adm,                            // AudioDeviceModule
      webrtc::CreateBuiltinAudioEncoderFactory(),
      webrtc::CreateBuiltinAudioDecoderFactory(),
      webrtc::CreateBuiltinVideoEncoderFactory(),
      webrtc::CreateBuiltinVideoDecoderFactory(),
      NULL,  // audio_mixer
      NULL   // audio_processing
    ).release();

  //Check
  if (!peer_connection_factory_)
    return S_FALSE;

  return S_OK;
}

void WebRTCProxy::FinalRelease()
{
  //Remove factory
  peer_connection_factory_ = nullptr;
  if (adm && adm->Initialized())
    adm->Terminate();
}

STDMETHODIMP WebRTCProxy::createPeerConnection(VARIANT variant, IUnknown** peerConnection)
{
  webrtc::PeerConnectionInterface::RTCConfiguration configuration;
  JSObject obj(variant);

  if (!obj.isNull())
  {
    /*
    dictionary RTCIceServer {
      required (DOMString or sequence<DOMString>) urls;
      DOMString                          username;
      (DOMString or RTCOAuthCredential)  credential;
      RTCIceCredentialType               credentialType = "password";
    };

    dictionary RTCConfiguration {
      sequence<RTCIceServer>   iceServers;
      RTCIceTransportPolicy    iceTransportPolicy = "all";
      RTCBundlePolicy          bundlePolicy = "balanced";
      RTCRtcpMuxPolicy         rtcpMuxPolicy = "require";
      DOMString                peerIdentity;
      sequence<RTCCertificate> certificates;
      [EnforceRange]
      octet                    iceCandidatePoolSize = 0;
      */
      //Get ice servers array
      JSObject iceServers       = obj.GetProperty(L"iceServers");
      //TODO: support the followin ones
      _bstr_t bundlePolicy      = obj.GetStringProperty(L"bundlePolicy");
      _bstr_t rtcpMuxPolicy     = obj.GetStringProperty(L"rtcpMuxPolicy");
      //_bstr_t peerIdentity      = obj.GetStringProperty(L"peerIdentity");
      int iceCandidatePoolSize  = obj.GetIntegerProperty(L"iceServers");

      //If we have them
      if (!iceServers.isNull())
      {
        //For each property
        for (auto name : iceServers.GetPropertyNames())
        {
          //Get ice server
          JSObject server = iceServers.GetProperty(name);
          //If we have it
          if (!server.isNull())
          {
            webrtc::PeerConnectionInterface::IceServer iceServer;

            //Get the values
            auto urls               = server.GetProperty(L"urls");
            _bstr_t username        = server.GetStringProperty(L"username");
            _bstr_t credential      = server.GetStringProperty(L"credential");
            //TODO: Support credential type
            _bstr_t credentialType  = server.GetStringProperty(L"credentialType"); //Not supported yet
            //if url is an string
            if (urls.vt == VT_BSTR)
            {
              //Get url
              _bstr_t url(urls.bstrVal);
              //Append
              iceServer.urls.push_back((char*)url);
            } else {
              //Conver to object
              JSObject aux(urls);
              //Ensure we hage it
              if (!aux.isNull())
              {
                //Get all urls
                for (auto idx : aux.GetPropertyNames())
                {
                  //Get url
                  _bstr_t url = aux.GetStringProperty(idx);
                  //Append
                  iceServer.urls.push_back((char*)url);
                }
              }
            }
            //Set username and credential, OATH not supported yet
            if ((char*)username)
              iceServer.username = (char*)username;
            if ((char*)credential)
              iceServer.password = (char*)credential;
            //Push
            configuration.servers.push_back(iceServer);
          }
        }
      }
    };
    
  //Create activeX object which is a
  CComObject<RTCPeerConnection>* pc;
  HRESULT hresult = CComObject<RTCPeerConnection>::CreateInstance(&pc);

  if (FAILED(hresult))
    return hresult;

  //Force unified plan
  configuration.sdp_semantics = webrtc::SdpSemantics::kUnifiedPlan;

  //Create peerconnection object, it will call the AddRef inside as it gets a ref to the observer
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> pci = peer_connection_factory_->CreatePeerConnection(
    configuration,
    NULL, // allocator
    NULL, // cert_generator
    pc    // observer
  );

  //Check it was created correctly
  if (!pci)
    //Error
    return E_INVALIDARG;

  //Sety event thread
  pc->SetThread(eventThread);

  //Attach to PC
  pc->Attach(pci);

  //Get Reference to pass it to JS
  *peerConnection = pc->GetUnknown();

  //Add JS reference
  (*peerConnection)->AddRef();

  //OK
  return hresult;
}


STDMETHODIMP WebRTCProxy::createLocalAudioTrack(VARIANT constraints, IUnknown** track)
{
  const cricket::AudioOptions options;
  //Create audio source
  auto audioSource = peer_connection_factory_->CreateAudioSource(options);

  //Ensure it is created
  if (!audioSource)
    return E_UNEXPECTED;

  //Create track
  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> audioTrack = peer_connection_factory_->CreateAudioTrack("audio", audioSource);

  //Ensure it is created
  if (!audioTrack)
    return E_UNEXPECTED;

  //Create activeX object which is a
  CComObject<MediaStreamTrack>* mediaStreamTrack;
  HRESULT hresult = CComObject<MediaStreamTrack>::CreateInstance(&mediaStreamTrack);

  if (FAILED(hresult))
    return hresult;
  
  //Attach to native track
  mediaStreamTrack->Attach(audioTrack);

  //Set dummy audio  label
  mediaStreamTrack->SetLabel("Default Audio Device");

  //Get Reference to pass it to JS
  *track = mediaStreamTrack->GetUnknown();

  //Add JS reference
  (*track)->AddRef();

  //OK
  return hresult;
}


STDMETHODIMP WebRTCProxy::createLocalVideoTrack(VARIANT constraints, IUnknown** track)
{

  rtc::scoped_refptr<VideoSourceAdapter> videoSourceA
      = new rtc::RefCountedObject<VideoSourceAdapter>();

  //Ensure it is created
  if (!videoSourceA)
      return E_UNEXPECTED;

  //Get info for all devices
  std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo>
      info(webrtc::VideoCaptureFactory::CreateDeviceInfo());

  //Get constrains
  JSObject obj(constraints);

  //If it has the deviceId
  auto deviceId = obj.GetStringProperty(L"deviceId");

  //Check length of the id to ensure it is valid
  bool hasDeviceId = deviceId.length()>0;

  //pick the first valid device
  Source* source = nullptr;
  const uint32_t kSize = 1024;
  char name[kSize] = { 0 };
  char id[kSize] = { 0 };
  int num_devices = info->NumberOfDevices();
  for (int i = 0; i < num_devices; ++i) 
  {
      if (info->GetDeviceName(i, name, kSize, id, kSize) != -1) 
      {
        //Match device id or first found
        if (!hasDeviceId || strcmp(id,deviceId)==0)
        {
          source = new Source(Source::DEVICE, i, name, id);
          break;
        }
      }
  }

  if (!source)
      return E_UNEXPECTED;

  if (!videoSourceA->Init(*source))
      return E_UNEXPECTED;

  webrtc::VideoCaptureCapability requested_cap;
  if (videoSourceA->Start(requested_cap) == webrtc::MediaSourceInterface::kEnded)
      return E_UNEXPECTED;

  // Now create the track in libwebrtc
  auto videoTrack = peer_connection_factory_->CreateVideoTrack(
      "video-" + std::to_string(source->type), videoSourceA);

  //Ensure it is created
  if (!videoTrack)
     return E_UNEXPECTED;

  //Create activeX object for media stream track
  CComObject<MediaStreamTrack>* mediaStreamTrack;
  HRESULT hresult = CComObject<MediaStreamTrack>::CreateInstance(&mediaStreamTrack);

  if (FAILED(hresult))
      return hresult;

  //Attach to native track
  mediaStreamTrack->Attach(videoTrack);

  //Set device name as label
  mediaStreamTrack->SetLabel(source->name);

  //Get Reference to pass it to JS
  *track = mediaStreamTrack->GetUnknown();

  //Add JS reference
  (*track)->AddRef();

  //OK
  return hresult;
}


STDMETHODIMP WebRTCProxy::parseIceCandidate(VARIANT candidate, VARIANT* parsed)
{
	//Check input is a string
	if (candidate.vt != VT_BSTR)
		return E_INVALIDARG;

	//Get candidate as string
	std::string str = (char*)_bstr_t(candidate);

	//Try to parse input
	webrtc::SdpParseError parseError;
	// Creates a IceCandidateInterface based on SDP string.
	std::unique_ptr<webrtc::IceCandidateInterface> iceCandidate(webrtc::CreateIceCandidate("audio", 0, str, &parseError));

	if (!iceCandidate)
		//Parsing error
		return E_INVALIDARG;

	//Fill data
	_variant_t foundation		= iceCandidate->candidate().foundation().c_str();
	_variant_t component		= iceCandidate->candidate().component();
	_variant_t priority			= iceCandidate->candidate().priority();
	_variant_t ip				= iceCandidate->candidate().address().hostname().c_str();
	_variant_t protocol			= iceCandidate->candidate().protocol().c_str();
	_variant_t port				= iceCandidate->candidate().address().port();
	_variant_t type				= iceCandidate->candidate().type().c_str();
	_variant_t tcpType			= iceCandidate->candidate().tcptype().c_str();
	_variant_t relatedAddress	= iceCandidate->candidate().related_address().hostname().c_str();
	_variant_t relatedPort		= iceCandidate->candidate().related_address().port();
	_variant_t usernameFragment = iceCandidate->candidate().username().c_str();

	CComSafeArray<VARIANT> args(11);
	args.SetAt(0, foundation);
	args.SetAt(1, component);
	args.SetAt(2, priority);
	args.SetAt(3, ip);
	args.SetAt(4, protocol);
	args.SetAt(5, port);
	args.SetAt(6, type);
	args.SetAt(7, tcpType);
	args.SetAt(8, relatedAddress);
	args.SetAt(9, relatedPort);
	args.SetAt(10, usernameFragment);

	// Initialize the variant
	VariantInit(parsed);
	parsed->vt = VT_ARRAY | VT_VARIANT;
	parsed->parray = args.Detach();

	//Parsed ok
	return S_OK;
}

STDMETHODIMP WebRTCProxy::enumerateDevices(VARIANT* devices)
{
  CComSafeArray<VARIANT> args;
  
  //Get info for all video devices
  std::unique_ptr<webrtc::VideoCaptureModule::DeviceInfo> info(webrtc::VideoCaptureFactory::CreateDeviceInfo());

  //If any
  if (info)
  {
    const uint32_t kSize = 1024;
    char name[kSize] = { 0 };
    char id[kSize] = { 0 };

    // List the names of all devices
    int num_devices = info->NumberOfDevices();
    RTC_LOG(INFO) << num_devices << " video recording devices detected.";

    for (int i = 0; i < num_devices; ++i)
    {
      //Get device id and name
      if (info->GetDeviceName(i, name, kSize, id, kSize) != -1)
      {
        CComSafeArray<VARIANT> device(3);
        device.SetAt(0, variant_t(id));
        device.SetAt(1, variant_t(name));
        device.SetAt(2, variant_t("videoinput"));

        // Initialize the variant
        VARIANT var;
        VariantInit(&var);
        var.vt = VT_ARRAY | VT_VARIANT;
        var.parray = device.Detach();

        //Add to devices
        args.Add(var);

      }
    }
  }

  if (adm)
  {
    char name[webrtc::kAdmMaxDeviceNameSize] = { 0 };
    char id[webrtc::kAdmMaxGuidSize] = { 0 };

    //Get number of devices
    uint16_t num = adm->RecordingDevices();

    // DEVICES
    for (int i = 0; i < num; ++i)
    {
      //Get device id and name
      if (adm->RecordingDeviceName(i, name, id) != -1)
      {
        CComSafeArray<VARIANT> device(3);
        device.SetAt(0, variant_t(id));
        device.SetAt(1, variant_t(name));
        device.SetAt(2, variant_t("audioinput"));

        // Initialize the variant
        VARIANT var;
        VariantInit(&var);
        var.vt = VT_ARRAY | VT_VARIANT;
        var.parray = device.Detach();

        //Add to devices
        args.Add(var);
      }
    }

    //Get number of devices
    num = adm->PlayoutDevices();

    // DEVICES
    for (int i = 0; i < num; ++i)
    {
      //Get device id and name
      if (adm->PlayoutDeviceName(i, name, id) != -1)
      {
      
        CComSafeArray<VARIANT> device(3);
        device.SetAt(0, variant_t(id));
        device.SetAt(1, variant_t(name));
        device.SetAt(2, variant_t("audiooutput"));

        // Initialize the variant
        VARIANT var;
        VariantInit(&var);
        var.vt = VT_ARRAY | VT_VARIANT;
        var.parray = device.Detach();

        //Add to devices
        args.Add(var);

      }
    }
  }
  
  // Initialize the variant
  VariantInit(devices);
  devices->vt = VT_ARRAY | VT_VARIANT;
  devices->parray = args.Detach();

  //Parsed ok
  return S_OK;
}
