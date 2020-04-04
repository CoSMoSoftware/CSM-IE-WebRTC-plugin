// RTPSender.h : Declaration of the RTPSender
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>
#include "WebRTCPlugin_i.h"
#include "Callback.h"

#include "api/rtpsenderinterface.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

interface DECLSPEC_UUID("ddc77b27-2465-4726-9c19-681f3d8d0112") ISenderAccess : public IUnknown
{
	public:
	virtual rtc::scoped_refptr<webrtc::RtpSenderInterface > GetSender() = 0;
};

// RTPSender
class ATL_NO_VTABLE RTPSender :
public CComObjectRootEx<CComSingleThreadModel>,
public IDispatchImpl<IRTPSender, &IID_IRTPSender, &LIBID_WebRTCPluginLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
public IOleControlImpl<RTPSender>,
public IOleObjectImpl<RTPSender>,
public IOleInPlaceActiveObjectImpl<RTPSender>,
public IViewObjectExImpl<RTPSender>,
public IOleInPlaceObjectWindowlessImpl<RTPSender>,
public CComCoClass<RTPSender, &CLSID_RTPSender>,
public CComControl<RTPSender>,
public ISenderAccess
{
public:


	RTPSender()
	{
	}

	DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
		OLEMISC_INVISIBLEATRUNTIME |
		OLEMISC_CANTLINKINSIDE |
		OLEMISC_INSIDEOUT |
		OLEMISC_ACTIVATEWHENVISIBLE |
		OLEMISC_SETCLIENTSITEFIRST
		)

	DECLARE_REGISTRY_RESOURCEID(IDR_RTPSENDER)


	DECLARE_NOT_AGGREGATABLE(RTPSender)

	BEGIN_COM_MAP(RTPSender)
	COM_INTERFACE_ENTRY(IRTPSender)
	COM_INTERFACE_ENTRY(ISenderAccess)
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

	BEGIN_PROP_MAP(RTPSender)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
	// PROP_PAGE(CLSID_StockColorPage)
	END_PROP_MAP()


	BEGIN_MSG_MAP(RTPSender)
	CHAIN_MSG_MAP(CComControl<RTPSender>)
	DEFAULT_REFLECTION_HANDLER()
	END_MSG_MAP()
	// Handler prototypes:
	//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
	//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	// IViewObjectEx
	DECLARE_VIEW_STATUS(0)

	// IRTPSender
public:
	HRESULT OnDraw(ATL_DRAWINFO& di)
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
		sender = nullptr;
	}

	void Attach(rtc::scoped_refptr<webrtc::RtpSenderInterface > &sender)
	{
		this->sender = sender;
	}

	rtc::scoped_refptr<webrtc::RtpSenderInterface > GetSender() override
	{
		return this->sender;
	}

	//Getters
	STDMETHOD(get_id)(VARIANT* val)
	{
		variant_t id = sender->id().c_str();
		*val = id;
		return S_OK;
	}

	STDMETHOD(get_track)(IUnknown** track)
	{
		return 0;
	};

	STDMETHOD(get_transport)(IUnknown** trans)
	{
		return 0;
	};

	//IRTCPeerConnection.idl

	STDMETHOD(getCapabilities)(VARIANT kind, VARIANT* caps)
	{
		return 0;
	};

	STDMETHOD(setParameters)(VARIANT params)
	{
		return 0;
	};

	STDMETHOD(getParameters)(VARIANT* params)
	{
		return 0;
	};

	STDMETHOD(replaceTrack)(IUnknown** track)
	{
		return 0;
	};

	STDMETHOD(getStats)(VARIANT* statsReport)
	{
		return 0;
	};

private:
	rtc::scoped_refptr<webrtc::RtpSenderInterface > sender;
};

OBJECT_ENTRY_AUTO(__uuidof(RTPSender), RTPSender)
