#include "../src/Utilities/LRUCache.h"
#include <catch.hpp>
#include <thread>
#include <vector>

using namespace Utilities;

TEST_CASE("LRUCache Basic Operations", "[LRUCache]") {
    auto onCacheMiss = [](const int& key) { return key * 2; };
    LRUCache<int, int> cache(onCacheMiss, 3);

    SECTION("Cache Miss") {
        REQUIRE(cache(1) == 2);
        REQUIRE(cache(2) == 4);
        REQUIRE(cache(3) == 6);
    }

    SECTION("Cache Hit") {
        cache(1);
        cache(2);
        cache(3);
        REQUIRE(cache(1) == 2);
        REQUIRE(cache(2) == 4);
        REQUIRE(cache(3) == 6);
    }

    SECTION("Cache Eviction") {
        cache(1);
        cache(2);
        cache(3);
        cache(4); // This should evict key 1
        REQUIRE(cache(1) == 2); // Cache miss, reinsert key 1
        REQUIRE(cache(2) == 4);
        REQUIRE(cache(3) == 6);
        REQUIRE(cache(4) == 8);
    }

    SECTION("Update Item") {
        cache(1);
        cache.UpdateItem(1, 10);
        REQUIRE(cache(1) == 10);
    }

    SECTION("Purge Item") {
        cache(1);
        cache.PurgeItem(1);
        REQUIRE(cache(1) == 2); // Cache miss, reinsert key 1
    }
}

TEST_CASE("LRUCache Thread Safety", "[LRUCache][ThreadSafety]") {
    auto onCacheMiss = [](const int& key) { return key * 2; };
    LRUCache<int, int> cache(onCacheMiss, 3);

    auto cacheAccess = [&cache](int key) {
        for (int i = 0; i < 100; ++i) {
            cache(key);
        }
        };

    std::vector<std::thread> threads;
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back(cacheAccess, i);
    }

    for (auto& thread : threads) {
        thread.join();
    }

    for (int i = 0; i < 10; ++i) {
        REQUIRE(cache(i) == i * 2);
    }
}