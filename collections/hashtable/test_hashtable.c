#define TEST_FILE_NAME_ONLY

#include "testing/testing.h"

#include "hashtable.h"

pair_declare(int, int);

size_t int_hash(const void *p) {
    // https://stackoverflow.com/a/12996028/17654649
    int x = *(int *) p;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}

int int_compare(const void *p1, const void *p2) {
    int *i1 = (int *) p1, *i2 = (int *) p2;
    return *i1 - *i2;
}

test_fact(hashtable_is_created_empty) {
    Pair(int, int) *ht = hashtable_of_type(typeof(*ht), int_hash, int_compare);

    test_assert(hashtable_empty(ht), "expected created hashtable to be empty");

    hashtable_free(ht);
}

test_fact(hashtbale_is_created_with_default_capacity) {
    Pair(int, int) *ht = hashtable_of_type(typeof(*ht), int_hash, int_compare);

    test_assert(
            HASHTABLE_DEFAULT_CAPACITY == hashtable_capacity(ht),
            "expected capacity to be equal to %s=%zu",
            test_nameof(HASHTABLE_DEFAULT_CAPACITY), (size_t) HASHTABLE_DEFAULT_CAPACITY
    );

    hashtable_free(ht);
}

test_fact(hashtbale_is_created_with_given_capacity) {
    const size_t expected_capacity = 42;
    Pair(int, int) *ht = hashtable_new(
            int_hash,
            int_compare,
            sizeof(ht->key), offsetof(typeof(*ht), key),
            sizeof(ht->value), offsetof(typeof(*ht), value),
            sizeof(*ht),
            expected_capacity
    );

    test_assert(
            expected_capacity == hashtable_capacity(ht),
            "expected capacity to be equal to %zu", expected_capacity
    );

    hashtable_free(ht);
}

test_fact(hashtable_put_inserts_elements) {
    const int key = 42;
    const int value = 10;
    Pair(int, int) *ht = hashtable_of_type(typeof(*ht), int_hash, int_compare);

    const int *const created_value = hashtable_put(ht, key, value);

    test_assert(false == hashtable_empty(ht), "expected hashtable to not be empty");
    test_assert(value == *created_value, "expected *%s to be %d", test_nameof(created_value), value);
    test_assert(value == hashtable_get(ht, key), "expected value at %d to be %d", key, value);

    hashtable_free(ht);
}

test_fact(hashtable_with_entry_inserts_elements) {
    const int key = 42;
    const int value = 10;
    Pair(int, int) *ht = hashtable_of_type(typeof(*ht), int_hash, int_compare);

    ht = hashtable_with_entry(ht, &key, &value, NULL);

    test_assert(false == hashtable_empty(ht), "expected hashtable to not be empty");
    test_assert(value == hashtable_get(ht, key), "expected value at %d to be %d", key, value);

    hashtable_free(ht);
}

test_fact(hashtable_with_entry_sets_pointer_to_inserted_value) {
    const int key = 42;
    const int value = 10;
    Pair(int, int) *ht = hashtable_of_type(typeof(*ht), int_hash, int_compare);

    void *inserted_value;
    ht = hashtable_with_entry(ht, &key, &value, &inserted_value);

    test_assert(false == hashtable_empty(ht), "expected hashtable to not be empty");
    test_assert(value == *(int *) inserted_value, "expected inserted value to be %d", value);

    hashtable_free(ht);
}

test_fact(hashtable_at_returns_pointer_to_mutable_value) {
    const int key = 42;
    const int value = 10;
    const int new_value = 20;
    Pair(int, int) *ht = hashtable_of_type(typeof(*ht), int_hash, int_compare);
    hashtable_put(ht, key, value);

    int *const p_value = hashtable_at(ht, key);
    *p_value = new_value;

    test_assert(new_value == hashtable_get(ht, key), "expected value at %d to be %d", key, value);

    hashtable_free(ht);
}

test_fact(hashtable_at_returns_null_if_value_does_not_exist) {
    const int key = 42;
    Pair(int, int) *ht = hashtable_of_type(typeof(*ht), int_hash, int_compare);

    int *const p_value = hashtable_at(ht, key);

    test_assert(NULL == p_value, "expected pointer to value to be NULL");

    hashtable_free(ht);
}

//size_t const_hash(const void *p) {
//    (void) p;
//    return 0;
//}
//
//test_fact(hashtable_handles_hash_collisions) {
//    Pair(int, int) *ht = hashtable_of_type(typeof(*ht), int_hash, int_compare);
//
//    // TODO insert many values
//
//    // TODO check size
//    // TODO check that all values are present
//
//    hashtable_free(ht);
//}

//test_fact(hashtable_foreach_iterates_over_all_elements) {
//    // TODO iterate over mutable hash table
//    // TODO iterate over const hash table
//}

test_all_tests = {
        test_case(hashtable_is_created_empty),
        test_case(hashtbale_is_created_with_default_capacity),
        test_case(hashtbale_is_created_with_given_capacity),
        test_case(hashtable_put_inserts_elements),
        test_case(hashtable_with_entry_inserts_elements),
        test_case(hashtable_with_entry_sets_pointer_to_inserted_value),
        test_case(hashtable_at_returns_pointer_to_mutable_value),
        test_case(hashtable_at_returns_null_if_value_does_not_exist),
};

test_run_all_tests()
