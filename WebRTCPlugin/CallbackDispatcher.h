#ifndef _CALLBACK_DISPATCHER_H
#define _CALLBACK_DISPATCHER_H

#include "Callback.h"
#include "rtc_base/ref_count.h"
#include "rtc_base/thread.h"


template<class RefCount, class FunctorT>
class EventMessageHandler :
  public rtc::MessageHandler
{
public:
  explicit EventMessageHandler(RefCount* obj, FunctorT& functor) :
    functor_(functor)
  {
      obj->AddRef();
    this->obj = obj;
  }

  explicit EventMessageHandler(FunctorT& functor) :
	  functor_(functor)
  {
	  this->obj = nullptr;
  }

  virtual void OnMessage(rtc::Message* msg) 
  {
    functor_();
	if (obj)
      obj->Release();
    delete(this);
  }

private:
  RefCount* obj;
  FunctorT functor_;
};


template<class RefCount>
class CallbackDispatcher : public RefCount
{
public:
 
  void SetThread(std::shared_ptr<rtc::Thread> &thread)
  {
    this->thread = thread;
  }

  std::shared_ptr<rtc::Thread> &GetThread()
  {
    return thread;
  }

  HRESULT MarshalCallback(Callback& callback, VARIANT &handler)
  {
    
    HRESULT hr = callback.Marshal(handler);
    if (FAILED(hr))
      return hr;
    //Unmarshal it on siganling thread async
    Async([&callback]() {
      callback.UnMarshal();
    });
    return S_OK;
  }

  template<typename FunctorT>
  HRESULT Async(FunctorT functor)
  {
    thread->Post(RTC_FROM_HERE, new EventMessageHandler<RefCount,FunctorT>(this, functor));
	return S_OK;
  }

  template<typename FunctorT>
  HRESULT DispatchAsyncInternal(FunctorT functor)
  {
	  thread->Post(RTC_FROM_HERE, new EventMessageHandler<RefCount,FunctorT>(functor));
	  return S_OK;
  }

  HRESULT DispatchAsync(Callback& callback)
  {
	  //clone callback
	  Callback *cloned = new Callback(callback);
	  //Dispatch
	  return DispatchAsyncInternal([=]() {

		  cloned->Invoke();
		  delete(cloned);
	  });
  }

  HRESULT DispatchAsync(Callback& callback, const std::string &msg)
  {
	  //clone callback
	  Callback *cloned = new Callback(callback);
	  //Dispatch
	  return DispatchAsyncInternal([=]() {
		  cloned->Invoke(msg);
		  delete(cloned);
	  });
  }

  HRESULT DispatchAsync(Callback& callback, const char* msg)
  {
	  //clone callback
	  Callback *cloned = new Callback(callback);
	  //Dispatch
	  return DispatchAsyncInternal([=]() {
		  cloned->Invoke(msg);
		  delete(cloned);
	  });
  }

  HRESULT DispatchAsync(Callback& callback, const IUnknown *iUnk)
  {
    //clone callback
    Callback *cloned = new Callback(callback);

    //Create stream on global mem
    IStream *stream;
    CreateStreamOnHGlobal(NULL, true, &stream);
    //Add ref
    stream->AddRef();
    //Marshal variant into stream that can be unmarshalled multiple times
    HRESULT hr = CoMarshalInterface(stream, IID_IDispatch, (LPUNKNOWN)iUnk, MSHCTX_INPROC, NULL, MSHLFLAGS_TABLESTRONG);
    if (FAILED(hr))
      return hr;

    //Dispatch
    return DispatchAsyncInternal([=]() {
      IUnknown *iUnk;
      //Rewind stream
      stream->Seek({ 0 }, STREAM_SEEK_SET, NULL);
      //Unmarshall
      HRESULT hr = CoUnmarshalInterface(stream, IID_IUnknown, reinterpret_cast<LPVOID*>(&iUnk));
      //release ref
      stream->Release();
      cloned->Invoke(iUnk);
      delete(cloned);
    });
  }

  HRESULT DispatchAsync(Callback& callback, const _variant_t &variant)
  {
	  //clone callback
	  Callback *cloned = new Callback(callback);
	  //Dispatch
	  return DispatchAsyncInternal([=](){
			cloned->Invoke(variant);
			delete(cloned);
	  });
  }

  HRESULT DispatchAsync(Callback& callback, const _variant_t &variant1, const _variant_t &variant2)
  {
	  //clone callback
	  Callback *cloned = new Callback(callback);
	  //Dispatch
	  return DispatchAsyncInternal([=]() {
		  cloned->Invoke(variant1, variant2);
		  delete(cloned);
	  });
  }

  HRESULT DispatchAsync(Callback& callback, const std::string &msg1, const std::string &msg2)
  {
	  //clone callback
	  Callback *cloned = new Callback(callback);
	  //Dispatch
	  return DispatchAsyncInternal([=]() {
		  cloned->Invoke(msg1, msg2);
		  delete(cloned);
	  });
  }
  
  HRESULT DispatchAsync(Callback& callback, const std::vector<variant_t> &variants)
  {
	  //clone callback
	  Callback *cloned = new Callback(callback);
	  //Dispatch
	  return DispatchAsyncInternal([=]() {
		  cloned->Invoke(variants);
		  delete(cloned);
	  });
  }

  HRESULT DispatchAsync(Callback& callback, std::function<IUnknown*()> func)
  {
    //clone callback
    Callback *cloned = new Callback(callback);
    //Dispatch
    return DispatchAsyncInternal([=]() {
      cloned->Invoke(func());
      delete(cloned);
    });
  }

  virtual ~CallbackDispatcher() = default;

private:
  std::shared_ptr<rtc::Thread> thread;
};
#endif