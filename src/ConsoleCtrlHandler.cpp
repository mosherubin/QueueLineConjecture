#include "ConsoleCtrlHandler.h"
#include "./QueueLineConjecture.h"

static QueueLineConjecture *internalQlc = 0;

#if defined (_WIN32)
// https://docs.microsoft.com/en-us/windows/console/registering-a-control-handler-function
BOOL CtrlHandler(DWORD fdwCtrlType)
{
	switch (fdwCtrlType)
	{
		// Handle the CTRL-C signal. 
	case CTRL_C_EVENT:
		printf("Ctrl-C event caught, dumping prime list and exiting\n\n");
		Beep(750, 300);
		internalQlc->SetToExitLoop();
		return (TRUE);

		// CTRL-CLOSE: confirm that the user wants to exit. 
	case CTRL_CLOSE_EVENT:
		Beep(600, 200);
		printf("Ctrl-Close event\n\n");
		return(TRUE);

		// Pass other signals to the next handler. 
	case CTRL_BREAK_EVENT:
		Beep(900, 200);
		printf("Ctrl-Break event\n\n");
		return FALSE;

	case CTRL_LOGOFF_EVENT:
		Beep(1000, 200);
		printf("Ctrl-Logoff event\n\n");
		return FALSE;

	case CTRL_SHUTDOWN_EVENT:
		Beep(750, 500);
		printf("Ctrl-Shutdown event\n\n");
		return FALSE;

	default:
		return FALSE;
	}
}

BOOL QlcSetConsoleCtrlHandler(QueueLineConjecture *qlc)
{
	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE))
	{
		printf("\nThe Control Handler is installed.\n");
		internalQlc = qlc;
		return TRUE;
	}
	else
	{
		printf("\nERROR: Could not set control handler");
		return FALSE;
	}
}

BOOL QlcClearConsoleCtrlHandler()
{
	internalQlc = 0;

	if (SetConsoleCtrlHandler((PHANDLER_ROUTINE)NULL, FALSE))
	{
		printf("\nThe default Control Handler is restored.\n");
		return TRUE;
	}
	else
	{
		printf("\nERROR: Could not restore control handler");
		return FALSE;
	}
}
#endif