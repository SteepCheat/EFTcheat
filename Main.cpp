#pragma once
#define WIN32_LEAN_AND_MEAN

#pragma warning (disable : 4838)
#pragma warning (disable : 4309)
#pragma warning (disable : 4312)
#pragma warning (disable : 4244)
#pragma warning (disable : 4996)

#include <Windows.h>
#include <stdio.h>
#include <TlHelp32.h>
#include <cstdint>
#include <thread>
#include <array>
#include <winternl.h>
#include <playsoundapi.h>
#include <codecvt>
#include <vector>
#include <string>
#include <algorithm>
#include "Custom.h"
#include "magnification.h"
#include <amp.h>
#pragma comment(lib, "magnification.lib")
static std::uint32_t pId = 0;
uintptr_t U_Base{};
uintptr_t gom{};
uintptr_t fps_camera{};





typedef struct _NULL_MEMORY
{
    void* buffer_address;
    UINT_PTR address;
    ULONGLONG size;
    ULONG pid;
    BOOLEAN write;
    BOOLEAN read;
    BOOLEAN req_base;
    void* output;
    const char* module_name;
    ULONG64 base_address;
    DWORD64 GA;
    DWORD64 UP;
}NULL_MEMORY;
struct HandleDisposer
{
    using pointer = HANDLE;
    void operator()(HANDLE handle) const
    {
        if (handle != NULL || handle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(handle);
        }
    }
};

inline uintptr_t oBaseAddress = 0;
using unique_handle = std::unique_ptr<HANDLE, HandleDisposer>;

template<typename ... Arg>
inline uint64_t CallHook(const Arg ... args)
{
    void* hooked_func = GetProcAddress(LoadLibrary("win32u.dll"), "NtGdiDdDDINetDispGetNextChunkInfo");

    auto func = static_cast<uint64_t(_stdcall*)(Arg...)>(hooked_func);

    return func(args ...);
}

inline static ULONG64 GetModuleBaseAddress(const char* module_name)
{
    NULL_MEMORY instructions = { 0 };
    instructions.pid = pId;
    instructions.req_base = TRUE;
    instructions.read = FALSE;
    instructions.write = FALSE;
    instructions.module_name = module_name;
    CallHook(&instructions);
    U_Base = instructions.UP;
    //std::cout << "GA : " << instructions.GA << std::endl;
    //std::cout << "UP : " << instructions.UP << std::endl;
    ULONG64 base = NULL;
    base = instructions.base_address;
    return base;
}

template <typename Type>
inline Type read(unsigned long long int Address)
{
    Type response{};
    NULL_MEMORY instructions;
    instructions.pid = pId;
    instructions.size = sizeof(Type);
    instructions.address = Address;
    instructions.read = TRUE;
    instructions.write = FALSE;
    instructions.req_base = FALSE;
    instructions.output = &response;
    CallHook(&instructions);

    return response;
}


template <typename Type>
inline Type readmem(unsigned long long int Address, int len)
{
    Type response{};
    NULL_MEMORY instructions;
    instructions.pid = pId;
    instructions.size = len;
    instructions.address = Address;
    instructions.read = TRUE;
    instructions.write = FALSE;
    instructions.req_base = FALSE;
    instructions.output = &response;
    CallHook(&instructions);

    return response;
}

inline void writefloat(unsigned long long int Address, float stuff)
{
    NULL_MEMORY instructions;
    instructions.address = Address;
    instructions.pid = pId;
    instructions.write = TRUE;
    instructions.read = FALSE;
    instructions.req_base = FALSE;
    instructions.buffer_address = (void*)&stuff;
    instructions.size = sizeof(float);

    CallHook(&instructions);
}


inline bool writevall(uintptr_t Address, PVOID value, ULONG write_size)
{
    NULL_MEMORY instructions;
    instructions.address = Address;
    instructions.pid = pId;
    instructions.write = TRUE;
    instructions.read = FALSE;
    instructions.req_base = FALSE;
    instructions.buffer_address = (void*)value;
    instructions.size = write_size;

    CallHook(&instructions);
    return true;
}
template<typename S>
inline bool writeval(UINT_PTR write_address, const S& value)
{
    return writevall(write_address, (PVOID)&value, sizeof(S));
}


inline void writedouble(unsigned long long int Address, double stuff)
{
    NULL_MEMORY instructions;
    instructions.address = Address;
    instructions.pid = pId;
    instructions.write = TRUE;
    instructions.read = FALSE;
    instructions.req_base = FALSE;
    instructions.buffer_address = (void*)&stuff;
    instructions.size = sizeof(double);

    CallHook(&instructions);
}

#pragma once
#include <Windows.h>

#define safe_read(Addr, Type) read<Type>(Addr)
#define safe_write(Addr, Data, Type) writeval<Type>(Addr, Data)
#define safe_memcpy(Dst, Src, Size) driver::write_memory(Sock, pid, Dst, driver::read_memory(Sock, pid, Src, 0, Size), Size)

int length(uintptr_t addr) { return safe_read(addr + 0x10, int); }

std::string readstring(uintptr_t addr) {
    try {
        static char buff[128] = { 0 };
        buff[length(addr)] = '\0';

        for (int i = 0; i < length(addr); ++i) {
            if (buff[i] < 128) {
                buff[i] = safe_read(addr + 0x14 + (i * 2), char);
            }
            else {
                buff[i] = '?';
                if (buff[i] >= 0xD800 && buff[i] <= 0xD8FF)
                    i++;
            }
        }
        return std::string(&buff[0], &buff[length(addr)]);
    }
    catch (const std::exception& exc) {}
    return "Error";
}
struct monostr
{
    char buffer[128];
};

std::string readchar(uintptr_t addr) {
    std::string str = safe_read(addr, monostr).buffer;
    if (!str.empty())
        return str;
    else
        return NULL;
}

#include "Main.h"
#include "Driver/Driver.hpp"
#include "xor_str.hpp"
#include "Vars.hpp"
#include "sdk.hpp"
#include "esp.hpp"


void decorations()
{

    ImGui::GetWindowDrawList()->AddRectFilled(
        ImGui::GetWindowPos() + ImVec2(590, 400),
        ImGui::GetWindowPos() + ImVec2(0, 0),
        ImColor(29, 29, 29),
        3
    );


    ImGui::GetWindowDrawList()->AddText(
        ImGui::GetWindowPos() + ImVec2(160, 20),
        ImColor(149, 137, 255),
        ("Name"
        ));
}


// Main code
int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Zoom, 0, 0, 0);
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, LOADER_BRAND, NULL };
    RegisterClassEx(&wc);
    main_hwnd = CreateWindow(wc.lpszClassName, LOADER_BRAND, WS_POPUP, 0, 0, 5, 5, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (!CreateDeviceD3D(main_hwnd)) {
        CleanupDeviceD3D();
        UnregisterClass(wc.lpszClassName, wc.hInstance);
        return 1;
    }

    // Show the window
    ShowWindow(main_hwnd, SW_HIDE);
    UpdateWindow(main_hwnd);

    // Setup Dear ImGui context
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr; //crutial for not leaving the imgui.ini file
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // Enable Multi-Viewport / Platform Windows
    auto font = io.Fonts->AddFontFromFileTTF(_("C:\\Windows\\Fonts\\calibrib.ttf"), 12.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle* style = &ImGui::GetStyle();
    style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
    style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_WindowBg] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
    style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
    style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
    style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
    style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
    style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(34, 44, 54, 0);
    style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(34, 44, 54, 0);
    style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(34, 44, 54, 0);
    style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
    style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
    style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
    style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
    style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
    style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
    style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
    style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);


    style->WindowBorderSize = 0.0f;
    style->GrabRounding = 0.0f;
    style->Alpha = 1.0f;
    style->GrabMinSize = 10.0f;
    style->FrameBorderSize = 0.0f;
    style->CurveTessellationTol = 3.25f;
    style->FramePadding = ImVec2(4, 4);
    style->FrameRounding = 0.0f;
    style->ItemSpacing = ImVec2(12, 8);
    style->ItemInnerSpacing = ImVec2(8, 6);
    style->IndentSpacing = 25.0f;
    style->ScrollbarSize = 1.0f;
    style->ScrollbarRounding = 0.0f;
    style->WindowPadding = ImVec2(0, 0);


    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(main_hwnd);
    ImGui_ImplDX9_Init(g_pd3dDevice);

  

    DWORD window_flags = ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_HorizontalScrollbar | ImGuiWindowFlags_AlwaysAutoResize;

    RECT screen_rect;
    GetWindowRect(GetDesktopWindow(), &screen_rect);
    auto x = float(screen_rect.right - WINDOW_WIDTH) / 2.f;
    auto y = float(screen_rect.bottom - WINDOW_HEIGHT) / 2.f;

    // Main loop
    MSG msg;
    ZeroMemory(&msg, sizeof(msg));
    while (msg.message != WM_QUIT)
    {
        if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
            continue;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX9_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        {
            ImGui::SetNextWindowPos(ImVec2(x, y), ImGuiCond_Once);
            ImGui::SetNextWindowSize(ImVec2(WINDOW_WIDTH, WINDOW_HEIGHT));
            ImGui::SetNextWindowBgAlpha(1.0f);

            ImGui::Begin(_(LOADER_BRAND), &loader_active, window_flags);
            {
                ImGui::SetCursorPos(ImVec2(10, 20));
                ImGui::BeginGroup();
                {
                    decorations();
                    ImGui::Text("");
                    if (Vars::Misc::EU) {
                        Retake::Checkbox(_("NoRecoil"), Vars::Misc::no_recoil); if (Vars::Misc::EU == true && Vars::Misc::RU == true) { Vars::Misc::RU = false; }
                    }
                    if (Vars::Misc::RU)
                    {
                        Retake::Checkbox(_(u8"Отключить отдачу"), Vars::Misc::no_recoil);
                    }
                    if (Vars::Misc::EU) {
                        Retake::Checkbox(_("NoSway"), Vars::Misc::no_sway);  if (Vars::Misc::EU == true && Vars::Misc::RU == true) { Vars::Misc::RU = false; }
                    }
                    if (Vars::Misc::RU)
                    {
                        Retake::Checkbox(_(u8"Отключить покачивание"), Vars::Misc::no_sway); 
                    }
                    if (Vars::Misc::EU) 
                    {
                        Retake::Checkbox(_("Zoom Hack"), Vars::Misc::zoomhack); if (Vars::Misc::EU == true && Vars::Misc::RU == true) { Vars::Misc::RU = false; }
                        if (Vars::Misc::zoomhack == true) {
                            ImGui::KeyBind(_("Zoom bind"), Vars::Misc::zoombind, ImVec2(100, 20));
                            Retake::SliderInt(_("Zoom Value"), &Vars::Misc::zoomvalue, 1, 6, _("%d"));
                        }
                    }
                    if (Vars::Misc::RU)
                    {
                        Retake::Checkbox(_(u8"Зум хак"), Vars::Misc::zoomhack);
                        if (Vars::Misc::zoomhack == true) {
                            ImGui::KeyBind(_(u8"Бинд зума"), Vars::Misc::zoombind, ImVec2(100, 20));
                            Retake::SliderInt(_(u8"Дальность зума"), &Vars::Misc::zoomvalue, 1, 6, _("%d"));
                        }
                    }
                    else if (Vars::Misc::zoomhack == false) {
                        Vars::Misc::zoombind = 0;
                    }
                    if (Vars::Misc::EU) {
                        Retake::Checkbox(_("ThermalVision"), Vars::Misc::ThermalVision); if (Vars::Misc::EU == true && Vars::Misc::RU == true) { Vars::Misc::RU = false; }
                    }
                    if (Vars::Misc::RU)
                    {
                        Retake::Checkbox(_(u8"Тепловизор"), Vars::Misc::ThermalVision);
                    }
                    if (Vars::Misc::EU) {
                        Retake::Checkbox(_("Visor"), Vars::Misc::Visor);  if (Vars::Misc::EU == true && Vars::Misc::RU == true) { Vars::Misc::RU = false; }
                    }
                    if (Vars::Misc::RU)
                    {
                        Retake::Checkbox(_(u8"Убрать забрало у шлема"), Vars::Misc::ThermalVision);
                    }
                    if (Vars::Misc::EU) {
                        Retake::Checkbox(_("Chams"), Vars::Misc::Chams); if (Vars::Misc::EU == true && Vars::Misc::RU == true) { Vars::Misc::RU = false; }
                    }
                    if (Vars::Misc::RU)
                    {
                        Retake::Checkbox(_(u8"Чамсы"), Vars::Misc::Chams);
                    }
                    if (Vars::Misc::EU) {
                        Retake::Checkbox(_("SpeedHack"), Vars::Misc::speedHack); if (Vars::Misc::EU == true && Vars::Misc::RU == true) { Vars::Misc::RU = false; }
                    }
                    if (Vars::Misc::RU)
                    {
                        Retake::Checkbox(_(u8"Ускорение"), Vars::Misc::Chams);
                    }
                    Retake::Checkbox(_("RU"), Vars::Misc::RU);
                    ImGui::SameLine();
                    Retake::Checkbox(_("EU"), Vars::Misc::EU);

                    ImGui::Text("p1d: %d", pId);
                    ImGui::SameLine();
                    ImGui::Text("uBase: %d", U_Base);

                    if(ImGui::Button("Exit", ImVec2(100, 20)))
                        exit(1);
                    ImGui::EndGroup();
                }

            }
            ImGui::End();
        }
        ImGui::EndFrame();

        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0f, 0);
        if (g_pd3dDevice->BeginScene() >= 0)
        {
            ImGui::Render();
            ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
            g_pd3dDevice->EndScene();
        }

        // Update and Render additional Platform Windows
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        HRESULT result = g_pd3dDevice->Present(NULL, NULL, NULL, NULL);

        // Handle loss of D3D9 device
        if (result == D3DERR_DEVICELOST && g_pd3dDevice->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) {
            ResetDevice();
        }
        if (!loader_active) {
            msg.message = WM_QUIT;
        }
    }

    ImGui_ImplDX9_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();

    CleanupDeviceD3D();
    DestroyWindow(main_hwnd);
    UnregisterClass(wc.lpszClassName, wc.hInstance);

    return 0;
}

DWORD FindPID(const char* szWndName)
{
    DWORD PID = 0;
    DWORD ThreadID = GetWindowThreadProcessId(FindWindowA(szWndName, nullptr), &PID);
    return PID;
}


DWORD __stdcall InitHooks(LPVOID)
{
    pId = FindPID(_("UnityWndClass"));

    U_Base = GetModuleBaseAddress("UnityPlayer.dll");
    std::thread([&]()
        {
            entity_list.update();
        }).detach();


        std::thread([&]()
            {
                entity_list.run();
           }).detach();

    return true;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;
 

    CreateThread(0, 0, (LPTHREAD_START_ROUTINE)InitHooks, 0, 0, 0);


    switch (msg)
    {
    case WM_SIZE:
        if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
        {
            g_d3dpp.BackBufferWidth = LOWORD(lParam);
            g_d3dpp.BackBufferHeight = HIWORD(lParam);
            ResetDevice();
        }
        return 0;
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) 
            return 0;
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hWnd, msg, wParam, lParam);
}
//BOOL APIENTRY DllMain(HMODULE hModule,
//    DWORD  ul_reason_for_call,
//    LPVOID lpReserved
//)
//{
//    switch (ul_reason_for_call)
//    {
//    case DLL_PROCESS_ATTACH:
//        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)WinMain, 0, 0, 0); 
//    case DLL_THREAD_ATTACH:
//    case DLL_THREAD_DETACH:
//    case DLL_PROCESS_DETACH:
//        break;
//    }
//    return TRUE;
//}