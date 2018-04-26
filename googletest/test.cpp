#include "pch.h"
#include "../src/QueueLineConjecture.h"
#include <algorithm>

class MatchesPrimeValue
{
	__int64 value;

public:
	MatchesPrimeValue(const __int64 &v) : value(v) {}

	bool operator()(const PrimeListElement &ple) const
	{
		return ple._prime == value;
	}
};

struct ExpectedPrimeTally
{
	__int64 prime;
	__int64 tally;
};

class QueueLineConjecture_Test : public QueueLineConjecture
{
public:
	QueueLineConjecture_Test() {}
	~QueueLineConjecture_Test() {};

	std::vector<__int64> &TestFactorNumber(__int64 number, std::vector<__int64> &factor)
	{
		return FactorNumber(number, factor);
	}

	bool IsPrime(__int64 n)
	{
		for (__int64 i = 3; i * i <= n; i += 2)
		{
			if (((n / i) * i) == n)
				return false;
		}

		return true;
	}

	bool PrimeListContainsOnlyPrimes()
	{
		for (int i = 0; i < this->primeList.size(); i++)
		{
			if (!IsPrime(primeList[i]._prime))
				return false;
		}

		return true;
	}

	bool ModifyPrimeListEntry(int index, __int64 newValue)
	{
		if (index >= primeList.size())
			return false;
		primeList[index]._prime = newValue;
		return true;
	}

	bool AllExpectedPrimesFoundInPrimeList()
	{
		__int64 lastPrime = primeList.back()._prime;

		for (__int64 i = 3; i <= lastPrime; i+=2)
		{
			if (IsPrime(i))
			{
				if (std::find_if(primeList.begin(), primeList.end(), MatchesPrimeValue(i)) == primeList.end())
					return false;
			}
		}

		return true;
	}

	__int64 ReturnPrimeTally(__int64 prime)
	{
		std::vector<PrimeListElement>::iterator iter = std::find_if(primeList.begin(), primeList.end(), MatchesPrimeValue(prime));

		if (iter == primeList.end())
		{
			// Prime number not found
			return 0;
		}

		return iter->_tally;
	}
};

class TestFixture : public ::testing::Test
{
public:

	QueueLineConjecture_Test * tqlc;
	std::vector<__int64> actualFactors;
	__int64 num;

	virtual void SetUp()
	{
		tqlc = new QueueLineConjecture_Test();
		tqlc->SetTrace(false);
		num = 0;
	}
};

bool CompareSortedArrays(std::vector<__int64> a, std::vector<__int64> b)
{
	if (a.size() != b.size())
	{
		return false;
	}

	std::vector<__int64> a_sorted;
	std::vector<__int64> b_sorted;

	a_sorted = a;
	sort(a_sorted.begin(), a_sorted.end());

	b_sorted = b;
	sort(b_sorted.begin(), b_sorted.end());

	for (int i = 0; i < a.size(); i++)
	{
		if (a[i] != b[i])
			return false;
	}

	return true;
}

TEST_F(TestFixture, factor_27) {
	num = 3 * 3 * 3; /*27*/
	std::vector<__int64> expectedFactors = { 3, 3, 3 };

	tqlc->SetUniqueFactors(false);
	tqlc->DoIt(num - 2);
	tqlc->TestFactorNumber(num, actualFactors);
	ASSERT_TRUE(CompareSortedArrays(actualFactors, expectedFactors));
}

TEST_F(TestFixture, factor_671) {
	num = 11 * 61; /*671*/
	std::vector<__int64> expectedFactors = { 11, 61 };

	tqlc->SetUniqueFactors(false);
	tqlc->DoIt(num - 2);
	tqlc->TestFactorNumber(num, actualFactors);
	ASSERT_TRUE(CompareSortedArrays(actualFactors, expectedFactors));
}

TEST_F(TestFixture, doit_up_to_31) {
	ExpectedPrimeTally ExpectedTallies[] = { { 3, 7 },{ 5, 3 },{ 7, 1 } };

	tqlc->SetUniqueFactors(false);
	tqlc->DoIt(31);

	for (int i = 0; i < sizeof(ExpectedTallies) / sizeof(ExpectedPrimeTally); i++)
	{
		ASSERT_TRUE(tqlc->ReturnPrimeTally(ExpectedTallies[i].prime) == ExpectedTallies[i].tally);
	}

	ASSERT_TRUE(true);
}

TEST_F(TestFixture, doit_up_to_99_non_unique_factors) {
	ExpectedPrimeTally ExpectedTallies[] = { { 3, 25 },{ 5, 11 },{ 7, 7 },{ 11, 4 },{ 13, 3 },{ 17, 2 },{ 19, 2 },{ 23, 1 },{ 29, 1 },{ 31, 1 } };

	tqlc->SetUniqueFactors(false);
	tqlc->DoIt(99);

	for (int i = 0; i < sizeof(ExpectedTallies) / sizeof(ExpectedPrimeTally); i++)
	{
		ASSERT_TRUE(tqlc->ReturnPrimeTally(ExpectedTallies[i].prime) == ExpectedTallies[i].tally);
	}

	ASSERT_TRUE(true);
}

TEST_F(TestFixture, doit_up_to_99_unique_factors) {
	ExpectedPrimeTally ExpectedTallies[] = { { 3, 16 },{ 5, 9 },{ 7, 6 },{ 11, 4 },{ 13, 3 },{ 17, 2 },{ 19, 2 },{ 23, 1 },{ 29, 1 },{ 31, 1 } };

	tqlc->SetUniqueFactors(true);
	tqlc->DoIt(99);

	for (int i = 0; i < sizeof(ExpectedTallies) / sizeof(ExpectedPrimeTally); i++)
	{
		ASSERT_TRUE(tqlc->ReturnPrimeTally(ExpectedTallies[i].prime) == ExpectedTallies[i].tally);
	}

	ASSERT_TRUE(true);
}

TEST_F(TestFixture, doit_up_to_3009_non_unique_factors) {
	ExpectedPrimeTally ExpectedTallies[] = { { 3, 752 },{ 5, 374 },{ 7, 250 },{ 11, 149 },{ 13, 125 },
	                                         { 17, 93 },{ 19, 82 },{ 23, 67 },{ 29, 53 },{ 31, 50 }, 
	                                         {199,7}, {211,6}, {229,6}, {233,5} 
	                                       };

	tqlc->SetUniqueFactors(false);
	tqlc->DoIt(3009);

	for (int i = 0; i < sizeof(ExpectedTallies) / sizeof(ExpectedPrimeTally); i++)
	{
		ASSERT_TRUE(tqlc->ReturnPrimeTally(ExpectedTallies[i].prime) == ExpectedTallies[i].tally);
	}

	ASSERT_TRUE(true);
}

TEST_F(TestFixture, factor_nonsquare_composite_all_factors) {
	num = 3 * 5;
	std::vector<__int64> expectedFactors = {3, 5};

	tqlc->SetUniqueFactors(false);
	tqlc->DoIt(num - 2);
	tqlc->TestFactorNumber(num, actualFactors);
	ASSERT_TRUE(CompareSortedArrays(actualFactors, expectedFactors));
}

TEST_F(TestFixture, factor_nonsquare_composite_unique_factors) {
	num = 3 * 5;
	std::vector<__int64> expectedFactors = { 3, 5 };

	tqlc->SetUniqueFactors(true);
	tqlc->DoIt(num - 2);
	tqlc->TestFactorNumber(num, actualFactors);
	ASSERT_TRUE(CompareSortedArrays(actualFactors, expectedFactors));
}

TEST_F(TestFixture, factor_square_composite_all_factors) {
	num = 29 * 29;
	std::vector<__int64> expectedFactors = {29, 29 };

	tqlc->SetUniqueFactors(false);
	tqlc->DoIt(num - 2);
	tqlc->TestFactorNumber(num, actualFactors);
	ASSERT_TRUE(CompareSortedArrays(actualFactors, expectedFactors));
}

TEST_F(TestFixture, factor_square_composite_unique_factors) {
	num = 29 * 29;
	std::vector<__int64> expectedFactors = { 29 };

	tqlc->SetUniqueFactors(true);
	tqlc->DoIt(num - 2);
	tqlc->TestFactorNumber(num, actualFactors);
	ASSERT_TRUE(CompareSortedArrays(actualFactors, expectedFactors));
}

TEST_F(TestFixture, factor_nonsquare_composite_all_factors_2) {
	num = 3 * 5 * 5;
	std::vector<__int64> expectedFactors = { 3, 5, 5 };

	tqlc->SetUniqueFactors(false);
	tqlc->DoIt(num - 2);
	tqlc->TestFactorNumber(num, actualFactors);
	ASSERT_TRUE(CompareSortedArrays(actualFactors, expectedFactors));
}

TEST_F(TestFixture, factor_nonsquare_composite_unique_factors_2) {
	num = 3 * 5 * 5;
	std::vector<__int64> expectedFactors = { 3, 5 };

	tqlc->SetUniqueFactors(true);
	tqlc->DoIt(num - 2);
	tqlc->TestFactorNumber(num, actualFactors);
	ASSERT_TRUE(CompareSortedArrays(actualFactors, expectedFactors));
}

TEST_F(TestFixture, factor_nonsquare_composite_all_factors_3) {
	num = 3 * 29 * 29;
	std::vector<__int64> expectedFactors = { 3, 29, 29 };

	tqlc->SetUniqueFactors(false);
	tqlc->DoIt(num - 2);
	tqlc->TestFactorNumber(num, actualFactors);
	ASSERT_TRUE(CompareSortedArrays(actualFactors, expectedFactors));
}

TEST_F(TestFixture, factor_nonsquare_composite_unique_factors_3) {
	num = 3 * 29 * 29;
	std::vector<__int64> expectedFactors = { 3, 29 };

	tqlc->SetUniqueFactors(true);
	tqlc->DoIt(num - 2);
	tqlc->TestFactorNumber(num, actualFactors);
	ASSERT_TRUE(CompareSortedArrays(actualFactors, expectedFactors));
}

TEST_F(TestFixture, test_all_numbers_in_primelist_are_prime) {
	tqlc->DoIt(1001);
	ASSERT_TRUE(tqlc->PrimeListContainsOnlyPrimes());
}

TEST_F(TestFixture, force_nonprime_into_primelist) {
	tqlc->DoIt(1001);
	tqlc->ModifyPrimeListEntry(4, 12LL);
	ASSERT_FALSE(tqlc->PrimeListContainsOnlyPrimes ());
}

TEST_F(TestFixture, test_all_prime_numbers_are_found_in_primelist) {
	tqlc->DoIt(1001);
	ASSERT_TRUE(tqlc->AllExpectedPrimesFoundInPrimeList());
}
