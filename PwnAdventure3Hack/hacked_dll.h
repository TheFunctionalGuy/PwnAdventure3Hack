#pragma once
#include <Windows.h>

#ifdef REPL_LIB_EXPORTS
#define REPL_LIB_API __declspec(dllexport)
#else
#define REPL_LIB_API __declspec(dllimport)
#endif

float fake_walk_speed();
float fake_jump_speed();
int fake_can_jump();