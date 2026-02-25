#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <hddll/hddll.h>

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved) {
  if (dwReason == DLL_PROCESS_ATTACH) {
    hddll::Start(hMod);
  }
  return TRUE;
}
