#include "test.h"
#include <stdio.h>
#include <Windows.h>
#include "detours.h"

float fake_walk_speed(VOID* empty) {
	return 0.0f;
}

BOOL WINAPI DllMain(HINSTANCE hinst, DWORD dwReason, LPVOID reserved) {
	LONG error;
	(void)hinst;
	(void)reserved;

	if (DetourIsHelperProcess()) {
		return TRUE;
	}

	PVOID original_walk_speed_function = DetourFindFunction("GameLogic.dll", "Player::GetWalkingSpeed");

	if (dwReason == DLL_PROCESS_ATTACH) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&original_walk_speed_function, fake_walk_speed);
		error = DetourTransactionCommit();

		if (error == NO_ERROR) {
			printf("Sucessful detour! :)\n");
		} else {
			printf("Error during detour...\n");
		}
	} else if (dwReason == DLL_PROCESS_DETACH) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourDetach(&original_walk_speed_function, fake_walk_speed);
		error = DetourTransactionCommit();
	}

	return TRUE;
}
