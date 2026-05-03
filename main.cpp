
#include "pch.h"
#include "main.h"

using DirectInput8Create_f = HRESULT (*WINAPI) (HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter);
static DirectInput8Create_f OriginalDirectInput8Create = nullptr;
static bool proxyDone = false;
static bool libsLoaded = false;

static void LoadProxyFunction()
{
    if (proxyDone)
        return;

    char buffer[MAX_PATH];
    GetSystemDirectoryA(buffer, MAX_PATH);
    strcat_s(buffer, "\\dinput8.dll");

    // Original dinput8 in system 32 (hopefully)
    auto module = LoadLibraryA(buffer);
    if (!module)
        return;

    OriginalDirectInput8Create = reinterpret_cast<DirectInput8Create_f>(GetProcAddress(module, "DirectInput8Create"));
    if (OriginalDirectInput8Create)
        proxyDone = true;
}

static void LoadLibraries()
{
    if (libsLoaded)
        return;

    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);

    auto path = std::filesystem::path(buffer);
    path = path.parent_path();
    path /= "mods";

    logger::println("Mods Path location: %s", path.string().c_str());
    
    std::string fileName;
    for (const auto& file : std::filesystem::directory_iterator(path))
    {
        logger::println("Found Mod: %s", file.path().string().c_str());
        if (file.path().extension().string() == ".dll")
        {
            fileName = file.path().string();
            LoadLibraryA(fileName.c_str());
        }
    }

    libsLoaded = true;
}

static DWORD WINAPI MainThread(LPVOID lParam)
{
#ifdef _DEBUG
    AllocConsole();
    SetStdHandle(STD_INPUT_HANDLE, GetStdHandle(STD_INPUT_HANDLE));
    SetStdHandle(STD_OUTPUT_HANDLE, GetStdHandle(STD_OUTPUT_HANDLE));

    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stdin, "CONIN$", "r", stdin);
#endif
    //LoadProxyFunction();
    //LoadLibraries();

    return 0;
}

extern "C" HRESULT WINAPI DirectInput8Create(HINSTANCE hinst, DWORD dwVersion, REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter)
{
    if (!proxyDone)
    {
        // If it failes we are crashing anyways :)
        LoadProxyFunction();
    }
    if (!libsLoaded)
    {
        LoadLibraries();
    }
    return OriginalDirectInput8Create(hinst, dwVersion, riidltf, ppvOut, punkOuter);
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        DisableThreadLibraryCalls(hModule);
        CreateThread(NULL, 0, &MainThread, NULL, NULL, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
