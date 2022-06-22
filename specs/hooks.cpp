#include "hooks.h"

#include <intrin.h>
#include <stdexcept>

#include "3rdparty/minhook/include/MinHook.h"

#include "3rdparty/imgui/backends/imgui_impl_dx9.h"
#include "3rdparty/imgui/backends/imgui_impl_win32.h"
#include "3rdparty/imgui/imgui.h"

void hooks::Setup() {

  if (MH_Initialize()) {
    throw std::runtime_error("Unable to initialize minhook");
  }

  if (MH_CreateHook(VirtualFunction(ui::device, 42), &EndScene,
                    reinterpret_cast<void **>(&EndSceneOriginal))) {
    throw std::runtime_error("Unable to hook EndScene");
  }

  if (MH_CreateHook(VirtualFunction(ui::device, 16), &Reset,
                    reinterpret_cast<void **>(&ResetOriginal))) {
    throw std::runtime_error("Unable to hook Reset");
  }

  if (MH_EnableHook(MH_ALL_HOOKS)) {
    throw std::runtime_error("Unable to enable hooks");
  }

  ui::DestroyDirectX();
}

void hooks::Destroy() noexcept {
  MH_DisableHook(MH_ALL_HOOKS);
  MH_RemoveHook(MH_ALL_HOOKS);
  MH_Uninitialize();
}

long __stdcall hooks::EndScene(IDirect3DDevice9 *device) noexcept {

  static const auto returnAddress = _ReturnAddress();

  const auto result = EndSceneOriginal(device, device);

  // Stop endscene getting called twice
  if (_ReturnAddress() == returnAddress) {
    return result;
  }

  if (!ui::setup) {
    ui::SetupMenu(device);
  }

  if (ui::open) {
    ui::Render();
  }

  return result;
}

HRESULT __stdcall hooks::Reset(IDirect3DDevice9 *device,
                               D3DPRESENT_PARAMETERS *params) noexcept {

  ImGui_ImplDX9_InvalidateDeviceObjects();
  const auto result = ResetOriginal(device, device, params);
  ImGui_ImplDX9_CreateDeviceObjects();
  return result;
}