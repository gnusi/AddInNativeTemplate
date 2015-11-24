#ifndef __ADDIN_NATIVE__H__
#define __ADDIN_NATIVE__H__

#include "ComponentBase.h"
#include "AddInDefBase.h"
#include "IMemoryManager.h"
#include "Map.h"
#include "register.h"

#include <string>
#include <functional>

////////////////////////////////////////////////////////////////////////////////
/// Generic helper definitions for shared library support
////////////////////////////////////////////////////////////////////////////////
#if defined _MSC_VER || defined __CYGWIN__
  #define COMPONENT_HELPER_DLL_IMPORT __declspec(dllimport)
  #define COMPONENT_HELPER_DLL_EXPORT __declspec(dllexport)
  #define COMPONENT_HELPER_DLL_LOCAL
#else
  #if __GNUC__ >= 4
    #define COMPONENT_HELPER_DLL_IMPORT __attribute__ ((visibility ("default")))
    #define COMPONENT_HELPER_DLL_EXPORT __attribute__ ((visibility ("default")))
    #define COMPONENT_HELPER_DLL_LOCAL  __attribute__ ((visibility ("hidden")))
  #else
    #define COMPONENT_HELPER_DLL_IMPORT
    #define COMPONENT_HELPER_DLL_EXPORT
    #define COMPONENT_HELPER_DLL_LOCAL
  #endif
#endif

// COMPONENT_API is used for the public API symbols. It either DLL imports or DLL exports (or does nothing for static build)
// COMPONENT_LOCAL is used for non-api symbols.
#ifdef COMPONENT_DLL
  #ifdef COMPONENT_DLL_EXPORTS
    #define COMPONENT_API COMPONENT_HELPER_DLL_EXPORT    
  #else
    #define COMPONENT_API COMPONENT_HELPER_DLL_IMPORT
  #endif // COMPONENT_DLL_EXPORTS
  #define COMPONENT_LOCAL COMPONENT_HELPER_DLL_LOCAL
#else // COMPONENT_DLL is not defined: this means IRESEARCH is a static lib.
  #define COMPONENT_API 
  #define COMPONENT_API_TEMPLATE
  #define COMPONENT_LOCAL
#endif // COMPONENT_DLL

////////////////////////////////////////////////////////////////////////////////
/// @class base implementation of the IComponent iterface
////////////////////////////////////////////////////////////////////////////////
class ComponentBaseImpl : public IComponentBase {
public:
  // Registration support 
  typedef std::function<IComponentBase*()> factory_f;

  struct Property {
    typedef std::function<bool(tVariant*)> prop_f;

    Property(const std::wstring& name);
    Property(Property&& rhs);
    Property& operator=(Property&& rhs);

    std::wstring name;
    std::wstring eng_name;
    prop_f get;
    prop_f set;
  }; // Property

  struct Method {
    typedef std::function<bool(tVariant*, long, tVariant*)> method_f;
    typedef std::function<bool(tVariant*)> get_arg_f;
    typedef std::vector<get_arg_f> get_arg_list_t;

    static bool empty_argument(tVariant* arg);

    Method(const std::wstring& name);
    Method(Method&& rhs);
    Method& operator=(Method&& rhs);

    method_f func;
    get_arg_list_t args;
    std::wstring name;
    std::wstring eng_name;
    bool is_func;
  }; // Method

  ComponentBaseImpl(const std::wstring& name, long version);

  // IInitDoneBase
  virtual bool ADDIN_API Init(void* conn);
  virtual bool ADDIN_API setMemManager(void* mem);
  virtual long ADDIN_API GetInfo();
  virtual void ADDIN_API Done();

  // ILanguageExtenderBase
  virtual bool ADDIN_API RegisterExtensionAs(WCHAR_T** extName);

  virtual long ADDIN_API GetNProps();
  virtual long ADDIN_API FindProp(const WCHAR_T* wsPropName);
  virtual const WCHAR_T* ADDIN_API GetPropName(
    long lPropNum,
    long lPropAlias);
  virtual bool ADDIN_API GetPropVal(
    const long lPropNum,
    tVariant* pvarPropVal);
  virtual bool ADDIN_API SetPropVal(
    const long lPropNum,
    tVariant* varPropVal);
  virtual bool ADDIN_API IsPropReadable(const long lPropNum);
  virtual bool ADDIN_API IsPropWritable(const long lPropNum);
  virtual long ADDIN_API GetNMethods();
  virtual long ADDIN_API FindMethod(const WCHAR_T* wsMethodName);
  virtual const WCHAR_T* ADDIN_API GetMethodName(
    const long lMethodNum,
    const long lMethodAlias);
  virtual long ADDIN_API GetNParams(const long lMethodNum);
  virtual bool ADDIN_API GetParamDefValue(
    const long lMethodNum,
    const long lParamNum,
    tVariant *pvarParamDefValue);
  virtual bool ADDIN_API HasRetVal(const long lMethodNum);
  virtual bool ADDIN_API CallAsProc(
    const long lMethodNum,
    tVariant* paParams,
    const long lSizeArray);
  virtual bool ADDIN_API CallAsFunc(
    const long lMethodNum,
    tVariant* pvarRetValue,
    tVariant* paParams,
    const long lSizeArray);
  
  // LocaleBase
  virtual void ADDIN_API SetLocale(const WCHAR_T* loc);

protected:
  void add_property(
    const std::wstring& name,
    const Property::prop_f& get = Property::prop_f(),
    const Property::prop_f& set = Property::prop_f(),
    const std::wstring& eng_name = std::wstring());

  void add_method(
    const std::wstring& name, bool is_func,
    const Method::method_f& func,
    const Method::get_arg_list_t& args = {},
    const std::wstring& eng_name = std::wstring());

  void* alloc(unsigned long size);

  template<typename T>
  T* alloc(unsigned long size) {
    return reinterpret_cast<T*>(alloc(size));
  }
  
  bool alloc_string(const std::wstring& str, WCHAR_T** out);
  WCHAR_T* alloc_string(const std::wstring& str);

  IAddInDefBase* connect_;
  IMemoryManager* memory_;

private:
  Map<Method> methods_;
  Map<Property> props_;
  std::wstring name_;
  long ver_;
}; // ComponentBase

class ComponentRegister 
  : public generic_register<
      std::wstring, 
      ComponentBaseImpl::factory_f,
      ComponentRegister> {
public:
  static std::wstring& class_names() {
    return class_names_;
  }

  template<typename Component>
  static void reg_name() {
    if (!class_names_.empty()) {
      class_names_ += '|';
    }
    class_names_ += Component::class_name();
  }

private:
  static std::wstring class_names_;
}; // ComponentRegister 

template<typename Component>
struct ComponentRegisterer : generic_registerer<ComponentRegister> {
  ComponentRegisterer()
    : generic_registerer<ComponentRegister>(Component::class_name(), &Component::make) {
    ComponentRegister::reg_name<Component>();
  }
}; // ComponentRegisterer

#define REGISTER_COMPONENT__(component_name, line) static ComponentRegisterer<component_name> component_registerer ## _ ## line
#define REGISTER_COMPONENT_EXPANDER__(component_name, line) REGISTER_COMPONENT__(component_name, line)
#define REGISTER_COMPONENT(component_name) REGISTER_COMPONENT_EXPANDER__(component_name, __LINE__)

#endif // __ADDIN_NATIVE__H__
