// RTPReceiver.cpp : Implementation of RTPReceiver
#include "stdafx.h"
#include <atlsafe.h>

#include "JSObject.h"
#include "RTPReceiver.h"
#include "MediaStreamTrack.h"

// RTPReceiver
STDMETHODIMP RTPReceiver::get_id(VARIANT* val)
{
  if (!receiver)
    return E_UNEXPECTED;
  variant_t id = receiver->id().c_str();
  *val = id;
  return S_OK;
}

STDMETHODIMP RTPReceiver::get_track(IUnknown** val)
{
  if (!receiver)
    return E_UNEXPECTED;

  //get native track
  auto trackInterface = receiver->track();

  //Create activeX object for receiver
  CComObject<MediaStreamTrack>* track;
  HRESULT hresult = CComObject<MediaStreamTrack>::CreateInstance(&track);

  if (FAILED(hresult))
    return hresult;

  //Attach to native object
  track->Attach(trackInterface);

  //Get Reference to pass it to JS
  *val = track->GetUnknown();

  //Add js rev
  (*val)->AddRef();

  return S_OK;
};

STDMETHODIMP RTPReceiver::get_transport(IUnknown** trans)
{
  if (!receiver)
    return E_UNEXPECTED;
  //TODO
  return S_OK;
};

STDMETHODIMP RTPReceiver::setParameters(VARIANT params)
{
  if (!receiver)
    return E_UNEXPECTED;
  //TODO
  return S_OK;
};

STDMETHODIMP RTPReceiver::getParameters(VARIANT* params)
{
  if (!receiver)
    return E_UNEXPECTED;

  CComSafeArray<VARIANT> args;

  //get parameters
  auto recvparams = receiver->GetParameters();

  /*
  required sequence<RTCRtpHeaderExtensionParameters> headerExtensions;
  required RTCRtcpParameters rtcp;
  required sequence<RTCRtpCodecParameters> codecs;
  */

  //Add header extensions
  {
    CComSafeArray<VARIANT> extensions;
    //Create empty array
    extensions.Create();
    //For each one
    for (auto& extension : recvparams.header_extensions)
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
    arr.Add(variant_t(recvparams.rtcp.cname.c_str()));
    arr.Add(variant_t(recvparams.rtcp.reduced_size));

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
    for (auto& codec : recvparams.codecs)
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


STDMETHODIMP RTPReceiver::getStreamIds(VARIANT* val)
{
  if (!receiver)
    return E_UNEXPECTED;

  //Get streams
  auto streamIds = receiver->stream_ids();
  
  CComSafeArray<VARIANT> args(streamIds.size());

  //For each id
  int i=0;
  for (auto streamId : streamIds)
    //add ot array
    args.SetAt(i++, _variant_t(streamId.c_str()));
  

  // Initialize the variant
  VariantInit(val);
  //Set array
  val->vt = VT_ARRAY | VT_VARIANT;
  val->parray = args.Detach();
  //Done
  return S_OK;
};
