#include <stdio.h>
#include <ctime>
#include <algorithm>
#include "QueueLineConjecture.h"

bool PrimeListElementPredicate(const PrimeListElement &a, const PrimeListElement &b)
{
	return (a._prime < b._prime);
}

// The following insert_sorted templates taken from https://stackoverflow.com/a/25524075/562242
template< typename T >
typename std::vector<T>::iterator
insert_sorted(std::vector<T> & vec, T const& item)
{
	return vec.insert
	(
		std::upper_bound(vec.begin(), vec.end(), item),
		item
	);
}

template< typename T, typename Pred >
typename std::vector<T>::iterator
insert_sorted(std::vector<T> & vec, T const& item, Pred pred)
{
	return vec.insert
	(
		std::upper_bound(vec.begin(), vec.end(), item, pred),
		item
	);
}

QueueLineConjecture::QueueLineConjecture()
{
	traceEnabled = false;
	/*DEBUG*/ useQlc = true;
	returnUniqueFactors = true;
	usePdcPrimes = true;
	exitLoop = false;

	primeList.clear();
	pdcList.clear();
}

QueueLineConjecture::~QueueLineConjecture()
{

}

void QueueLineConjecture::SetTrace (bool enable)
{
	traceEnabled = enable;
}

void QueueLineConjecture::SetUniqueFactors(bool useUniqueFactors)
{
	returnUniqueFactors = useUniqueFactors;
}

void QueueLineConjecture::SetUsePdcPrimes(bool usePdcPrimesForPrimality)
{
	usePdcPrimes = usePdcPrimesForPrimality;
}

void QueueLineConjecture::InitializePrimeList()
{
	for (int i = 3; i <= 7; i += 2)
	{
		PrimeListElement elem;

		elem._prime = i;
		elem._tally = 0;
		insert_sorted(primeList, elem, PrimeListElementPredicate);
	}
}

void QueueLineConjecture::InitializePdcList()
{
	for (__int64 i = 3; i <= 7; i += 2)
	{
		insert_sorted(pdcList, i);
	}
}

void QueueLineConjecture::HandleNewPrime(__int64 prime)
{
	// <i> is prime, update <primeList> and <pdcList>
	if (traceEnabled)
	{
		/*DEBUG*/ printf("PRIME: %lld\n", prime);
	}

	PrimeListElement elem;
	elem._prime = prime;
	elem._tally = 0;
	insert_sorted(primeList, elem, PrimeListElementPredicate);

	if (primeList[primeList.size() - 2]._tally > 1)
	{
		// The newly added prime is a PDC
		AddToPdc(prime);
	}

	if (traceEnabled)
	{
		/*DEBUG*/ DumpLists(stdout, "After", prime);
	}
}

void QueueLineConjecture::HandleOddComposite(__int64 composite, std::vector<__int64> &factor)
{
	std::vector<__int64>::iterator it;
	__int64 index = -1;

	// <i> id composite, there are factors, update both <primeList> and <pdcList>
	/*DEBUG*/ if (traceEnabled) printf("COMPOSITE: %lld\n", composite);
	for (it = factor.begin(); it != factor.end(); ++it)
	{
		index = ReturnIndexInPrimeList(*it);
		UpdatePdcList(index);

		// Now increment the tally count
		primeList[index]._tally += 1;
	}
}

void QueueLineConjecture::DoIt(__int64 n)
{
	std::vector<__int64> factor;
	__int64 i;

	InitializePrimeList();
	InitializePdcList();

	for (i = 9; i <= n; i += 2)
	{
		/*DEBUG*/ if (i % 100000 == 1)
		{
			fprintf(stderr, "%s: Reached %lld in loop (target is %lld)\n", ReturnTimestamp().c_str(), i, n);
		}
		
		if (traceEnabled)
		{
			/*DEBUG*/ printf("==========================================================\n");
			/*DEBUG*/ printf("Processing %lld\n", i);
			/*DEBUG*/ printf("==========================================================\n");
			/*DEBUG*/ DumpLists(stdout, "Before", i);
		}

		FactorNumber(i, factor);

		if (factor.size() == 0)
		{
			HandleNewPrime(i);
			//continue;
		}
		else
		{
			HandleOddComposite(i, factor);
		}

		if (traceEnabled)
		{
			/*DEBUG*/ DumpLists(stdout, "After", i);
			/*DEBUG*/ DumpFactors(i, factor);
		}

		if (exitLoop)
			break;
	}

	__int64 numberToFactor = n;

	if (exitLoop)
	{
		// We exited the above loop prematurely due to receiving a Ctrl-C event.
		// We cannot factor <n> as we might not have all the primes up to <n>.  
		// Therefore we'll factor <i> itself.
		numberToFactor = i;
	}
	
	// Now that we've factored all numbers up to, but not including <n>, let's factor <n> itself.
	if (traceEnabled) printf("Before FactorNumber: %s\n", ReturnTimestamp().c_str());
	FactorNumber(numberToFactor, factor);
	if (traceEnabled) printf("After FactorNumber: %s\n", ReturnTimestamp().c_str());

	DumpLists(stdout, "Final Results", numberToFactor);

	printf("The factors of %lld are:\n", numberToFactor);
	for (std::vector<__int64>::iterator it=factor.begin(); it != factor.end(); ++it)
	{
		printf("\t%lld\n", (*it));
	}
}

std::string QueueLineConjecture::ReturnTimestamp()
{
	time_t t = time(0);   // get time now
	struct tm * now = localtime(&t);

	char ts[1024];

	sprintf(ts, "%04d-%02d-%02d %02d:%02d:%02d",
		now->tm_year + 1900,
		now->tm_mon + 1,
		now->tm_mday,
		now->tm_hour,
		now->tm_min,
		now->tm_sec
	);

	return ts;
}

std::vector<__int64> &QueueLineConjecture::FactorNumber(__int64 number, std::vector<__int64> &factor)
{
	__int64 temp = number;
	__int64 lastIndex = -1;

	factor.clear();

	for (__int64 i = 0; ;)
	{
		if (useQlc)
		{
			if (usePdcPrimes)
			{
				if (i >= (__int64)pdcList.size())
					break;
			}
			else
			{
				if (i >= (__int64)primeList.size())
					break;
			}
		}
		else
		{
			if (primeList[i]._prime * primeList[i]._prime > number)
				break;
		}

		// See https://stackoverflow.com/a/28283447 for a faster way of checking divisibility, rather than using modulus

		//__int64 quotient = temp / primeList[i]._prime;
		//if (quotient * primeList[i]._prime == temp)
		__int64 divisor = usePdcPrimes ? pdcList[i] : primeList[i]._prime;
		if (temp % divisor == 0)
		{
			// primeList[i] is a factor
			if (!returnUniqueFactors || (i != lastIndex))
			{
				// We want to add pdcList[i] more than once to the list of factors
				insert_sorted(factor, divisor);
			}

			temp = temp / divisor;
			if (temp == 1)
			{
				// <number> is fully factored
				break;
			}

			lastIndex = i;
		}
		else
		{
			// <pdcList[i] is NOT a factor, continue to the next prime candidate
			lastIndex = i;
			++i;
		}
	}

	if ((temp != 1) && (temp != number))
	{
		// <temp> is a prime factor
		insert_sorted(factor, temp);
	}

	return factor;
}

void QueueLineConjecture::AddToPdc(__int64 prime)
{
	insert_sorted(pdcList, prime);
	if (traceEnabled)
	{
		/*DEBUG*/ printf("Adding %lld to PDC\n", prime);
	}
}

void QueueLineConjecture::RemoveFromPdc(__int64 prime)
{
	std::vector<__int64>::iterator it = std::find(pdcList.begin(), pdcList.end(), prime);

	if (it != pdcList.end())
	{
		pdcList.erase(it);
		if (traceEnabled)
		{
			/*DEBUG*/ printf("Removed %lld from PDC\n", prime);
		}
	}
}

void QueueLineConjecture::DumpPrimeList(FILE *f, char *desc)
{
	fprintf(f, "-------------------------------\n");
	fprintf(f, "primeList (%s):\n", desc);
	for (int j = 0; j < primeList.size(); ++j)
	{
		if (primeList[j]._tally == 0)
		{
			// From this point onwards, all primes will have a count of zero, so exit now.
			break;
		}

		fprintf(f, "\t%lld\t%lld\n", primeList[j]._prime, primeList[j]._tally);
	}
}

void QueueLineConjecture::DumpPdcList(FILE *f, char *desc)
{
	fprintf(f, "-------------------------------\n");
	fprintf(f, "pdcList (%s):\n", desc);
	for (int k = 0; k < pdcList.size(); ++k)
	{
		fprintf(f, "\t%lld (tally: %lld)\n", pdcList[k], ReturnPrimeTally(pdcList[k]));
	}
}

void QueueLineConjecture::DumpFactors(__int64 n, std::vector<__int64> &factor)
{
	printf("-------------------------------\n");
	printf("factors of %lld: ", n);
	for (std::vector<__int64>::iterator it = factor.begin(); it != factor.end(); ++it)
	{
		printf("%lld ", *it);
	}
	printf("\n");
}

void QueueLineConjecture::DumpLists(FILE *f, const char *s, __int64 i)
{
	char buffer[1024];

	/*DEBUG*/ sprintf(buffer, "%s processing %lld", s, i);
	/*DEBUG*/ DumpPrimeList(f, buffer);
	/*DEBUG*/ sprintf(buffer, "%s processing %lld", s, i);
	/*DEBUG*/ DumpPdcList(f, buffer);
}

void QueueLineConjecture::UpdatePdcList(__int64 indexOfThisPrime)
{
	/*
	Case #| lowerPrimeTally|thisPrimeTally|higherPrimeTally || removePdc | addPdc | addPdc 
          |                |              |                 || (this) ?  | (this)?| (higher)?
	------| ---------------|--------------|-----------------||------------+--------+----------
	0     |    -           |     -        |       -         ||   -      |   Y    |    -
	
	1     |    -           |     0        |       -         ||   -      |   Y    |    -
	2     |    -           |     0        |       0         ||   -      |   Y    |    -
	3     |    1           |     0        |       -         ||   -      |   -    |    -
	4     |    1           |     0        |       0         ||   -      |   -    |    -
	5     |   >=2          |     0        |       -         ||   -      |   Y    |    -
	6     |   >=2          |     0        |       0         ||   -      |   Y    |    -

	7     |    -           |     N        |       N         ||   -      |   -    |    Y
	8     |    -           |     N        |      N-1        ||   -      |   -    |    -
	9     |    -           |     N        |       -         ||   -      |   -    |    -

   10     |   N+1          |     N        |       N         ||   Y      |   -    |    Y
   11     |   N+1          |     N        |      N-1        ||   Y      |   -    |    -
   12     |   N+1          |     N        |       -         ||   Y      |   -    |    - 
	
   13     |   N+2          |     N        |       N         ||   -      |   -    |    Y
   14     |   N+2          |     N        |      N-1        ||   -      |   -    |    -
   15     |   N+2          |     N        |       -         ||   -      |   -    |    - 
	*/

	__int64 lowerPrimeTally;
	__int64 thisPrimeTally;
	__int64 higherPrimeTally;

	// Before we increment primeList<index>, we want to update <pdcList>.
	// This should be done before incrementing the tally.

	bool thereIsALowerPrime = (bool)(indexOfThisPrime > 0);
	bool thereIsAHigherPrime = (bool)(indexOfThisPrime < (__int64)primeList.size() - 1);

	lowerPrimeTally = thereIsALowerPrime ? primeList[indexOfThisPrime - 1]._tally : -1;
	thisPrimeTally = primeList[indexOfThisPrime]._tally;
	higherPrimeTally = thereIsAHigherPrime ? primeList[indexOfThisPrime + 1]._tally : -1;

	if (pdcList.size() == 0)
	{
		AddToPdc(primeList[indexOfThisPrime]._prime);
	}
	else
	{
		if (thisPrimeTally == 0)
		{
			if (lowerPrimeTally != 1)
			{
				AddToPdc(primeList[indexOfThisPrime]._prime);
				return;
			}
		}

		if (lowerPrimeTally - thisPrimeTally == 1)
		{
			// The prime has moved up to be a non-PDC in a new group
			RemoveFromPdc(primeList[indexOfThisPrime]._prime);
		}

		if ((thisPrimeTally - higherPrimeTally == 0) && (higherPrimeTally > 0))
		{
			// Since <this> moved out of the tally group, the higher prime now becomes a PDC
			AddToPdc(primeList[indexOfThisPrime + 1]._prime);
		}
	}
}

__int64 QueueLineConjecture::ReturnIndexInPrimeList(__int64 p)
{
	// Perform binary search over <primeList>, return index of entry matching the value <p>
	// Source: http://www.pracspedia.com/AOA/binarysearch.html
	__int64 mid;
	__int64 low = 0;
	__int64 high = primeList.size() - 1;

	while (low <= high)
	{
		mid = (low + high) / 2;
		if (primeList[mid]._prime > p)
			high = mid - 1;
		else if (primeList[mid]._prime < p)
			low = mid + 1;
		else
			return mid; //found
	}
	return -1; //not found
}

__int64 QueueLineConjecture::ReturnPrimeTally(__int64 p)
{
	__int64 index = ReturnIndexInPrimeList(p);
	if (index < 0)
	{
		// Prime not found
		return 0;
	}
	else
	{
		return primeList[index]._tally;
	}
}

void QueueLineConjecture::SetToExitLoop()
{
	exitLoop = true;
}
