// RTPTransceiver.cpp : Implementation of RTPTransceiver
#include "stdafx.h"
#include <atlsafe.h>

#include "RTPTransceiver.h"
#include "RTPSender.h"
#include "RTPReceiver.h"

// RTPTransceiver
STDMETHODIMP RTPTransceiver::get_mid(VARIANT* val)
{
  if (!transceiver)
    return E_UNEXPECTED;
  return S_OK;
}

STDMETHODIMP RTPTransceiver::get_sender(IUnknown**   val)
{
  if (!transceiver)
    return E_UNEXPECTED;
  //get native sender
  auto senderInterface = transceiver->sender();

  //Create activeX object for sender
  CComObject<RTPSender>* sender;
  HRESULT hresult = CComObject<RTPSender>::CreateInstance(&sender);

  if (FAILED(hresult))
    return hresult;

  //Attach to native object
  sender->Attach(senderInterface);

  //Get Reference to pass it to JS
  *val = sender->GetUnknown();

  //Add JS reference
  (*val)->AddRef();

  return S_OK;
}

STDMETHODIMP RTPTransceiver::get_receiver(IUnknown**   val)
{
  if (!transceiver)
    return E_UNEXPECTED;

  //get native receiver
  auto receiverInterface = transceiver->receiver();

  //Create activeX object for sender
  CComObject<RTPReceiver>* receiver;
  HRESULT hresult = CComObject<RTPReceiver>::CreateInstance(&receiver);

  if (FAILED(hresult))
    return hresult;

  //Attach to native object
  receiver->Attach(receiverInterface);

  //Get Reference to pass it to JS
  *val = receiver->GetUnknown();

  //Add JS reference
  (*val)->AddRef();

  return S_OK;
}

STDMETHODIMP RTPTransceiver::get_direction(VARIANT* val)
{
  if (!transceiver)
    return E_UNEXPECTED;
 
  //get native direction
  variant_t direction;
  switch(transceiver->direction())
  {
    case webrtc::RtpTransceiverDirection::kSendRecv:
      direction = "sendrecv";
      break;
    case webrtc::RtpTransceiverDirection::kSendOnly:
      direction = "sendonly";
      break;
    case webrtc::RtpTransceiverDirection::kRecvOnly:
      direction = "recvonly";
      break;
    case webrtc::RtpTransceiverDirection::kInactive:
      direction = "inactive";
      break;
  }
  
  *val = direction;

  return S_OK;
}

STDMETHODIMP RTPTransceiver::get_currentDirection(VARIANT* val)
{
  if (!transceiver)
    return E_UNEXPECTED;

  //get native direction
  auto current = transceiver->current_direction();
  //Check if it has a direction
  if (current.has_value()) 
  { 
    //Get value
    variant_t direction;
    switch (transceiver->direction())
    {
      case webrtc::RtpTransceiverDirection::kSendRecv:
        direction = "sendrecv";
        break;
      case webrtc::RtpTransceiverDirection::kSendOnly:
        direction = "sendonly";
        break;
      case webrtc::RtpTransceiverDirection::kRecvOnly:
        direction = "recvonly";
        break;
      case webrtc::RtpTransceiverDirection::kInactive:
        direction = "inactive";
        break;
    }
    *val = direction;
  } else {
    // Initialize the variant
    VariantInit(val);
    //Nothing
    val->vt = VT_NULL;
  }
  //Done
  return S_OK;
}

STDMETHODIMP RTPTransceiver::put_direction(VARIANT val)
{
  if (!transceiver)
    return E_UNEXPECTED;
  //Get direction as string
  std::string direction = (char*)_bstr_t(val);

  //Set it in native
  if  (direction.compare("sendrecv")==0)
    transceiver->SetDirection(webrtc::RtpTransceiverDirection::kSendRecv);
  else if (direction.compare("sendonly") == 0)
    transceiver->SetDirection(webrtc::RtpTransceiverDirection::kSendOnly);
  else if (direction.compare("recvonly") == 0)
    transceiver->SetDirection(webrtc::RtpTransceiverDirection::kRecvOnly);
  else if (direction.compare("inactive") == 0)
    transceiver->SetDirection(webrtc::RtpTransceiverDirection::kInactive);
  else 
    return E_INVALIDARG;
  
  return S_OK;
}

STDMETHODIMP RTPTransceiver::stop()
{
  if (!transceiver)
    return E_UNEXPECTED;
  //Stop
  transceiver->Stop();
  return S_OK;
}