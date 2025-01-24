#include "pch.h"
#include "CppUnitTest.h"
#include "../custom-memory-pool/custom-memory-pool.h"
#include "../custom-memory-pool/get-cache-line-size.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace custommemorypoolunittests
{
	TEST_CLASS(AllocateInitialBlockTestsWithCacheLineAlignment)
	{
	public:
		TEST_METHOD(AllocateTenIntegers)
		{
			MemoryPool<int> pool((size_t)10, "CacheLineAlignment");

			Assert::AreEqual(pool.getCacheLinesPerObject(), (size_t)0);
			Assert::AreEqual(pool.getLengthFreeList(), (size_t)10);
			Assert::AreEqual(pool.getNumItemsPerCacheLine(), (size_t)16);
			Assert::AreEqual(pool.getPoolByteSize(), static_cast<size_t>(ceil(static_cast<double>(10) / (getCacheLineSize() / sizeof(int))) * getCacheLineSize()));
		}
		TEST_METHOD(AllocateTenStrings)
		{
			MemoryPool<string> pool((size_t)10, "CacheLineAlignment");

			Assert::AreEqual(pool.getCacheLinesPerObject(), (size_t)0);
			Assert::AreEqual(pool.getLengthFreeList(), (size_t)10);
			Assert::AreEqual(pool.getNumItemsPerCacheLine(), (size_t)1);
			Assert::AreEqual(pool.getPoolByteSize(), static_cast<size_t>(ceil(static_cast<double>(10) / (getCacheLineSize() / sizeof(string))) * getCacheLineSize()));
		}
		TEST_METHOD(AllocateTenDoubles)
		{
			MemoryPool<double> pool((size_t)10, "CacheLineAlignment");

			Assert::AreEqual(pool.getCacheLinesPerObject(), (size_t)0);
			Assert::AreEqual(pool.getLengthFreeList(), (size_t)10);
			Assert::AreEqual(pool.getNumItemsPerCacheLine(), (size_t)8);
			Assert::AreEqual(pool.getPoolByteSize(), static_cast<size_t>(ceil(static_cast<double>(10) / (getCacheLineSize() / sizeof(double))) * getCacheLineSize()));
		}
		TEST_METHOD(AllocateTenLongs)
		{
			MemoryPool<long long int> pool((size_t)10, "CacheLineAlignment");

			Assert::AreEqual(pool.getCacheLinesPerObject(), (size_t)0);
			Assert::AreEqual(pool.getLengthFreeList(), (size_t)10);
			Assert::AreEqual(pool.getNumItemsPerCacheLine(), (size_t)8);
			Assert::AreEqual(pool.getPoolByteSize(), static_cast<size_t>(ceil(static_cast<double>(10) / (getCacheLineSize() / sizeof(long long int))) * getCacheLineSize()));
		}
		TEST_METHOD(AllocateTenStructs56BytesEach)
		{
			struct equal56Bytes {
				int first;
				char second;
				string third;
				char fourth;
				int fifth;
			};
			MemoryPool<equal56Bytes> pool((size_t)10, "CacheLineAlignment");

			Assert::AreEqual(pool.getCacheLinesPerObject(), (size_t)0);
			Assert::AreEqual(pool.getLengthFreeList(), (size_t)10);
			Assert::AreEqual(pool.getNumItemsPerCacheLine(), (size_t)1);
			Assert::AreEqual(pool.getPoolByteSize(), static_cast<size_t>(ceil(static_cast<double>(sizeof(equal56Bytes)) / getCacheLineSize())* getCacheLineSize() * 10));
		}
		TEST_METHOD(AllocateTenStructs64BytesEach)
		{
			struct equal64Bytes {
				int first;
				char second;
				string third;
				char fourth;
				int fifth;
				long long int sixth;
			};
			MemoryPool<equal64Bytes> pool((size_t)10, "CacheLineAlignment");

			Assert::AreEqual(pool.getCacheLinesPerObject(), (size_t)1);
			Assert::AreEqual(pool.getLengthFreeList(), (size_t)10);
			Assert::AreEqual(pool.getNumItemsPerCacheLine(), (size_t)1);
			Assert::AreEqual(pool.getPoolByteSize(), static_cast<size_t>(ceil(static_cast<double>(sizeof(equal64Bytes)) / getCacheLineSize())* getCacheLineSize() * 10));
		}
		TEST_METHOD(AllocateTenStructs72BytesEach)
		{
			struct equal72Bytes {
				int first;
				char second;
				string third;
				char fourth;
				int fifth;
				long long int sixth;
				char seventh;
			};
			MemoryPool<equal72Bytes> pool((size_t)10, "CacheLineAlignment");

			Assert::AreEqual(pool.getCacheLinesPerObject(), (size_t)2);
			Assert::AreEqual(pool.getLengthFreeList(), (size_t)10);
			Assert::AreEqual(pool.getNumItemsPerCacheLine(), (size_t)1);
			Assert::AreEqual(pool.getPoolByteSize(), static_cast<size_t>(ceil(static_cast<double>(sizeof(equal72Bytes)) / getCacheLineSize())* getCacheLineSize() * 10));
		}
		TEST_METHOD(AllocateTenStructs160BytesEach)
		{
			struct equal160Bytes {
				string first;
				string second;
				string third;
				string fourth;
			};
			MemoryPool<equal160Bytes> pool((size_t)10, "CacheLineAlignment");

			Assert::AreEqual(pool.getCacheLinesPerObject(), (size_t)3);
			Assert::AreEqual(pool.getLengthFreeList(), (size_t)10);
			Assert::AreEqual(pool.getNumItemsPerCacheLine(), (size_t)1);
			Assert::AreEqual(pool.getPoolByteSize(), static_cast<size_t>(ceil(static_cast<double>(sizeof(equal160Bytes)) / getCacheLineSize()) * getCacheLineSize() * 10));
		}
	};
	TEST_CLASS(AllocateInitialBlockTests)
	{
	public:
		TEST_METHOD(AllocateTenIntegers)
		{
			MemoryPool<int> pool((size_t)10, "CacheLineAlignment");

			Assert::AreEqual(pool.getCacheLinesPerObject(), (size_t)0);
			Assert::AreEqual(pool.getLengthFreeList(), (size_t)10);
			Assert::AreEqual(pool.getNumItemsPerCacheLine(), (size_t)16);
			Assert::AreEqual(pool.getPoolByteSize(), static_cast<size_t>(ceil(static_cast<double>(10) / (getCacheLineSize() / sizeof(int))) * getCacheLineSize()));
		}
	};
}
