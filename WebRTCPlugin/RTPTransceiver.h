// RTPTransceiver.h : Declaration of the RTPTransceiver
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "WebRTCPlugin_i.h"

#include "api/rtp_transceiver_interface.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;



// RTPTransceiver
class ATL_NO_VTABLE RTPTransceiver :
	public CComObjectRootEx<CComSingleThreadModel>,
	public IDispatchImpl<IRTPTransceiver, &IID_IRTPTransceiver, &LIBID_WebRTCPluginLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IOleControlImpl<RTPTransceiver>,
	public IOleObjectImpl<RTPTransceiver>,
	public IOleInPlaceActiveObjectImpl<RTPTransceiver>,
	public IViewObjectExImpl<RTPTransceiver>,
	public IOleInPlaceObjectWindowlessImpl<RTPTransceiver>,
	public IObjectSafetyImpl<RTPTransceiver, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
	public CComCoClass<RTPTransceiver, &CLSID_RTPTransceiver>,
	public CComControl<RTPTransceiver>
{
public:


	RTPTransceiver()
	{
	}

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_INVISIBLEATRUNTIME |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)

DECLARE_REGISTRY_RESOURCEID(IDR_RTPTRANSCEIVER)


BEGIN_COM_MAP(RTPTransceiver)
	COM_INTERFACE_ENTRY(IRTPTransceiver)
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

BEGIN_PROP_MAP(RTPTransceiver)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()


BEGIN_MSG_MAP(RTPTransceiver)
	CHAIN_MSG_MAP(CComControl<RTPTransceiver>)
	DEFAULT_REFLECTION_HANDLER()
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

// IViewObjectEx
	DECLARE_VIEW_STATUS(0)

// IRTPTransceiver
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
		this->transceiver = nullptr;
	}
	
	void Attach(rtc::scoped_refptr<webrtc::RtpTransceiverInterface> &pc)
	{
		this->transceiver = transceiver;
	}

private:
	 rtc::scoped_refptr<webrtc::RtpTransceiverInterface> transceiver;
};

OBJECT_ENTRY_AUTO(__uuidof(RTPTransceiver), RTPTransceiver)
