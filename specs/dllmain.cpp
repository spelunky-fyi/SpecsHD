#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <cstdint>
#include <thread>

#include "hooks.h"

void WINAPI MainThread(const HMODULE instance) {

  try {
    ui::Setup();
    hooks::Setup();
  } catch (const std::exception &error) {
    MessageBeep(MB_ICONERROR);
    MessageBox(0, error.what(), "Specs HD Error", MB_OK | MB_ICONEXCLAMATION);
    goto DIE;
  }

#ifdef DEV
  while (!GetAsyncKeyState(VK_END) & 1) {
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
  }
#else
  return;
#endif

DIE:
  hooks::Destroy();
  ui::Destroy();
  FreeLibraryAndExitThread(instance, 0);
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved) {

  if (dwReason == DLL_PROCESS_ATTACH) {
    // AllocConsole();
    // freopen("CONOUT$", "w", stdout);
    // std::cout << "Starting Specs HD!" << std::endl;

    DisableThreadLibraryCalls(hMod);
    const auto thread = CreateThread(
        nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(MainThread), hMod,
        0, nullptr);

    if (thread) {
      CloseHandle(thread);
    }
  }

  return TRUE;
}