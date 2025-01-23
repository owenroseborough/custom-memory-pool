#include "pch.h"
#include "CppUnitTest.h"
#include "../custom-memory-pool/custom-memory-pool.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace custommemorypoolunittests
{
	TEST_CLASS(custommemorypoolunittests)
	{
	public:
		TEST_METHOD(AllocateTenIntegers)
		{
			MemoryPool<int> pool((size_t)10, "CacheLineAlignment");

			Assert::AreEqual(pool.getCacheLinesPerObject(), (size_t)0);
			Assert::AreEqual(pool.getLengthFreeList(), (size_t)10);
			Assert::AreEqual(pool.getNumItemsPerCacheLine(), (size_t)16);
		}
		TEST_METHOD(AllocateTenStrings)
		{
			MemoryPool<string> pool((size_t)10, "CacheLineAlignment");

			Assert::AreEqual(pool.getCacheLinesPerObject(), (size_t)0);
			Assert::AreEqual(pool.getLengthFreeList(), (size_t)10);
			Assert::AreEqual(pool.getNumItemsPerCacheLine(), (size_t)1);
		}
		TEST_METHOD(AllocateTenDoubles)
		{
			MemoryPool<double> pool((size_t)10, "CacheLineAlignment");

			Assert::AreEqual(pool.getCacheLinesPerObject(), (size_t)0);
			Assert::AreEqual(pool.getLengthFreeList(), (size_t)10);
			Assert::AreEqual(pool.getNumItemsPerCacheLine(), (size_t)1);
		}
	};
}
