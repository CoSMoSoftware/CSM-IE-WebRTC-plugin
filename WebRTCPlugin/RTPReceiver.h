// RTPReceiver.h : Declaration of the RTPReceiver

#pragma once

// ActiveX generic
#include "resource.h"       // main symbols
#include <atlctl.h>

// this plugin
#include "WebRTCPlugin_i.h"
#include "Callback.h"

// libwebrtc C++ layer
#include "api/rtp_receiver_interface.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

using namespace ATL;

interface DECLSPEC_UUID("6394e804-fdf9-499b-8ae3-130982e4b8df") IReceiverAccess : public IUnknown
{
public:
  virtual  rtc::scoped_refptr<webrtc::RtpReceiverInterface > GetReceiver() = 0;
};

// RTPReceiver
class ATL_NO_VTABLE RTPReceiver :
  public CComObjectRootEx<CComSingleThreadModel>,
  public IDispatchImpl<IRTPReceiver, &IID_IRTPReceiver, &LIBID_WebRTCPluginLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
  public IOleControlImpl<RTPReceiver>,
  public IOleObjectImpl<RTPReceiver>,
  public IOleInPlaceActiveObjectImpl<RTPReceiver>,
  public IViewObjectExImpl<RTPReceiver>,
  public IOleInPlaceObjectWindowlessImpl<RTPReceiver>,
  public CComCoClass<RTPReceiver, &CLSID_RTPReceiver>,
  public CComControl<RTPReceiver>,
  public IReceiverAccess
{
public:
  RTPReceiver() {}

  DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
  OLEMISC_INVISIBLEATRUNTIME |
    OLEMISC_CANTLINKINSIDE |
    OLEMISC_INSIDEOUT |
    OLEMISC_ACTIVATEWHENVISIBLE |
    OLEMISC_SETCLIENTSITEFIRST
    )

    DECLARE_REGISTRY_RESOURCEID(IDR_RTPRECEIVER)


  DECLARE_NOT_AGGREGATABLE(RTPReceiver)

  BEGIN_COM_MAP(RTPReceiver)
    COM_INTERFACE_ENTRY(IRTPReceiver)
    COM_INTERFACE_ENTRY(IReceiverAccess)
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

  BEGIN_PROP_MAP(RTPReceiver)
    PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
    PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
    // Example entries
    // PROP_ENTRY_TYPE("Property Name", dispid, clsid, vtType)
    // PROP_PAGE(CLSID_StockColorPage)
  END_PROP_MAP()


  BEGIN_MSG_MAP(RTPReceiver)
    CHAIN_MSG_MAP(CComControl<RTPReceiver>)
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
    receiver = nullptr;
  }

  void Attach(rtc::scoped_refptr<webrtc::RtpReceiverInterface >& receiver)
  {
    this->receiver = receiver;
  }
  
  rtc::scoped_refptr<webrtc::RtpReceiverInterface > GetReceiver() override
  {
    return this->receiver;
  }

  //Getters
  STDMETHOD(get_id)(VARIANT* val);
  STDMETHOD(get_track)(IUnknown** track);
  STDMETHOD(get_transport)(IUnknown** trans);

  STDMETHOD(setParameters)(VARIANT params);
  STDMETHOD(getParameters)(VARIANT* params);

private:
  rtc::scoped_refptr<webrtc::RtpReceiverInterface > receiver;
};

OBJECT_ENTRY_AUTO(__uuidof(RTPReceiver), RTPReceiver)
