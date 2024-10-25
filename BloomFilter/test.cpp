#include "pch.h"
#include "bloom_filter.cpp"

namespace bloomfilter {
	TEST(HashIsConsistent, BloomFilter) {
		BloomFilter* filter = new BloomFilter(100, 33);
		ASSERT_EQ(filter->BloomHash("hello", 5), filter->BloomHash("hello", 5));
	}

	TEST(MatchesKeys, BloomFilter) {
		BloomFilter* filter = new BloomFilter(100, 33);
		filter->Add("hello");
		filter->Add("world");
		ASSERT_TRUE(filter->KeyMayMatch("hello"));
		ASSERT_TRUE(filter->KeyMayMatch("world"));
		ASSERT_TRUE(!filter->KeyMayMatch("x"));
		ASSERT_TRUE(!filter->KeyMayMatch("foo"));
	}
}