#include "hacked_dll.h"
#include <stdio.h>
#include <Windows.h>
#include <vector>
#include <string>
#include "detours.h"

LPCSTR dll = "GameLogic.dll";

PVOID exported_function = DetourFindFunction(dll, "?GetUserId@GameAPI@@QAEHXZ"); // Address = 0x1001ee80

// Create struct for injection
typedef struct {
	PVOID original_function;
	PVOID injected_function;
} InjectionTuple;

// Get all original functions
PVOID original_get_mana_function = (PVOID)((DWORD)exported_function + 0x310F0); // GetMana() 
PVOID original_walk_speed_function = (PVOID)((DWORD)exported_function + 0x31110); // GetWalkingSpeed()
PVOID original_jump_speed_function = (PVOID)((DWORD)exported_function + 0x31120); // GetJumpSpeed()
PVOID original_look_rotation_function = (PVOID)((DWORD)exported_function + 0x327C0); // GetLookRotation()
PVOID original_can_jump_function = (PVOID)((DWORD)exported_function + 0x32800); // CanJump()
PVOID original_chat_function = (PVOID)((DWORD)exported_function + 0x36320); // Chat()
PVOID original_get_circuit_input_function = (PVOID)((DWORD)exported_function + 0x376B0); // GetCircuitInputs


// Create list of injections and fill it
std::vector<InjectionTuple> injections{
	{ original_walk_speed_function, fakeWalkSpeed },
	{ original_jump_speed_function, fakeJumpSpeed },
	{ original_can_jump_function, fakeCanJump },
	//{ original_look_rotation_function, fakeLookRotation },
	//{ original_get_circuit_input_function, fakeGetInputs },
	{ original_get_mana_function, fakeHealthAndMana },
};


float fakeWalkSpeed() {
	return 10000.0f;
}

float fakeJumpSpeed() {
	return 2500.0f;
}

int __stdcall fakeCanJump() {
	char chat_input[] = "This is a test string!";
	((void(__stdcall *)(char *))original_chat_function)(chat_input);

	return 1;
}

int fakeHealthAndMana() {
	return 1000;
}

int __stdcall fakeGetInputs(char* parameter) {
	//((void(__stdcall*)(char*))original_chat_function)(parameter);
	return ((int(__stdcall*)(char*))original_get_circuit_input_function)(parameter);
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
	(void)hinst;
	(void)reserved;

	if (DetourIsHelperProcess()) {
		return TRUE;
	}

	if (dwReason == DLL_PROCESS_ATTACH) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		for (auto& injection : injections) {
			DetourAttach(&injection.original_function, injection.injected_function);
		}
		DetourTransactionCommit();
	} else if (dwReason == DLL_PROCESS_DETACH) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		for (auto& injection : injections) {
			DetourDetach(&injection.original_function, injection.injected_function);
		}
		DetourTransactionCommit();
	}

	return TRUE;
}
