#pragma once
#include <d3d9.h>

namespace ui {

// Show Menu
inline bool open = true;

// Is Menu Initialized
inline bool setup = false;

// Win API
inline HWND window = nullptr;
inline WNDCLASSEX windowClass = {};
inline WNDPROC originalWindowProcess = nullptr;

// DX
inline LPDIRECT3DDEVICE9 device = nullptr;
inline LPDIRECT3D9 d3d9 = nullptr;

bool SetupWindowClass(const char *windowClassName) noexcept;
void DestroyWindowClass() noexcept;

bool SetupWindow(const char *windowName) noexcept;
void DestroyWindow() noexcept;

bool SetupDirectX() noexcept;
void DestroyDirectX() noexcept;

// Setup Device
void Setup();

void SetupMenu(LPDIRECT3DDEVICE9 device) noexcept;
void Destroy() noexcept;

void Render() noexcept;
} // namespace ui