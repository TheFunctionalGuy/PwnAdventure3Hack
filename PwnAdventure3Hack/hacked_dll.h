#pragma once
#include <Windows.h>

#ifdef REPL_LIB_EXPORTS
#define REPL_LIB_API __declspec(dllexport)
#else
#define REPL_LIB_API __declspec(dllimport)
#endif

float fakeWalkSpeed();
float fakeJumpSpeed();
int __stdcall fakeCanJump();
int fakeHealthAndMana();
int __stdcall fakeGetInputs(char* parameter);
