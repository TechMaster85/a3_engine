#include "engine.h"

#include <cstdlib>

#ifdef _WIN32
#include <windows.h>
#include <filesystem>
#include <iostream>
#endif

int main(int argc, char *argv[]) {
// Searches for dll files in libs/ folder so they don't pollute main folder
#ifdef _WIN32
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

    (void)argc;
    (void)argv;

    Engine engine;
    engine.startup();
    engine.loop();

    return 0;
}
