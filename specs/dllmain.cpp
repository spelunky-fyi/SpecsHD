#include "3rdparty/imgui/backends/imgui_impl_dx9.h"
#include "3rdparty/imgui/backends/imgui_impl_win32.h"
#include "3rdparty/imgui/imgui.h"
#include "3rdparty/kiero/kiero.h"

#include <d3d9.h>

#include "specs.h"

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg,
                                              WPARAM wParam, LPARAM lParam);
typedef long(__stdcall *EndScene)(LPDIRECT3DDEVICE9);
typedef LRESULT(CALLBACK *WNDPROC)(HWND, UINT, WPARAM, LPARAM);

EndScene oEndScene = NULL;
WNDPROC oWndProc;
static HWND window = NULL;

void InitImGui(LPDIRECT3DDEVICE9 pDevice) {
  ImGui::CreateContext();
  ImGui_ImplWin32_Init(window);
  ImGui_ImplDX9_Init(pDevice);
}

bool init = false;
long __stdcall hkEndScene(LPDIRECT3DDEVICE9 pDevice) {
  if (!init) {
    InitImGui(pDevice);

    specsOnInit();
    init = true;
  }

  ImGui_ImplDX9_NewFrame();
  ImGui_ImplWin32_NewFrame();

  ImGui::NewFrame();

  specsOnFrame();

  ImGui::EndFrame();
  ImGui::Render();

  ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

  return oEndScene(pDevice);
}

LRESULT __stdcall WndProc(const HWND hWnd, UINT uMsg, WPARAM wParam,
                          LPARAM lParam) {

  if (true && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
    return true;

  return CallWindowProc(oWndProc, hWnd, uMsg, wParam, lParam);
}

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam) {
  DWORD wndProcId;
  GetWindowThreadProcessId(handle, &wndProcId);

  if (GetCurrentProcessId() != wndProcId)
    return TRUE; // skip to next window

  window = handle;
  return FALSE; // window found abort search
}

HWND GetProcessWindow() {
  window = NULL;
  EnumWindows(EnumWindowsCallback, NULL);
  return window;
}

DWORD WINAPI MainThread(LPVOID lpReserved) {
  bool attached = false;
  do {
    if (kiero::init(kiero::RenderType::D3D9) == kiero::Status::Success) {
      kiero::bind(42, (void **)&oEndScene, hkEndScene);
      do
        window = GetProcessWindow();
      while (window == NULL);
      oWndProc =
          (WNDPROC)SetWindowLongPtr(window, GWLP_WNDPROC, (LONG_PTR)WndProc);
      attached = true;
    }
  } while (!attached);
  return TRUE;
}

BOOL WINAPI DllMain(HMODULE hMod, DWORD dwReason, LPVOID lpReserved) {

  switch (dwReason) {
  case DLL_PROCESS_ATTACH:
    // AllocConsole();
    // freopen("CONOUT$", "w", stdout);
    // std::cout << "Starting Specs HD!" << std::endl;

    DisableThreadLibraryCalls(hMod);
    CreateThread(nullptr, 0, MainThread, hMod, 0, nullptr);
    break;
  case DLL_PROCESS_DETACH:
    kiero::shutdown();
    break;
  }
  return TRUE;
}