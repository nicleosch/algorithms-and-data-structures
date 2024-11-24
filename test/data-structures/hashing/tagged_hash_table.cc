//---------------------------------------------------------------------------
// #######
// # AnD #
// #######
//---------------------------------------------------------------------------
#include <gtest/gtest.h>
#include <thread>
#include "hashing/tagged_hash_table.h"
#include "hashing/utils.h"
#include <iostream>
//---------------------------------------------------------------------------
using namespace data_structures::tagged_hash_table;
//---------------------------------------------------------------------------
TEST(HashTableTest, Size) {
    uint64_t size = 133;
    auto ht = HashTable<int>(size);
    EXPECT_EQ(ht.size(), next_power_of_2(size));
}
//---------------------------------------------------------------------------
TEST(HashTableTest, InsertOne) {
    auto entry = HashTable<int>::Entry{1, 5};
    auto ht = HashTable<int>(1);
    ht.insert(&entry);
    auto it = ht.lookup(1);
    EXPECT_EQ(it->key, 1);
    EXPECT_EQ(it->value, 5);
    EXPECT_EQ(it->next, nullptr);
}
//---------------------------------------------------------------------------
TEST(HashTableTest, InsertMany) {
    size_t size = 1000;
    std::vector<HashTable<int>::Entry> entries;
    for(size_t i = 0; i < size; ++i) {
        entries.emplace_back(i, i*2);
    }
    auto ht = HashTable<int>(entries.size());
    for(auto& entry : entries) {
        ht.insert(&entry);
    }
    for(size_t i = 0; i < size; ++i) {
        auto it = ht.lookup(i);
        for(; it != ht.end(); ++i) {
            if(it->key == i) {
                EXPECT_EQ(it->value, i*2);
                break;
            }
        }
        EXPECT_NE(it, ht.end());
    }
}
//---------------------------------------------------------------------------
TEST(MTHashTableTest, InsertMany) {
    size_t thread_count = std::thread::hardware_concurrency();
    size_t buffer_size = 1000;

    // Fill buffers
    std::vector<std::vector<HashTable<int>::Entry>> buffers;
    buffers.resize(thread_count);
    size_t id = 0;
    for(size_t i = 0; i < thread_count; ++i) {
        for(size_t j = 0; j < buffer_size; ++j) {
            buffers[i].emplace_back(id, id*2);
            ++id;
        }
    }

    auto ht = HashTable<int>(thread_count * buffer_size);

    // Assign each thread a buffer and insert
    std::vector<std::thread> threads;
    threads.reserve(thread_count);
    for(size_t i = 0; i < thread_count; ++i) {
        threads.emplace_back([&buffers, &ht, i]() {
            for(auto& entry : buffers[i]) {
                ht.insert(&entry);
            }
        });
    }
    for (auto& thread : threads) {
        thread.join();
    }

    // Lookup
    for(size_t i = 0; i < thread_count * buffer_size; ++i) {
        auto it = ht.lookup(i);
        for(; it != ht.end(); ++it) {
            if(it->key == i) {
                EXPECT_EQ(it->value, i*2);
                break;
            }
        }
        EXPECT_NE(it, ht.end());
    }
}