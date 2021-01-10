#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

DWORD getProcessId(const char* process_name) {
	DWORD process_id = 0;
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (snapshot != INVALID_HANDLE_VALUE) {
		PROCESSENTRY32 process_entry;
		process_entry.dwSize = sizeof(process_entry);

		if (Process32First(snapshot, &process_entry)) {
			do {
				if (!_stricmp(process_entry.szExeFile, process_name)) {
					process_id = process_entry.th32ProcessID;
					break;
				}
			} while (Process32Next(snapshot, &process_entry));
		}
	}

	return process_id;
}

HMODULE injectDLL(HANDLE process_handle, const char* dll_path) {
	void* location = VirtualAllocEx(process_handle, NULL, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (location) {
		WriteProcessMemory(process_handle, location, dll_path, strlen(dll_path), NULL);
	}

	HANDLE thread_handle = CreateRemoteThread(process_handle, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, location, 0, NULL);

	DWORD ret = 0;

	if (thread_handle && thread_handle != INVALID_HANDLE_VALUE) {
		WaitForSingleObject(thread_handle, INFINITE);
		GetExitCodeThread(thread_handle, &ret);

		CloseHandle(thread_handle);
	}

	if (location) {
		VirtualFreeEx(process_handle, location, 0, MEM_RELEASE);
	}

	return (HMODULE) ret;
}

BOOL ejectDLL(HANDLE process_handle, HMODULE module_handle) {
	HANDLE thread_handle = CreateRemoteThread(process_handle, NULL, 0, (LPTHREAD_START_ROUTINE)FreeLibrary, module_handle, 0, NULL);

	DWORD ret = 0;

	if (thread_handle && thread_handle != INVALID_HANDLE_VALUE) {
		WaitForSingleObject(thread_handle, INFINITE);
		GetExitCodeProcess(thread_handle, &ret);

		CloseHandle(thread_handle);
	}

	return !ret;
}

int main(int argc, char* argv[]) {
	if (argc != 4 || !(!strcmp(argv[1], "ADD") || !strcmp(argv[1], "REMOVE"))) {
		std::cout << "Please provide exactly 3 arguments." << std::endl;
		std::cout << "Usage: " << argv[0] << " ADD    EXE_NAME DLL_PATH" << std::endl;
		std::cout << "       " << argv[0] << " REMOVE EXE_NAME MODULE" << std::endl;

		return 1;
	}

	if (!strcmp(argv[1], "ADD")) {
		std::cout << "Now injecting DLL into target..." << std::endl;
		std::cout << "EXE_NAME: " << argv[2] << std::endl;
		std::cout << "DLL_PATH: " << argv[3] << std::endl;
	}
	else {
		std::cout << "Now ejecting DLL from target..." << std::endl;
		std::cout << "EXE_NAME: " << argv[2] << std::endl;
		std::cout << "MODULE:   " << argv[3] << std::endl;
	}

	DWORD process_id = getProcessId(argv[2]);

	HANDLE process_handle = OpenProcess(PROCESS_ALL_ACCESS, NULL, process_id);

	if (process_handle && process_handle != INVALID_HANDLE_VALUE) {
		if (!strcmp(argv[1], "ADD")) {
			HMODULE module_handle = injectDLL(process_handle, argv[3]);

			std::cout << "MODULE:   0x" << module_handle << std::endl;
		}
		else {
			HMODULE module_handle = (HMODULE)strtol(argv[3], NULL, 16);
			BOOL eject_successful = ejectDLL(process_handle, module_handle);
		}
	}

	if (process_handle) {
		CloseHandle(process_handle);
	}

	return 0;
}