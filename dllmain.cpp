#include "ComponentBaseImpl.h"
#include <string>

// -----------------------------------------------------------------------------
// --SECTION--                                                    Export methods 
// -----------------------------------------------------------------------------

long GetClassObject(const WCHAR_T* wsName, IComponentBase** pInterface) {
  auto factory = ComponentRegister::instance().get(wsName);
  if (!factory) {
    return 0;
  }

  *pInterface = factory();
  return 1;
}

long DestroyObject(IComponentBase** pIntf) {
  delete *pIntf;
  return 0;
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