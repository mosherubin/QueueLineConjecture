#pragma once
#if defined (_WIN32)
#include <windows.h> 
#include <stdio.h>
#include "./QueueLineConjecture.h"

BOOL CtrlHandler(DWORD fdwCtrlType);
BOOL QlcSetConsoleCtrlHandler(QueueLineConjecture *qlc);
BOOL QlcClearConsoleCtrlHandler();

#endif