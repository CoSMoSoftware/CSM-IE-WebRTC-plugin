// RTPSender.cpp : Implementation of RTPSender
#include "stdafx.h"
#include <atlsafe.h>

// this project
#include "JSObject.h"
#include "RTPSender.h"
#include "MediaStreamTrack.h"

// RTPSender
//Getters
STDMETHODIMP RTPSender::get_id(VARIANT* val)
{
  if (!sender)
    return E_UNEXPECTED;
  variant_t id = sender->id().c_str();
  *val = id;
  return S_OK;
}

STDMETHODIMP RTPSender::get_track(IUnknown** val)
{
  if (!sender)
    return E_UNEXPECTED;

  //get native track
  auto trackInterface = sender->track();

  //Create activeX object for sender
  CComObject<MediaStreamTrack>* track;
  HRESULT hresult = CComObject<MediaStreamTrack>::CreateInstance(&track);

  if (FAILED(hresult))
    return hresult;

  //Attach to native object
  track->Attach(trackInterface);

  //Get Reference to pass it to JS
  *val = track->GetUnknown();

  //Add JS reference
  (*val)->AddRef();

  return S_OK;
};

STDMETHODIMP RTPSender::get_transport(IUnknown** trans)
{
  if (!sender)
    return E_UNEXPECTED;
  //TODO
  return S_OK;
};

STDMETHODIMP RTPSender::setParameters(VARIANT params)
{
  if (!sender)
    return E_UNEXPECTED;

  //TODO
  return S_OK;
};

STDMETHODIMP RTPSender::getParameters(VARIANT* params)
{
  if (!sender)
    return E_UNEXPECTED;

  CComSafeArray<VARIANT> args;

  //get parameters
  auto sendparams = sender->GetParameters();

  /*
  required DOMString transactionId;
  required sequence<RTCRtpEncodingParameters> encodings;
  required sequence<RTCRtpHeaderExtensionParameters> headerExtensions;
  required RTCRtcpParameters rtcp;
  required sequence<RTCRtpCodecParameters> codecs;
  */
  //Add params
  args.Add(variant_t(sendparams.transaction_id.c_str()));
  
  //Add encodings
  {
    CComSafeArray<VARIANT> encodings;
    //Create empty array
    encodings.Create();
    //For each one
    for (auto& encoding : sendparams.encodings)
    {
      /*
        DOMString rid;
        boolean active = true;
        unsigned long maxBitrate;
        double scaleResolutionDownBy;
      */
      CComSafeArray<VARIANT> arr;
      arr.Add(variant_t(encoding.rid.c_str()));
      arr.Add(variant_t(encoding.active));
      if (encoding.max_bitrate_bps.has_value())
        arr.Add(variant_t(encoding.max_bitrate_bps.value()));
      else
        arr.Add(variant_t());
      if (encoding.scale_framerate_down_by.has_value())
        arr.Add(variant_t(encoding.scale_framerate_down_by.value()));
      else
        arr.Add(variant_t());
      
      // Initialize the variant
      VARIANT var;
      VariantInit(&var);
      var.vt = VT_ARRAY | VT_VARIANT;
      var.parray = arr.Detach();
      //add to encodinsg
      encodings.Add(var);
    }
    // Initialize the variant
    VARIANT var;
    VariantInit(&var);
    var.vt = VT_ARRAY | VT_VARIANT;
    var.parray = encodings.Detach();

    //Add encodings
    args.Add(var);
   }

  //Add header extensions
  {
    CComSafeArray<VARIANT> extensions;
    //Create empty array
    extensions.Create();
    //For each one
    for (auto& extension : sendparams.header_extensions)
    {
      /*
        required DOMString uri;
        required unsigned short id;
        boolean encrypted = false;
      */
      CComSafeArray<VARIANT> arr;
      arr.Add(variant_t(extension.uri.c_str()));
      arr.Add(variant_t(extension.id));
      arr.Add(variant_t(extension.encrypt));

      // Initialize the variant
      VARIANT var;
      VariantInit(&var);
      var.vt = VT_ARRAY | VT_VARIANT;
      var.parray = arr.Detach();
      //add to encodinsg
      extensions.Add(var);
    }
    // Initialize the variant
    VARIANT var;
    VariantInit(&var);
    var.vt = VT_ARRAY | VT_VARIANT;
    var.parray = extensions.Detach();

    //Add encodings
    args.Add(var);
  }
  

  //Add rtcp stuff
  {
    /*
          DOMString cname;
          boolean reducedSize;
    */
    CComSafeArray<VARIANT> arr;
    arr.Add(variant_t(sendparams.rtcp.cname.c_str()));
    arr.Add(variant_t(sendparams.rtcp.reduced_size));

    // Initialize the variant
    VARIANT var;
    VariantInit(&var);
    var.vt = VT_ARRAY | VT_VARIANT;
    var.parray = arr.Detach();
    //add to encodinsg
    args.Add(var);
  }

  //Add codecs
  {
    CComSafeArray<VARIANT> codecs;
    //Create empty array
    codecs.Create();
    //For each one
    for (auto& codec : sendparams.codecs)
    {
      /**
        required octet payloadType;
        required DOMString mimeType;
        required unsigned long clockRate;
        unsigned short channels;
        DOMString sdpFmtpLine;
      */
      CComSafeArray<VARIANT> arr;
      arr.Add(variant_t(codec.payload_type));
      arr.Add(variant_t(codec.mime_type().c_str()));
      if (codec.clock_rate.has_value())
        arr.Add(variant_t(codec.clock_rate.value()));
      else
        arr.Add(variant_t());
      if (codec.num_channels.has_value())
        arr.Add(variant_t(codec.num_channels.value()));
      else
        arr.Add(variant_t());
      if (!codec.parameters.empty())
      {
        std::string sdpFmtpLine;
        for (auto& pair : codec.parameters)
        {
          if (!sdpFmtpLine.empty())
            sdpFmtpLine += "; ";
          sdpFmtpLine += pair.first + "=" + pair.second;
        }
        arr.Add(variant_t(sdpFmtpLine.c_str()));
      } else {
        arr.Add(variant_t());
      }

      // Initialize the variant
      VARIANT var;
      VariantInit(&var);
      var.vt = VT_ARRAY | VT_VARIANT;
      var.parray = arr.Detach();
      //add to encodinsg
      codecs.Add(var);
    }
    // Initialize the variant
    VARIANT var;
    VariantInit(&var);
    var.vt = VT_ARRAY | VT_VARIANT;
    var.parray = codecs.Detach();

    //Add codecs to params
    args.Add(var);
  }

  // Initialize the variant
  VariantInit(params);
  params->vt = VT_ARRAY | VT_VARIANT;
  params->parray = args.Detach();

  //Done
  return S_OK;
};

STDMETHODIMP RTPSender::replaceTrack(VARIANT track, boolean *ret)
{
  if (!sender)
    return E_UNEXPECTED;

  //Get dispatch interface
  if (track.vt != VT_DISPATCH)
    return E_INVALIDARG;

  IDispatch* disp = V_DISPATCH(&track);

  if (!disp)
    return E_INVALIDARG;

  //Get atl com object from track.
  CComPtr<ITrackAccess> proxy;
  HRESULT hr = disp->QueryInterface(IID_PPV_ARGS(&proxy));
  if (FAILED(hr))
    return hr;

  //Add track
  *ret = sender->SetTrack(proxy->GetTrack());

  return S_OK;
};