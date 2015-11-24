#include "ComponentBaseImpl.h"

// -----------------------------------------------------------------------------
// --SECTION--                                                     ComponentBaseImpl 
// -----------------------------------------------------------------------------

ComponentBaseImpl::Property::Property(const std::wstring& name)
  : name(name) {
}

ComponentBaseImpl::Property::Property(ComponentBaseImpl::Property&& rhs)
  : name(std::move(rhs.name)),
    eng_name(std::move(rhs.eng_name)),
    get(std::move(rhs.get)),
    set(std::move(rhs.set)) {
}

ComponentBaseImpl::Property&
ComponentBaseImpl::Property::operator=(ComponentBaseImpl::Property&& rhs) {
  if (this != &rhs) {
    name = std::move(rhs.name);
    eng_name = std::move(rhs.eng_name);
    get = std::move(rhs.get);
    set = std::move(rhs.set);
  }
  return *this;
}

ComponentBaseImpl::Method::Method(const std::wstring& name)
  : name(name) {
}

ComponentBaseImpl::Method::Method(ComponentBaseImpl::Method&& rhs)
  : func(std::move(rhs.func)), 
    args(std::move(rhs.args)),
    name(std::move(rhs.name)), 
    eng_name(std::move(rhs.eng_name)),
    is_func(rhs.is_func) {
}

ComponentBaseImpl::Method& 
ComponentBaseImpl::Method::operator=(ComponentBaseImpl::Method&& rhs) {
  if (this != &rhs) {
    func = std::move(rhs.func);
    args = std::move(rhs.args);
    name = std::move(rhs.name);
    eng_name = std::move(rhs.eng_name);
    is_func = rhs.is_func;
  }
  return *this;
}

bool ComponentBaseImpl::Method::empty_argument(tVariant* arg) {
  TV_VT(arg) = VTYPE_EMPTY;
  return true;
}

ComponentBaseImpl::ComponentBaseImpl(const std::wstring& name, long version)
  : name_(name), connect_{}, memory_{}, ver_(version) {
}

// IInitDoneBase
bool ComponentBaseImpl::Init(void* conn) {
  return nullptr != (connect_ = reinterpret_cast<IAddInDefBase*>(conn));
}

bool ComponentBaseImpl::setMemManager(void* mem) {
  return nullptr != (memory_ = reinterpret_cast<IMemoryManager*>(mem));
}

long ComponentBaseImpl::GetInfo() {
  return ver_;
}

void ComponentBaseImpl::Done() {}

// ILanguageExtenderBase
bool ComponentBaseImpl::RegisterExtensionAs(WCHAR_T** extName) {
  return alloc_string(name_, extName);
}

long ComponentBaseImpl::GetNProps() {
  return props_.size();
}

long ComponentBaseImpl::FindProp(const WCHAR_T* wsPropName) {
  return props_.find_id(wsPropName);
}

const WCHAR_T* ComponentBaseImpl::GetPropName(
  long lPropNum,
  long lPropAlias) {
  auto& prop = props_.at(lPropNum);
  return alloc_string((lPropAlias ? prop.name : prop.eng_name));
}

bool ComponentBaseImpl::GetPropVal(
  const long lPropNum,
  tVariant* pvarPropVal) {
  return props_.at(lPropNum).get(pvarPropVal);
}

bool ComponentBaseImpl::SetPropVal(
  const long lPropNum,
  tVariant* varPropVal) {
  return props_.at(lPropNum).set(varPropVal);
}

bool ComponentBaseImpl::IsPropReadable(const long lPropNum) {
  return bool(props_.at(lPropNum).get);
}

bool ComponentBaseImpl::IsPropWritable(const long lPropNum) {
  return bool(props_.at(lPropNum).set);
}

long ComponentBaseImpl::GetNMethods() {
  return methods_.size();
}

long ComponentBaseImpl::FindMethod(const WCHAR_T* wsMethodName) {
  return methods_.find_id(wsMethodName);
}

const WCHAR_T* ComponentBaseImpl::GetMethodName(
  const long lMethodNum,
  const long lMethodAlias) {
  auto& meth = methods_.at(lMethodNum);
  return alloc_string(lMethodAlias ? meth.name : meth.eng_name);
}

long ComponentBaseImpl::GetNParams(const long lMethodNum) {
  return methods_.at(lMethodNum).args.size();
}

bool ComponentBaseImpl::GetParamDefValue(
  const long lMethodNum,
  const long lParamNum,
  tVariant *pvarParamDefValue) {
  return methods_.at(lMethodNum).args[lParamNum](pvarParamDefValue);
}

bool ComponentBaseImpl::HasRetVal(const long lMethodNum) {
  return methods_.at(lMethodNum).is_func;
}

bool ComponentBaseImpl::CallAsProc(
  const long lMethodNum,
  tVariant* paParams,
  const long lSizeArray) {
  return methods_.at(lMethodNum).func(paParams, lSizeArray, nullptr);
}

bool ComponentBaseImpl::CallAsFunc(
  const long lMethodNum,
  tVariant* pvarRetValue,
  tVariant* paParams,
  const long lSizeArray) {
  return methods_.at(lMethodNum).func(paParams, lSizeArray, pvarRetValue);
}

// LocaleBase
void ComponentBaseImpl::SetLocale(const WCHAR_T* loc) {
#ifndef __linux__
  _wsetlocale(LC_ALL, loc);
#else
  //We convert in char* char_locale
  //also we establish locale
  //setlocale(LC_ALL, char_locale);
#endif
}

void* ComponentBaseImpl::alloc(unsigned long size) {
  void* out{};
  if (memory_) {
    memory_->AllocMemory(&out, size);
  }
  return out;
}

bool ComponentBaseImpl::alloc_string(const std::wstring& str, WCHAR_T** out) {
  const unsigned long size = sizeof(WCHAR_T)*(str.size() + 1);
  if (*out = alloc<WCHAR_T>(size)) {
    std::copy(str.begin(), str.end(), *out);
    return true;
  }

  return false;
}

WCHAR_T* ComponentBaseImpl::alloc_string(const std::wstring& str) {
  WCHAR_T* out;
  auto res = alloc_string(str, &out);
  assert(res);
  return out;
}

void ComponentBaseImpl::add_property(
    const std::wstring& name,
    const Property::prop_f& get /* = Property::prop_f() */,
    const Property::prop_f& set /* = Property::prop_f() */,
    const std::wstring& eng_name /* = std::wstring() */ ) {
  auto& prop = props_.insert(name);
  prop.eng_name = eng_name.empty() ? name : eng_name;
  prop.get = get;
  prop.set = set;
}

void ComponentBaseImpl::add_method(
    const std::wstring& name, bool is_func,
    const Method::method_f& func,
    const Method::get_arg_list_t& args /* = {} */,
    const std::wstring& eng_name /* = std::wstring() */ ) {
  auto& meth = methods_.insert(name);
  meth.eng_name = eng_name.empty() ? name : eng_name;
  meth.is_func = is_func;
  meth.args = args;
  meth.func = func;
}

std::wstring ComponentRegister::class_names_;