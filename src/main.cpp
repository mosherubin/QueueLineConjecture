#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <set>
#include <iostream>
#include "./QueueLineConjecture.h"
#include "ConsoleCtrlHandler.h"

struct RunArguments
{
	__int64 targetNumber;
	bool useUniqueFactors;
	bool enableTrace;
	bool usePdcPrimes;
};

bool ProcessArguments(int argc, char *argv[], RunArguments &runArgs)
{
	if (argc < 5)
	{
		printf("Error: Incorrect number of arguments passed, terminating\n");
		return false;
	}

	runArgs.targetNumber = atoll(argv[1]);
	runArgs.useUniqueFactors = (strcmp(argv[2], "0") != 0);
	runArgs.enableTrace = (strcmp(argv[3], "0") != 0);
	runArgs.usePdcPrimes = (strcmp(argv[4], "0") != 0);

	if (runArgs.targetNumber <= 0)
	{
		printf("Error: Target number \"%lld\" is not valid, terminating\n", runArgs.targetNumber);
		return false;
	}

	return true;
}

void usage(int argc, char *argv[])
{
	printf("Usage: %s highval use-unique-factors enable-trace use-pdc-primes\n", argc > 0 ? argv[0] : "QLC.exe");
}

bool ConfigureQlc (QueueLineConjecture *qlc, RunArguments runArgs)
{
	if (qlc == 0)
	{
		return false;
	}
	qlc->SetUniqueFactors(runArgs.useUniqueFactors);
	qlc->SetTrace(runArgs.enableTrace);
	qlc->SetUsePdcPrimes(runArgs.usePdcPrimes);

	return true;
}

int main(int argc, char *argv[])
{
	RunArguments runArgs;

	if (!ProcessArguments(argc, argv, runArgs))
	{
		usage(argc, argv);
		exit(-1);
	}

	/*DEBUG*/ printf("Target Number = %lld\n", runArgs.targetNumber);
	
	QueueLineConjecture *qlc = new QueueLineConjecture();
	if (!ConfigureQlc(qlc, runArgs))
	{
		printf("ConfigureQlc() returned false, exiting\n");
		exit(-1);
	}

	if (!QlcSetConsoleCtrlHandler(qlc))
	{
		printf("\nERROR: Could not set control handler");
		exit(-1);
	}

	printf("Starting time: %s\n", qlc->ReturnTimestamp().c_str());
	qlc->DoIt(runArgs.targetNumber);
	QlcClearConsoleCtrlHandler();
	printf("Ending time: %s\n", qlc->ReturnTimestamp().c_str());

	return 0;
}
