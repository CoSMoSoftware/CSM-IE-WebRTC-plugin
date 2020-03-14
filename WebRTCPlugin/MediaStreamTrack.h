// MediaStreamTrack.h : Declaration of the MediaStreamTrack
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "WebRTCPlugin_i.h"
#include "Callback.h"

#include "api\mediastreaminterface.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;


interface DECLSPEC_UUID("ddc77b27-2463-4726-9c19-681f3d8d0112") ITrackAccess : public IUnknown
{
public:
  virtual rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> GetTrack() = 0;
};

// MediaStreamTrack
class ATL_NO_VTABLE MediaStreamTrack :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IMediaStreamTrack, &IID_IMediaStreamTrack, &LIBID_WebRTCPluginLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IOleControlImpl<MediaStreamTrack>,
	public IOleObjectImpl<MediaStreamTrack>,
	public IOleInPlaceActiveObjectImpl<MediaStreamTrack>,
	public IViewObjectExImpl<MediaStreamTrack>,
	public IOleInPlaceObjectWindowlessImpl<MediaStreamTrack>,
	public CComCoClass<MediaStreamTrack, &CLSID_MediaStreamTrack>,
	public CComControl<MediaStreamTrack>,
	public ITrackAccess
{
public:

	MediaStreamTrack()
	{
	}

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_INVISIBLEATRUNTIME |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_MEDIASTREAMTRACK)

DECLARE_NOT_AGGREGATABLE(MediaStreamTrack)

BEGIN_COM_MAP(MediaStreamTrack)
	COM_INTERFACE_ENTRY(IMediaStreamTrack)
	COM_INTERFACE_ENTRY(ITrackAccess)
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
END_COM_MAP()

BEGIN_PROP_MAP(MediaStreamTrack)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()


BEGIN_MSG_MAP(MediaStreamTrack)
	CHAIN_MSG_MAP(CComControl<MediaStreamTrack>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// IViewObjectEx
	DECLARE_VIEW_STATUS(0)

// IMediaStreamTrack
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
    track = nullptr;
	}

  void SetLabel(std::string label)
  {
    this->label = label;
  }

  void Attach(rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track)
  {
    this->track = track;
  }

  virtual rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> GetTrack() override
  {
    return track;
  }

  STDMETHOD(get_id)(VARIANT* val)
  {
    variant_t id = track->id().c_str();
    *val = id;
    return S_OK;
  }

  STDMETHOD(get_kind)(VARIANT* val)
  {
    variant_t kind = track->kind().c_str();
    *val = kind;
    return S_OK;
  }

  STDMETHOD(get_label)(VARIANT* val)
  {
    variant_t label = this->label.c_str();
    *val = label;
    return S_OK;
  }

  STDMETHOD(get_state)(VARIANT* val)
  {
    variant_t state = track->state() == webrtc::MediaStreamTrackInterface::kLive ? "live" : "ended";
    *val = state;
    return S_OK;
  }

  STDMETHOD(get_enabled)(VARIANT* val)
  {
    V_BOOL(val) = track->enabled() ? VARIANT_TRUE : VARIANT_FALSE;
    return S_OK;
  }

  STDMETHOD(put_enabled)(VARIANT val)
  {
    track->set_enabled(V_BOOL(&val)==VARIANT_TRUE);
    return S_OK;
  }
  
private:
  std::string label;
  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track;
};

OBJECT_ENTRY_AUTO(__uuidof(MediaStreamTrack), MediaStreamTrack)