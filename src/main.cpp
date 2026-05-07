#include "engine.h"

#ifdef _WIN32
#include <filesystem>
#include <iostream>
#include <windows.h>
#endif

int main(int argc, char *argv[]) {
    (void)argc;
    (void)argv;

#ifdef _WIN32

    // Windows searches for DLLs in either current directory or system folders
    // Too many dlls next to an executable is annoying
    // Windows has no concept of rpath like Unix, so we add the folder manually

    // Get the path of the executable
    wchar_t buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    std::filesystem::path exePath = std::filesystem::path(buffer).parent_path();

    // Combine with the 'libs' subfolder
    std::filesystem::path libPath = exePath / "libs";

    // Add to DLL search path
    if (!SetDllDirectoryW(libPath.c_str())) {
        std::cerr << "Failed to set DLL directory." << std::endl;
    }
#endif

    Engine engine;
    engine.startup();
    engine.loop();

    return 0;
}
