// RTCPeerConnection.h : Declaration of the RTCPeerConnection
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "WebRTCPlugin_i.h"
#include "CallbackDispatcher.h"
#include "api/peerconnectioninterface.h"


#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;



// RTCPeerConnection
class ATL_NO_VTABLE RTCPeerConnection :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IRTCPeerConnection, &IID_IRTCPeerConnection, &LIBID_WebRTCPluginLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IOleControlImpl<RTCPeerConnection>,
	public IOleObjectImpl<RTCPeerConnection>,
	public IOleInPlaceActiveObjectImpl<RTCPeerConnection>,
	public IViewObjectExImpl<RTCPeerConnection>,
	public IOleInPlaceObjectWindowlessImpl<RTCPeerConnection>,
	public IObjectSafetyImpl<RTCPeerConnection, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
	public CComCoClass<RTCPeerConnection, &CLSID_RTCPeerConnection>,
	public CComControl<RTCPeerConnection>,
  public webrtc::PeerConnectionObserver,
  public CallbackDispatcher<IUnknown>
{
public:


	RTCPeerConnection()
	{
	}

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_INVISIBLEATRUNTIME |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_RTCPEERCONNECTION)


DECLARE_NOT_AGGREGATABLE(RTCPeerConnection)

BEGIN_COM_MAP(RTCPeerConnection)
	COM_INTERFACE_ENTRY(IRTCPeerConnection)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IViewObjectEx)
	COM_INTERFACE_ENTRY(IViewObject2)
	COM_INTERFACE_ENTRY(IViewObject)
	COM_INTERFACE_ENTRY(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceObject)
	COM_INTERFACE_ENTRY2(IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY(IOleControl)
	COM_INTERFACE_ENTRY(IOleObject)
	COM_INTERFACE_ENTRY_IID(IID_IObjectSafety, IObjectSafety)
END_COM_MAP()

BEGIN_PROP_MAP(RTCPeerConnection)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()


BEGIN_MSG_MAP(RTCPeerConnection)
	CHAIN_MSG_MAP(CComControl<RTCPeerConnection>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// IViewObjectEx
	DECLARE_VIEW_STATUS(0)

// IRTCPeerConnection
public:
	HRESULT OnDrawAdvanced(ATL_DRAWINFO& di)
	{
		return S_OK;
	}


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
    this->pc = nullptr;
  }

  void Attach(rtc::scoped_refptr<webrtc::PeerConnectionInterface> &pc)
  {
    this->pc = pc;
  }

 //IRTCPeerConnectin.idl
  STDMETHOD(setConfiguration)     (VARIANT variant);
  STDMETHOD(createOffer)          (VARIANT successCallback, VARIANT failureCallback, VARIANT options);
  STDMETHOD(setLocalDescription)  (VARIANT successCallback, VARIANT failureCallback, VARIANT description);
  STDMETHOD(createAnswer)         (VARIANT successCallback, VARIANT failureCallback, VARIANT options);
  STDMETHOD(setRemoteDescription) (VARIANT successCallback, VARIANT failureCallback, VARIANT description);
  STDMETHOD(addIceCandidate)      (VARIANT successCallback, VARIANT failureCallback, VARIANT candidate);
  STDMETHOD(addTrack)             (VARIANT track, VARIANT stream, IUnknown** rtpSender);
  STDMETHOD(removeTrack)          (VARIANT sender);
  STDMETHOD(getRemoteStreamTracks)(VARIANT stream, VARIANT successCallback);
  STDMETHOD(createDataChannel)(VARIANT label, VARIANT dataChannelDict, IUnknown** dataChannel);
  STDMETHOD(close)                ();

 //Getters
  STDMETHOD(get_localDescription)(VARIANT* val);
  STDMETHOD(get_currentLocalDescription)(VARIANT* val);
  STDMETHOD(get_pendingLocalDescription)(VARIANT* val);
  STDMETHOD(get_remoteDescription)(VARIANT* val);
  STDMETHOD(get_currentRemoteDescription)(VARIANT* val);
  STDMETHOD(get_pendingRemoteDescription)(VARIANT* val);
    
 //Event handlers
  STDMETHOD(put_onnegotiationneeded)(VARIANT handler);
  STDMETHOD(put_onicecandidate)(VARIANT handler);
  STDMETHOD(put_onicecandidateerror)(VARIANT handler);
  STDMETHOD(put_onsignalingstatechange)(VARIANT handler);
  STDMETHOD(put_oniceconnectionstatechange)(VARIANT handler);
  STDMETHOD(put_onicegatheringstatechange)(VARIANT handler);
  STDMETHOD(put_onconnectionstatechange)(VARIANT handler);
  STDMETHOD(put_onaddstream)(VARIANT handler);
  STDMETHOD(put_onremovestream)(VARIANT handler);
  STDMETHOD(put_ondatachannel)(VARIANT handler);

 //webrtc::PeerConnectionObserver
  void OnSignalingChange(webrtc::PeerConnectionInterface::SignalingState new_state) override;
  void OnAddStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
  void OnRemoveStream(rtc::scoped_refptr<webrtc::MediaStreamInterface> stream) override;
  void OnDataChannel(rtc::scoped_refptr<webrtc::DataChannelInterface> channel) override;
  void OnRenegotiationNeeded() override;
  void OnIceConnectionChange(webrtc::PeerConnectionInterface::IceConnectionState new_state) override;
  void OnIceGatheringChange(webrtc::PeerConnectionInterface::IceGatheringState new_state) override;
  void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;
  void OnIceConnectionReceivingChange(bool receiving) override;

private:
  rtc::scoped_refptr<webrtc::PeerConnectionInterface> pc;
	std::map<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface>> localStreams;
	std::map<std::string, rtc::scoped_refptr<webrtc::MediaStreamInterface>> remoteStreams;

  Callback onnegotiationneeded;
  Callback onicecandidate;
  Callback onicecandidateerror;
  Callback onsignalingstatechange;
  Callback oniceconnectionstatechange;
  Callback onicegatheringstatechange;
  Callback onconnectionstatechange;
	Callback onaddstream;
	Callback onremovestream;
  Callback ondatachannel;

};

OBJECT_ENTRY_AUTO(__uuidof(RTCPeerConnection), RTCPeerConnection)
