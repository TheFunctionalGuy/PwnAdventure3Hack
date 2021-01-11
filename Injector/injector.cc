#include <iostream>
#include <Windows.h>
#include "detours.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Please provide exactly 2 arguments." << std::endl;
        std::cout << "Usage: " << argv[0] << " EXE_PATH DLL_PATH" << std::endl;

        return 1;
    }

    std::cout << "EXE_PATH: " << argv[1] << std::endl;
    std::cout << "DLL_PATH: " << argv[2] << std::endl;

    // Information necessary to create process with DLL
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(STARTUPINFO));
    ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
    si.cb = sizeof(STARTUPINFO);

    // Convert parameter to expected format
    size_t newsize = strlen(argv[1]) + 1;
    LPWSTR w_path_to_exe = new wchar_t[newsize];
    size_t converted_chars = 0;
    mbstowcs_s(&converted_chars, w_path_to_exe, newsize, argv[1], _TRUNCATE);

    // Create new process with DLL injected
    BOOL success = DetourCreateProcessWithDllEx(NULL, w_path_to_exe, NULL,
        NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL,
        &si, &pi, argv[2], NULL);
    
    if (success) {
        std::cout << "Injection and start were successful." << std::endl;
    } else {
        std::cout << "Injection and start were NOT successful." << std::endl;
    }

    return 0;
}
