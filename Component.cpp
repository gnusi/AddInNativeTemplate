#include "ComponentBaseImpl.h"

////////////////////////////////////////////////////////////////////////////////
/// @brief SampleComponent
////////////////////////////////////////////////////////////////////////////////
class SampleComponent : public ComponentBaseImpl {
public:
  static IComponentBase* make() {
    return new SampleComponent();
  }

  static const std::wstring& class_name() {
    static std::wstring name(L"AddInSample");
    return name;
  }

  enum Version : long {
    V10
  };

  SampleComponent() : ComponentBaseImpl(L"SampleComponent", V10) {
    add_property(
      L"SetMe",
      /* getter */
      [this] (tVariant* out) {
      TV_VT(out) = VTYPE_BOOL;
      out->bVal = flag_;
      return true;
    },
      /* setter */
      [this] (tVariant* in) {
      if (TV_VT(in) != VTYPE_BOOL) {
        return false;
      }

      flag_ = in->bVal;
      return true;
    });

    add_method(
      L"CallMe", true, 
      [this] (tVariant* params, long count, tVariant* out) {
      return true;
    });

    add_method(
      L"SendEvent", true, 
      [this] (tVariant* params, long count, tVariant* out) {
      return true;
    }, {
      &Method::empty_argument,
      &Method::empty_argument,
      [] (tVariant* arg) {
        TV_VT(arg) = VTYPE_EMPTY;
        return true;
      }
    });
  }

private:
  bool flag_;
}; // SampleComponent

REGISTER_COMPONENT(SampleComponent);