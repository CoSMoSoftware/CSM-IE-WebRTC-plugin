// RTPSender.cpp : Implementation of RTPSender
#include "stdafx.h"

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
  //TODO
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