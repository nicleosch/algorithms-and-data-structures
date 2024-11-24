//---------------------------------------------------------------------------
// #######
// # AnD #
// #######
//---------------------------------------------------------------------------
// This file contains my implementation of a lock-free tagged chaining
// hash table described in https://dl.acm.org/doi/10.1145/2588555.2610507.
// This hash table is designed for hash joins in databases. Thus, there are no
// deletions and the hash table is built once completely and then only probed.
// The concept described in the paper, and thus my code, makes use of this.
//---------------------------------------------------------------------------
#ifndef TAGGED_HASH_TABLE_H_
#define TAGGED_HASH_TABLE_H_
//---------------------------------------------------------------------------
#include <cstdint>
#include <vector>
#include <atomic>
#include "utils.h"
//---------------------------------------------------------------------------
namespace data_structures::tagged_hash_table {
//---------------------------------------------------------------------------
template<typename ValueT>
class HashTable {
    public:
    struct Entry {
        /// The key into the hash table
        uint64_t key;
        /// The value stored in this entry
        ValueT value;
        /// The next entry in the chain
        Entry* next = nullptr;

        Entry(uint64_t key, ValueT value) : key(key), value(value) {}
    };
    class BucketIterator {
        using iterator_category = std::forward_iterator_tag;

        public:
        /// Default constructor
        BucketIterator() : current(nullptr) {}
        /// Constructor
        explicit BucketIterator(Entry* current) : current(current) {}
        /// Dereference operator
        Entry& operator*() { return *current; }
        /// Member access operator
        Entry* operator->() { return current; }
        /// Pre-increment operator
        BucketIterator operator++() {
            auto tmp = *this;
            if(current != nullptr) {
                current = current->next;
            }
            return tmp;
        }
        /// Post-increment operator
        BucketIterator& operator++(int) {
            if(current != nullptr)
                current = current->next;
            return *this;
        }
        /// Equality operator
        bool operator==(const BucketIterator& other) const { return current == other.current; }
        /// Inquality operator
        bool operator!=(const BucketIterator& other) const { return current != other.current; }

        private:
        Entry* current;
    };
    /// Constructor 
    explicit HashTable(uint64_t size) {
        uint64_t ht_size = next_power_of_2(size);
        ht_mask = ht_size - 1;
        table = std::vector<std::atomic<Entry*>>(ht_size);
    }
    /// Insert an entry into the hash table.
    void insert(Entry* entry) {
        uint64_t hash = mm_hash(entry->key);
        uint64_t slot = hash & ht_mask;
        Entry* old_entry;
        Entry* new_entry;
        do {
            old_entry = table[slot];
            entry->next = untag(old_entry);
            new_entry = reinterpret_cast<Entry*>(
                reinterpret_cast<uintptr_t>(entry) |
                (reinterpret_cast<uintptr_t>(old_entry) & tag_mask) |
                tag(hash)
            );
        } while(!std::atomic_compare_exchange_weak(&table[slot], &old_entry, new_entry));
    }
    /// Lookup a key in the hash table.
    BucketIterator lookup(uint64_t key) const {
        uint64_t hash = mm_hash(key);
        uint64_t bucket = hash & ht_mask;

        // Use tag for early filtering
        uint64_t bucket_tag = reinterpret_cast<uintptr_t>(table[bucket].load()) & tag_mask;
        if(uint64_t key_tag = tag(hash); key_tag != (key_tag & bucket_tag))
            return BucketIterator();

        return BucketIterator(untag(table[bucket]));
    }
    /// Get the size of the hash table.
    size_t size() const { return table.size(); }
    /// Get the end of the hash table.
    BucketIterator end() { return BucketIterator(); }

    private:
    /// Determine the tag for a given hash.
    uint64_t tag(uint64_t hash) const {
        return hash &= tag_mask;
    }
    /// Untag a given entry pointer.
    Entry* untag (Entry* entry) const {
        auto ptr = reinterpret_cast<uintptr_t>(entry);
        ptr &= ~tag_mask;
        return reinterpret_cast<Entry*>(ptr);
    }
    /// The mask for tagging the pointers
    uint64_t tag_mask = 0xFFFF000000000000;
    /// The mask for efficient modulo
    uint64_t ht_mask;
    /// The hash table interface
    std::vector<std::atomic<Entry*>> table;
};
//---------------------------------------------------------------------------
}
//---------------------------------------------------------------------------
#endif // TAGGED_HASH_TABLE_H_
//---------------------------------------------------------------------------