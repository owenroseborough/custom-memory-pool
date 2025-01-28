#include <array>
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
	TEST_CLASS(AllocateInitialBlockTestsNoCacheLineAlignment)
	{
	public:
		TEST_METHOD(AllocateTenIntegers)
		{
			MemoryPool<int> pool((size_t)10, "NoAlignment");
			Assert::AreEqual(pool.getLengthFreeList(), (size_t)10);
			Assert::AreEqual(pool.getPoolByteSize(), sizeof(int) * (size_t)10);
		}
		TEST_METHOD(AllocateTenStrings)
		{
			MemoryPool<string> pool((size_t)10, "NoAlignment");
			Assert::AreEqual(pool.getLengthFreeList(), (size_t)10);
			Assert::AreEqual(pool.getPoolByteSize(), sizeof(string) * (size_t)10);
		}
		TEST_METHOD(AllocateTenDoubles)
		{
			MemoryPool<double> pool((size_t)10, "NoAlignment");
			Assert::AreEqual(pool.getLengthFreeList(), (size_t)10);
			Assert::AreEqual(pool.getPoolByteSize(), sizeof(double) * (size_t)10);
		}
		TEST_METHOD(AllocateTenLongs)
		{
			MemoryPool<long long int> pool((size_t)10, "NoAlignment");
			Assert::AreEqual(pool.getLengthFreeList(), (size_t)10);
			Assert::AreEqual(pool.getPoolByteSize(), sizeof(long long int) * (size_t)10);
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
			MemoryPool<equal56Bytes> pool((size_t)10, "NoAlignment");
			Assert::AreEqual(pool.getLengthFreeList(), (size_t)10);
			Assert::AreEqual(pool.getPoolByteSize(), sizeof(equal56Bytes)* (size_t)10);
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
			MemoryPool<equal64Bytes> pool((size_t)10, "NoAlignment");
			Assert::AreEqual(pool.getLengthFreeList(), (size_t)10);
			Assert::AreEqual(pool.getPoolByteSize(), sizeof(equal64Bytes)* (size_t)10);
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
			MemoryPool<equal72Bytes> pool((size_t)10, "NoAlignment");
			Assert::AreEqual(pool.getLengthFreeList(), (size_t)10);
			Assert::AreEqual(pool.getPoolByteSize(), sizeof(equal72Bytes)* (size_t)10);
		}
		TEST_METHOD(AllocateTenStructs160BytesEach)
		{
			struct equal160Bytes {
				string first;
				string second;
				string third;
				string fourth;
			};
			MemoryPool<equal160Bytes> pool((size_t)10, "NoAlignment");
			Assert::AreEqual(pool.getLengthFreeList(), (size_t)10);
			Assert::AreEqual(pool.getPoolByteSize(), sizeof(equal160Bytes)* (size_t)10);
		}
	};
	TEST_CLASS(AllocateAndDeallocateChunksFromMemoryPool)
	{
	public:
		TEST_METHOD(AllocateAndDeallocateSeventeenIntegers)
		{
			vector<shared_ptr<int*>> seventeenAllocations;
			MemoryPool<int> pool((size_t)17, "CacheLineAlignment");
			for (size_t i = 0; i < 17; i++) {
				seventeenAllocations.push_back(pool.allocate());
			}
			size_t counter = 0;
			for (auto intP : seventeenAllocations) {
				int* testPtr;
				Assert::AreEqual(typeid(testPtr).name(), typeid(*intP.get()).name());
				if (counter < 16) {
					Assert::IsTrue(*seventeenAllocations[counter].get() > *seventeenAllocations[counter + 1].get());
				}
				counter++;
			}
			size_t freeListSize = pool.getLengthFreeList();
			for (auto pointer : seventeenAllocations) {
				pool.deallocate(*pointer.get());
				Assert::IsTrue(pool.getLengthFreeList() == freeListSize + 1);
				freeListSize += 1;
			}
		}
		TEST_METHOD(AllocateAndDeallocateSeventeenStrings)
		{
			vector<shared_ptr<string*>> seventeenAllocations;
			MemoryPool<string> pool((size_t)17, "CacheLineAlignment");
			for (size_t i = 0; i < 17; i++) {
				seventeenAllocations.push_back(pool.allocate());
			}
			size_t counter = 0;
			for (auto stringP : seventeenAllocations) {
				string* testPtr;
				Assert::AreEqual(typeid(testPtr).name(), typeid(*stringP.get()).name());
				if (counter < 16) {
					Assert::IsTrue(*seventeenAllocations[counter].get() > *seventeenAllocations[counter + 1].get());
				}
				counter++;
			}
			size_t freeListSize = pool.getLengthFreeList();
			for (auto pointer : seventeenAllocations) {
				pool.deallocate(*pointer.get());
				Assert::IsTrue(pool.getLengthFreeList() == freeListSize + 1);
				freeListSize += 1;
			}
		}
		TEST_METHOD(AllocateAndDeallocateSeventeenStructs160BytesEach)
		{
			struct equal160Bytes {
				string first;
				string second;
				string third;
				string fourth;
			};
			vector<shared_ptr<equal160Bytes*>> seventeenAllocations;
			MemoryPool<equal160Bytes> pool((size_t)17, "CacheLineAlignment");
			for (size_t i = 0; i < 17; i++) {
				seventeenAllocations.push_back(pool.allocate());
			}
			size_t counter = 0;
			for (auto equal160BytesP : seventeenAllocations) {
				equal160Bytes* testPtr;
				Assert::AreEqual(typeid(testPtr).name(), typeid(*equal160BytesP.get()).name());
				if (counter < 16) {
					Assert::IsTrue(*seventeenAllocations[counter].get() > *seventeenAllocations[counter + 1].get());
				}
				counter++;
			}
			size_t freeListSize = pool.getLengthFreeList();
			for (auto pointer : seventeenAllocations) {
				pool.deallocate(*pointer.get());
				Assert::IsTrue(pool.getLengthFreeList() == freeListSize + 1);
				freeListSize += 1;
			}
		}
	};
}
