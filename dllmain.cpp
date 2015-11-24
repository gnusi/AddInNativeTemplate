#include "ComponentBaseImpl.h"
#include <string>

// -----------------------------------------------------------------------------
// --SECTION--                                                    Export methods 
// -----------------------------------------------------------------------------

long GetClassObject(const WCHAR_T* wsName, IComponentBase** pInterface) {
  enum GET_CLASS_OBJECT_RESULT : long {
    GCO_OK = 1,
    GCO_FALSE = 0
  };

  auto factory = ComponentRegister::instance().get(wsName);
  if (!factory) {
    return GCO_FALSE;
  }

  *pInterface = factory();
  return GCO_OK;
}

long DestroyObject(IComponentBase** pIntf) {
  enum DestoyObjectResult : long {
    DO_OK = 0,
    DO_FALSE = 1
  };

  delete *pIntf;
  return DO_OK;
}

const WCHAR_T* GetClassNames() {
  return ComponentRegister::class_names().c_str();
}

// -----------------------------------------------------------------------------
// --SECTION--                                                           DllMain 
// -----------------------------------------------------------------------------

#ifdef _MSC_VER 

#include <Windows.h>

BOOL APIENTRY DllMain(
    HMODULE,
    DWORD ul_reason_for_call,
    LPVOID) {
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

#endif