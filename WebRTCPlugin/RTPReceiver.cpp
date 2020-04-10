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
  //TODO
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
