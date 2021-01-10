#pragma once
#include <Windows.h>

#ifdef REPL_LIB_EXPORTS
#define REPL_LIB_API __declspec(dllexport)
#else
#define REPL_LIB_API __declspec(dllimport)
#endif

extern "C" REPL_LIB_API float fake_walk_speed(VOID* empty);
