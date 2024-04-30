#pragma once
#include <stdbool.h>

#if defined(_WIN32)
#include <conio.h>
#include <windows.h>
#define SLEEP_MS(ms) Sleep(ms)
#define CLEAR_TERMINAL() system("cls")
#elif defined(__linux__)
#include <unistd.h>
#define SLEEP_MS(ms) usleep(ms * 1000)
#define CLEAR_TERMINAL() system("clear")
#endif

bool isKeyPressed(char c);