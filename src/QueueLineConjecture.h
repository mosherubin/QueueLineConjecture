#ifndef QueueLineConjecture_h
#define QueueLineConjecture_h

#include <vector>

struct PrimeListElement
{
	__int64 _prime;
	__int64 _tally;
};

class QueueLineConjecture
{
public:
	QueueLineConjecture();
	~QueueLineConjecture();
	
	void DoIt(__int64 n);

	std::string ReturnTimestamp();
	void SetTrace(bool enable);
	void SetUniqueFactors(bool useUniqueFactors);
	void SetUsePdcPrimes(bool usePdcPrimes);
	void SetToExitLoop();

private:
	void InitializePrimeList();
	void InitializePdcList();
	void HandleNewPrime(__int64 prime);
	void HandleOddComposite(__int64 composite, std::vector<__int64> &factor);
	std::vector<__int64> &QueueLineConjecture::FactorNumber(__int64 number, std::vector<__int64> &factor);
	void AddToPdc(__int64 prime);
	void RemoveFromPdc(__int64 prime);
	void UpdatePdcList(__int64 index);
	__int64 ReturnIndexInPrimeList(__int64 p);
	__int64 ReturnPrimeTally(__int64 p);

	void DumpPrimeList(FILE *f, char *desc);
	void DumpPdcList(FILE *f, char *desc);
	void DumpFactors(__int64 i, std::vector<__int64> &factor);
	void DumpLists(FILE *f, const char *s, __int64 i);

	std::vector<PrimeListElement> primeList;
	std::vector<__int64> pdcList;
	bool traceEnabled;
	bool useQlc;
	bool returnUniqueFactors;
	bool usePdcPrimes;
	bool exitLoop;

	// The following 'friend' declaration allows unit testing the internal functions and data of <QueueLineConjecture>
	friend class QueueLineConjecture_Test;
};

#endif
