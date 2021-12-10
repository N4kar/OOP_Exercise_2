#ifndef NSU_OOP_2021_LABS_HASHTABLE_H
#define NSU_OOP_2021_LABS_HASHTABLE_H

#include <string>
#include "HashTableKeyNotFoundException.h"

// В этой задаче для простоты не требуется делать контейнер шаблонным,
// это допускается по желанию. Для нешаблонного решения,
// введем типы ключей и значений: ключом будет выступать строка (например, имя
// студента, значением - произвольная структура (например, численные
// характеристики студента.
struct Student {
    unsigned age;
    unsigned weight;

    bool operator <(const Student&b) const {
        if (this->age < b.age){
            return true;
        } else if (this->age == b.age) {
            return this->weight < b.weight;
        } else {
            return false;
        }
    }

    bool operator ==(const Student&b) const {
        return this->age == b.age && this->weight == b.weight;
    }

    bool operator !=(const Student&b) const {
        return !(*this == b);
    }
};

typedef std::string Key;
typedef Student Value;

class HashTable {
private:
    const float TARGET_LOAD_FACTOR = 0.75;
    const unsigned INITIAL_BUCKETS_COUNT = 2;
    const unsigned CAPACITY_GROWTH_FACTOR = 2;

    class BucketElement {
    public:
        Key key;
        Value value;
        BucketElement *next;

        BucketElement(const Key &key, const Value &value) {
            this->key = key;
            this->value = value;
            this->next = nullptr;
        }

        void setValue(const Value &value) {
            this->value = value;
        }

        void setNext(BucketElement *const next) {
            this->next = next;
        }
    };

    unsigned findBucketIndexForKey(const Key &key) const {
        unsigned hash = 0;
        for (char i : key) {
            hash += i;
        }

        return hash % this->bucketsCount;
    }

    void rebuildWithGivenBucketsCount(unsigned newBucketsCount) {
        // if we try to resize for the same size, there is no need to do anything
        if (newBucketsCount == this->bucketsCount) {
            return;
        }

        // extract all elements in a temporary array
        auto **allBucketElements = new BucketElement *[this->elementsCount];
        int counter = 0;
        for (unsigned i = 0; i < this->bucketsCount; ++i) {
            BucketElement *bucketElement = this->buckets[i];
            while (bucketElement != nullptr) {
                allBucketElements[counter++] = bucketElement;
                bucketElement = bucketElement->next;
            }
        }

        // zero out internal links because we are going to replace them with new ones
        for (unsigned i = 0; i < this->elementsCount; ++i) {
            allBucketElements[i]->next = nullptr;
        }

        // create new buckets array of the new size
        delete[] this->buckets;
        this->bucketsCount = newBucketsCount;
        this->buckets = new BucketElement *[this->bucketsCount];
        for (unsigned i = 0; i < this->bucketsCount; ++i) {
            this->buckets[i] = nullptr;
        }

        // insert elements back
        for (unsigned i = 0; i < this->elementsCount; ++i) {
            BucketElement *bucketElement = allBucketElements[i];
            unsigned bucketIndex = this->findBucketIndexForKey(bucketElement->key);
            if (this->buckets[bucketIndex] == nullptr) {
                this->buckets[bucketIndex] = bucketElement;
            } else {
                bucketElement->next = this->buckets[bucketIndex];
                this->buckets[bucketIndex] = bucketElement;
            }
        }

        // free temporary resources
        delete[] allBucketElements;
    }

    void prepareCapacityGrowth() {
        // in the beginning: 0 + 1 > 0.75 * 0
        if (this->elementsCount + 1 > TARGET_LOAD_FACTOR * this->bucketsCount) {
            // double capacity or initialize buckets if is empty
            if (this->buckets == nullptr) {
                this->bucketsCount = INITIAL_BUCKETS_COUNT;
                this->buckets = new BucketElement *[this->bucketsCount];
                for(unsigned i = 0 ; i < this->bucketsCount; ++i) {
                    this->buckets[i] = nullptr;
                }
            } else {
                // grow the capacity and rehash
                this->rebuildWithGivenBucketsCount(this->bucketsCount * CAPACITY_GROWTH_FACTOR);
            }
        }
    }

    // is called after elements removal to be able to reduce the buckets count
    void optimizeForCurrentCapacity() {
        // if no elements left, free resources
        // else make the capacity two times smaller
        if (this->elementsCount == 0 && this->bucketsCount > 0) {
            delete[] this->buckets;
            this->bucketsCount = 0;
            this->buckets = nullptr;
        } else if (this->elementsCount < TARGET_LOAD_FACTOR * this->bucketsCount / CAPACITY_GROWTH_FACTOR) {
            // reduce the buckets count and rehash
            this->rebuildWithGivenBucketsCount(this->bucketsCount / CAPACITY_GROWTH_FACTOR);
        }
    }

    BucketElement **createBucketsDeepCopy() const {
        auto **copiedBuckets = new BucketElement *[this->bucketsCount];

        for (unsigned i = 0; i < this->bucketsCount; ++i) {
            copiedBuckets[i] = nullptr;

            BucketElement *originalBucketElement = this->buckets[i];
            while (originalBucketElement != nullptr) {
                // copy
                auto copiedBucketElement = new BucketElement(
                        originalBucketElement->key, originalBucketElement->value
                );
                copiedBucketElement->next = copiedBuckets[i];
                copiedBuckets[i] = copiedBucketElement;

                // move forward
                originalBucketElement = originalBucketElement->next;
            }
        }

        return copiedBuckets;
    }

    unsigned elementsCount;
    unsigned bucketsCount;
    BucketElement **buckets;
public:
    HashTable() {
        this->elementsCount = 0;
        this->bucketsCount = 0;
        this->buckets = nullptr;
    }

    ~HashTable() {
        // deletes the content if there is any
        this->clear();
    }

    // copy constructor
    HashTable(const HashTable &b) {
        this->elementsCount = b.elementsCount;
        this->bucketsCount = b.bucketsCount;
        this->buckets = b.createBucketsDeepCopy();
    }

    // move constructor
    HashTable(HashTable &&b) {
        // move resources
        this->elementsCount = b.elementsCount;
        this->bucketsCount = b.bucketsCount;
        this->buckets = b.buckets;

        // make sure the destructor does not wipe the elements
        b.elementsCount = 0;
        b.bucketsCount = 0;
        b.buckets = nullptr;
    }

    // assignment operator
    HashTable &operator=(const HashTable &b) {
        // self-assignment case
        if (this == &b) {
            return *this;
        }

        // free internal resources
        this->clear();

        // copy resources
        this->elementsCount = b.elementsCount;
        this->bucketsCount = b.bucketsCount;
        this->buckets = b.createBucketsDeepCopy();

        return *this;
    }

    // move assignment operator
    HashTable &operator=(HashTable &&b) {
        // self-assignment case
        if (this == &b) {
            return *this;
        }

        // move resources
        this->elementsCount = b.elementsCount;
        this->bucketsCount = b.bucketsCount;
        this->buckets = b.buckets;

        // make sure the destructor does not wipe them
        b.elementsCount = 0;
        b.bucketsCount = 0;
        b.buckets = nullptr;

        return *this;
    }

    // Обменивает значения двух хэш-таблиц.
    void swap(HashTable &b) {
        std::swap(this->bucketsCount, b.bucketsCount);
        std::swap(this->elementsCount, b.elementsCount);
        std::swap(this->buckets, b.buckets);
    }

    // Очищает контейнер.
    void clear() {
        // if there are no elements, there is nothing to clear
        if (this->elementsCount == 0) {
            return;
        }

        for (unsigned i = 0; i < this->bucketsCount; ++i) {
            // deleting all BucketElements
            if (this->buckets[i] != nullptr) {
                BucketElement *element = this->buckets[i];
                while (element != nullptr) {
                    BucketElement *next = element->next;
                    delete element;
                    element = next;
                }
                this->buckets[i] = nullptr;
            }
        }

        // deleting buckets array
        delete[] this->buckets;
        this->buckets = nullptr;
        this->bucketsCount = 0;
        this->elementsCount = 0;
    }

    // Удаляет элемент по заданному ключу.
    bool erase(const Key &k) {
        // if no elements, there is nothing to delete
        // what's more, it allows to avoid deleting by zero in the bucket index
        // calculation code
        if (this->elementsCount == 0) {
            return false;
        }

        // what bucket does the key k correspond to
        unsigned bucketIndex = this->findBucketIndexForKey(k);

        BucketElement *bucketElementPrevious = this->buckets[bucketIndex];
        BucketElement *bucketElementCurrent = bucketElementPrevious != nullptr ? bucketElementPrevious->next : nullptr;

        // case: bucket is empty
        if (bucketElementPrevious == nullptr) {
            return false;
        }

        // case: it is the first element of the bucket
        if (bucketElementPrevious->key == k) {
            delete bucketElementPrevious;
            this->buckets[bucketIndex] = bucketElementCurrent;
            this->elementsCount--;
            this->optimizeForCurrentCapacity();
            return true;
        }

        // it is a non-first element of the bucket
        while (bucketElementCurrent != nullptr) {
            bucketElementPrevious = bucketElementCurrent;
            bucketElementCurrent = bucketElementCurrent->next;

            if (bucketElementCurrent->key == k) {
                bucketElementPrevious->next = bucketElementCurrent->next;
                delete bucketElementCurrent;
                this->elementsCount--;
                this->optimizeForCurrentCapacity();
                return true;
            }
        }

        return false;
    }

    // Вставка в контейнер. Возвращаемое значение - успешность вставки.
    bool insert(const Key &k, const Value &v) {
        // probably there is already an entry with the key k
        if (this->contains(k)) {
            return false;
        }

        // the key k does not exist in the hashmap, inserting a new value

        this->prepareCapacityGrowth();

        unsigned bucketIndex = this->findBucketIndexForKey(k);

        auto newElement = new BucketElement(k, v);
        newElement->setNext(this->buckets[bucketIndex]);
        this->buckets[bucketIndex] = newElement;
        this->elementsCount++;
        return true;
    }

    // Проверка наличия значения по заданному ключу.
    bool contains(const Key &k) const {
        // if there are no elements, no need to search
        if (this->elementsCount == 0) {
            return false;
        }

        unsigned bucketIndex = this->findBucketIndexForKey(k);

        BucketElement *bucketElement = this->buckets[bucketIndex];
        while (bucketElement != nullptr) {
            if (bucketElement->key == k) {
                return true;
            }

            bucketElement = bucketElement->next;
        }

        return false;
    }

    // Возвращает значение по ключу. Небезопасный метод.
    // В случае отсутствия ключа в контейнере, следует вставить в контейнер
    // значение, созданное конструктором по умолчанию и вернуть ссылку на него.
    Value &operator[](const Key &k) {
        // if there is no elements, there is no need to search
        // what's more it avoids dividing by zero
        if (this->elementsCount > 0) {
            unsigned bucketIndex = this->findBucketIndexForKey(k);
            BucketElement *bucketElement = this->buckets[bucketIndex];
            while (bucketElement != nullptr) {
                if (bucketElement->key == k) {
                    return bucketElement->value;
                }
                bucketElement = bucketElement->next;
            }
        }

        // create new dummy entry
        this->insert(k, Value());

        return this->at(k);
    }

    // Возвращает значение по ключу. Бросает исключение при неудаче.
    Value &at(const Key &k) {
        // @see https://riptutorial.com/cplusplus/example/16974/avoiding-duplication-of-code-in-const-and-non-const-getter-methods-
        return const_cast<Value &>(const_cast<const HashTable *>(this)->at(k));
    }

    const Value &at(const Key &k) const {
        if (this->elementsCount > 0) {
            unsigned bucketIndex = this->findBucketIndexForKey(k);
            BucketElement *bucketElement = this->buckets[bucketIndex];
            while (bucketElement != nullptr) {
                if (bucketElement->key == k) {
                    return bucketElement->value;
                }
                bucketElement = bucketElement->next;
            }
        }

        throw HashTableKeyNotFoundException();
    }

    size_t size() const {
        return this->elementsCount;
    }

    bool empty() const {
        return this->elementsCount == 0;
    }

    friend bool operator==(const HashTable &a, const HashTable &b);

    friend bool operator!=(const HashTable &a, const HashTable &b);
};

#endif //NSU_OOP_2021_LABS_HASHTABLE_H
