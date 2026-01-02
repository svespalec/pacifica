#pragma once

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>
#include <Psapi.h>

#include <cstdio>
#include <cstdint>

#include <detours/detours.h>

#pragma comment( lib, "psapi.lib" )

#define BUILD_NUMBER "0.1"

#define LOG(fmt, ...) printf("[ pacifica ] " fmt "\n", ##__VA_ARGS__)