#include "gtest/gtest.h"
#include <memory>
#include <iostream>
#include "../HashTable.h"

class HashTableFixture : public ::testing::Test {
protected:
    virtual void SetUp() {
        this->emptyHashTable = std::make_shared<HashTable>();
        this->hashTable1 = std::make_shared<HashTable>();
        this->hashTable2 = std::make_shared<HashTable>();
        this->hashTable3 = std::make_shared<HashTable>();
        this->hashTable4 = std::make_shared<HashTable>();
        this->hashTable5 = std::make_shared<HashTable>();
        this->hashTable6 = std::make_shared<HashTable>();
    }

    virtual void TearDown() {}

    std::shared_ptr<HashTable> emptyHashTable;
    std::shared_ptr<HashTable> hashTable1;
    std::shared_ptr<HashTable> hashTable2;
    std::shared_ptr<HashTable> hashTable3;
    std::shared_ptr<HashTable> hashTable4;
    std::shared_ptr<HashTable> hashTable5;
    std::shared_ptr<HashTable> hashTable6;
};

TEST_F(HashTableFixture, dummyTest) {
    ASSERT_EQ(hashTable1->size(), 0);
}

TEST_F(HashTableFixture, testInsert) {
    hashTable1->clear();
    hashTable1->insert("Key 1", Value());
    hashTable1->insert("Key 2", Value());
    hashTable1->insert("Key 3", Value());
    ASSERT_EQ(hashTable1->size(), 3) << "Test size after inserting";
}

TEST_F(HashTableFixture, testDeepCopy) {
    hashTable1->insert("Key 1", Value());
    hashTable1->insert("Key 2", Value());

    *hashTable2 = *hashTable1;

    hashTable1->clear();

    ASSERT_TRUE(hashTable1->empty()) <<"Test for deep copy";
    ASSERT_FALSE(hashTable2->empty());
}

TEST_F(HashTableFixture, testContain){
    hashTable1->insert("Key 1", Value());
    ASSERT_TRUE(hashTable1->contains("Key 1"))<<"Check for erase function";
}



TEST_F(HashTableFixture, testErase){
    hashTable1->insert("Key 1", Value());
    hashTable1->insert("Key 2", Value());
    hashTable1->insert("Key 3", Value());
    hashTable1->erase("Key 1");
    ASSERT_FALSE(hashTable1->contains("Hello"))<<"Check for erase function";


}

TEST_F(HashTableFixture, testAts){
    hashTable1->insert("Key 1", Value());
    hashTable1->at("Key 1");
    ASSERT_TRUE(hashTable1->contains("Key 1"));
}
TEST_F(HashTableFixture, testOperators){
    Value value = Value();
    value.age=18;
    value.weight=80;
    hashTable1->insert("Key 1", value);
    ASSERT_EQ((*hashTable1)["Key 1"], value);
    ASSERT_EQ((*hashTable1)["emptyKey"], Value());
}
TEST_F(HashTableFixture, testOperatorEquality){
    Value value = Value();
    value.age=18;
    value.weight=80;
    hashTable1->insert("Key1", value);
    hashTable2->insert("Key1", value);
    ASSERT_TRUE(*hashTable1 == *hashTable2);
}
TEST_F(HashTableFixture, testSwap){
    Value value1 = Value();
    Value value2 = Value();
    value2.age=60;
    value2.weight=40;
    value1.age=18;
    value1.weight=80;
    hashTable1->insert("Key1", value1);
    hashTable2->insert("Key2", value2);
    hashTable1->swap(*hashTable2);
    ASSERT_TRUE(hashTable1->contains("Key2"));
    ASSERT_FALSE(hashTable1->contains("Key1"));
}
TEST_F(HashTableFixture, testClear){
    Value value1 = Value();
    Value value2 = Value();
    value2.age=60;
    value2.weight=40;
    value1.age=18;
    value1.weight=80;
    hashTable1->insert("Key1", value1);
    hashTable1->insert("Key2", value2);
    hashTable1->clear();
    ASSERT_TRUE(hashTable1);
}
