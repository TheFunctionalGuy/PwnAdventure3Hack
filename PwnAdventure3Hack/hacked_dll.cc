#include "hacked_dll.h"
#include <stdio.h>
#include <Windows.h>
#include <vector>
#include "detours.h"

LPCSTR dll = "GameLogic.dll";

PVOID exported_function = DetourFindFunction(dll, "?GetUserId@GameAPI@@QAEHXZ");

// Create struct for injection
typedef struct {
	PVOID original_function;
	PVOID injected_function;
} InjectionTuple;

// Get all original functions
PVOID original_walk_speed_function = (PVOID)((DWORD)exported_function + 0x31110); // GetWalkingSpeed()
PVOID original_jump_speed_function = (PVOID)((DWORD)exported_function + 0x31120); // GetJumpSpeed()
PVOID original_can_jump_function = (PVOID)((DWORD)exported_function + 0x32800); // CanJump()

// Create list of injections and fill it
std::vector<InjectionTuple> injections{
	{ original_walk_speed_function, fake_walk_speed },
	{ original_jump_speed_function, fake_jump_speed },
	{ original_can_jump_function, fake_can_jump }
};

float fake_walk_speed() {
	return 5000.0f;
}

float fake_jump_speed() {
	return 5000.0f;
}

int fake_can_jump() {
	return 1;
}



BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
	LONG error;
	(void)hinst;
	(void)reserved;

	if (DetourIsHelperProcess()) {
		return TRUE;
	}

	if (dwReason == DLL_PROCESS_ATTACH) {
		for (auto& injection : injections) {
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			DetourAttach(&injection.original_function, injection.injected_function);
			DetourTransactionCommit();
		}
	} else if (dwReason == DLL_PROCESS_DETACH) {
		for (auto& injection : injections) {
			DetourTransactionBegin();
			DetourUpdateThread(GetCurrentThread());
			DetourDetach(&injection.original_function, injection.injected_function);
			DetourTransactionCommit();
		}
	}

	return TRUE;
}
