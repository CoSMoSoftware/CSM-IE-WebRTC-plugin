// RTPSender.cpp : Implementation of RTPSender
#include "stdafx.h"
#include <atlsafe.h>

// this project
#include "JSObject.h"
#include "RTPSender.h"
#include "MediaStreamTrack.h"

#include <cctype>
// trim from start (in place)
static inline void ltrim(std::string &s)
{
  s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
    return !std::isspace(ch);
  }));
}

// trim from end (in place)
static inline void rtrim(std::string &s)
{
  s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
    return !std::isspace(ch);
  }).base(), s.end());
}

// trim from both ends (in place)
static inline void trim(std::string &s)
{
  ltrim(s);
  rtrim(s);
}

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

  webrtc::RtpParameters parameters;

  //Get js object
  JSObject js(params);

  //If valid
  if (!js.isNull())
  {
    //Set transaction id
    parameters.transaction_id = js.GetStringProperty(L"transactionId");

    //Get encodings;
    auto encodings = js.GetArrayObjectProperty(L"encodings");
    //Set them
    for (auto& encoding : encodings)
    {
      //Create native
      webrtc::RtpEncodingParameters parameter;
      
      //If valid
      if (!encoding.isNull())
      {
        parameter.rid     = encoding.GetStringProperty(L"rid");
        parameter.active  = encoding.GetBooleanProperty(L"active",true);
        if (encoding.HasProperty(L"maxBitrate"))
          parameter.max_bitrate_bps = encoding.GetIntegerProperty(L"maxBitrate");
        if (encoding.HasProperty(L"scaleResolutionDownBy"))
          parameter.scale_framerate_down_by = encoding.GetDoubleProperty(L"scaleResolutionDownBy");
      }

      //Push it
      parameters.encodings.push_back(parameter);
    }

    //Get header extension info
    auto headerExtensions = js.GetArrayObjectProperty(L"headerExtensions");
    //Set them
    for (auto& headerExtension : headerExtensions)
    {
      //Create native
      webrtc::RtpHeaderExtensionParameters parameter;

      //If valid
      if (!headerExtension.isNull())
      {
        parameter.uri       = headerExtension.GetStringProperty(L"uri");
        parameter.id        = headerExtension.GetIntegerProperty(L"active");
        parameter.encrypt   = headerExtension.GetBooleanProperty(L"encrypted",false);
      }

      //Push it
      parameters.header_extensions.push_back(parameter);
    }

    //Get encodings;
    auto codecs = js.GetArrayObjectProperty(L"codecs");
    //Set them
    for (auto& codec : codecs)
    {
      //Create native
      webrtc::RtpCodecParameters parameter;

      //If valid
      if (!codec.isNull())
      {
        parameter.payload_type = codec.GetIntegerProperty(L"payloadType");
        std::string mimeType   = codec.GetStringProperty(L"mimeType");
        //Find type/name separator
        auto middle = mimeType.find("/");
        //Get key and val
        auto type = mimeType.substr(0, middle);
        //Check tipe
        if (type.compare("video")==0)
          parameter.kind = cricket::MEDIA_TYPE_AUDIO;
        else if (type.compare("audio") == 0)
          parameter.kind = cricket::MEDIA_TYPE_VIDEO;
        //Set name
        parameter.name = middle != std::string::npos ? mimeType.substr(middle + 1) : "";
        if (codec.HasProperty(L"maxBiclockRatetrate"))
          parameter.clock_rate = codec.GetIntegerProperty(L"clockRate");

        if (codec.HasProperty(L"sdpFmtpLine"))
        {
          //Get line
          std::string sdpFmtpLine = codec.GetStringProperty(L"sdpFmtpLine");

          //Tokenize
          auto start = 0U;
          while (start <= sdpFmtpLine.length())
          {
            //Find delimiter
            auto end = sdpFmtpLine.find(";");
            //Get param until delimiter or end of string
            auto fmtp = sdpFmtpLine.substr(start, end != std::string::npos ? end - start : std::string::npos);

            //Find key/val separator
            auto middle = fmtp.find("=");
            //Get key and val
            auto key = fmtp.substr(0,middle);
            auto val = middle != std::string::npos ? fmtp.substr(middle+1) : "";

            //Trim them
            trim(key);
            trim(val);
            //Append
            parameter.parameters.emplace(key, val);

            //Move next
            start = end + 1;
          }


        }
      }

      //Push it
      parameters.codecs.push_back(parameter);
    }

    //Get rtcp
    auto rtcp = js.GetObjectProperty(L"rtcp");

    //If valid
    if (!rtcp.isNull())
    {
      //Set values
      parameters.rtcp.cname         = rtcp.GetStringProperty(L"cname");
      parameters.rtcp.reduced_size  = rtcp.GetBooleanProperty(L"reducedSize");
    }

  }

  //Set them
  auto res = sender->SetParameters(parameters);

  //Done
  return res.ok() ? S_OK : E_INVALIDARG;
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

  //If it is null
  if (track.vt == VT_NULL)
  {
    //Add track
    *ret = sender->SetTrack(nullptr);

    return S_OK;
  }

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