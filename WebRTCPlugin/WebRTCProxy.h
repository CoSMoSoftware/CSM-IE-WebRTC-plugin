// WebRTCProxy.h : Declaration of the WebRTCProxy
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "WebRTCPlugin_i.h"
#include "CallbackDispatcher.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

#include "api/peerconnectioninterface.h"


// WebRTCProxy
class ATL_NO_VTABLE WebRTCProxy :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IWebRTCProxy, &IID_IWebRTCProxy, &LIBID_WebRTCPluginLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IOleControlImpl<WebRTCProxy>,
	public IOleObjectImpl<WebRTCProxy>,
	public IOleInPlaceActiveObjectImpl<WebRTCProxy>,
	public IViewObjectExImpl<WebRTCProxy>,
	public IOleInPlaceObjectWindowlessImpl<WebRTCProxy>,
	public IObjectSafetyImpl<WebRTCProxy, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
	public CComCoClass<WebRTCProxy, &CLSID_WebRTCProxy>,
	public CComControl<WebRTCProxy>
{
public:


	WebRTCProxy()
	{
	}

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_INVISIBLEATRUNTIME |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_WEBRTCPROXY)


DECLARE_NOT_AGGREGATABLE(WebRTCProxy)

BEGIN_COM_MAP(WebRTCProxy)
	COM_INTERFACE_ENTRY(IWebRTCProxy)
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

BEGIN_PROP_MAP(WebRTCProxy)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()


BEGIN_MSG_MAP(WebRTCProxy)
	CHAIN_MSG_MAP(CComControl<WebRTCProxy>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// IViewObjectEx
	DECLARE_VIEW_STATUS(0)

// IWebRTCProxy
public:
	HRESULT OnDrawAdvanced(ATL_DRAWINFO& di)
	{
		return S_OK;
	}


	DECLARE_PROTECT_FINAL_CONSTRUCT()

  HRESULT FinalConstruct();
  void FinalRelease();

  STDMETHOD(createPeerConnection)(VARIANT configuration, IUnknown** pc);
  STDMETHOD(createLocalAudioTrack)(VARIANT constraints, IUnknown** track);
  STDMETHOD(createLocalVideoTrack)(VARIANT constraints, IUnknown** track);
  STDMETHOD(parseIceCandidate)(VARIANT candidate, VARIANT* parsed);

  static std::shared_ptr<rtc::Thread>& GetEventThread() {	  return eventThread; }
  
private:
  static bool inited;
  static std::shared_ptr<rtc::Thread> signalingThread;
  static std::shared_ptr<rtc::Thread> eventThread;
  static std::shared_ptr<rtc::Thread> workingAndNetworkThread;

  // WebRTC objects variables
  rtc::scoped_refptr<webrtc::PeerConnectionFactoryInterface>  peer_connection_factory_;
  std::unique_ptr<cricket::VideoCapturer> video_capturer_;

};

OBJECT_ENTRY_AUTO(__uuidof(WebRTCProxy), WebRTCProxy)