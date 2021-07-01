#pragma once
#include <Windows.h>
#include <iostream>
#include <fstream>

#include "Discord/discord.h"
#include "PresenceDat.h"

#define DLLExport   __declspec( dllexport )
#define DLLImport   __declspec( dllimport )

extern "C" {
	DLLExport discord::Activity readActivity();
	DLLExport float readUptime();
}