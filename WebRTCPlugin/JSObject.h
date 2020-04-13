#ifndef _JSOBJECT_H_
#define _JSOBJECT_H_

#include <atlctl.h>
#include <atlcomcli.h>
#include <OleAuto.h>
#include <comutil.h>
using namespace ATL;

#include <vector>
#include <string>

inline int64_t GetInt(VARIANT* variant, int64_t default)
{
  switch (variant->vt)
  {
    case VT_I1:
      return V_I1(variant);
    case VT_I2:
      return V_I2(variant);
    case VT_I4:
      return V_I4(variant);
    case VT_UI1:
      return V_UI1(variant);
    case VT_UI2:
      return V_UI2(variant);
    case VT_INT:
      return V_INT(variant);
    case VT_UI4:
      return V_UI4(variant);
    case VT_UINT:
      return V_UINT(variant);
    case VT_I8:
      return V_I8(variant);
    case VT_UI8:
      return V_UI8(variant);
  }
  return default;
}

class JSObject
{
public:
  JSObject(VARIANT& obj)
    : dispatchEx(V_DISPATCH(&obj))
  {

  }
  bool HasProperty(const std::wstring& name) const
  {
    HRESULT hr = E_NOTIMPL;
    DISPID dispId = DISPID_UNKNOWN;
    CComVariant result;
    CComExcepInfo exceptionInfo;
    DISPPARAMS params = { 0 };

    hr = dispatchEx->GetDispID(CComBSTR(name.c_str()), fdexNameEnsure | fdexNameCaseSensitive | 0x10000000, &dispId);

    return dispId!= DISPID_UNKNOWN;
  }


  CComVariant GetProperty(const std::wstring& name) {

    HRESULT hr = E_NOTIMPL;
    DISPID dispId = DISPID_UNKNOWN;
    CComVariant result;
    CComExcepInfo exceptionInfo;
    DISPPARAMS params = { 0 };

    hr = dispatchEx->GetDispID(CComBSTR(name.c_str()), fdexNameEnsure | fdexNameCaseSensitive | 0x10000000, &dispId);
    if (SUCCEEDED(hr) && dispId != DISPID_UNKNOWN)
      hr = dispatchEx->InvokeEx(dispId, LOCALE_USER_DEFAULT, DISPATCH_PROPERTYGET, &params, &result, &exceptionInfo, NULL);

    return result;
  }

  _bstr_t GetStringProperty(const std::wstring& name, const std::string& default = "") {
    //Get property
    auto prop = GetProperty(name);
    //check type
    if (prop.bstrVal)
      return prop;
    //Return empy
    return _bstr_t(default.c_str());
  }

  int64_t GetIntegerProperty(const std::wstring& name, int64_t default = 0) {
    auto prop = GetProperty(name);
    //Get property
    return GetInt(&prop, default);
  }

  bool GetBooleanProperty(const std::wstring& name, bool default = false) {
    auto prop = GetProperty(name);
    //Get property
    if (prop.vt == VT_BOOL) 
      return prop.boolVal == VARIANT_TRUE;
    return GetInt(&prop, default);
  }

  std::vector<std::wstring> GetPropertyNames() {

    std::vector<std::wstring> names;

    DISPID dispid = DISPID_STARTENUM;
    while (dispatchEx->GetNextDispID(fdexEnumAll, dispid, &dispid) != S_FALSE) 
    {
      if (dispid < 0) 
        continue;
      CComBSTR memberName;
      if (SUCCEEDED(dispatchEx->GetMemberName(dispid, &memberName)))
        names.push_back(std::wstring(memberName));
    }
    return names;
  }

  bool isNull() {
    return !dispatchEx;
  }

private:
  CComQIPtr<IDispatchEx> dispatchEx;
};

#endif
