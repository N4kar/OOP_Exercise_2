#include <algorithm>
#include <string>
// for std::tie -- tuple comparison helper
#include <tuple>
#include "HashTable.h"

bool operator==(const HashTable & a, const HashTable &b) {
    using BucketElement = HashTable::BucketElement;

    if (a.elementsCount != b.elementsCount) {
        return false;
    }

    unsigned counter1 = 0;
    auto **elements1 = new BucketElement *[a.elementsCount];

    unsigned counter2 = 0;
    auto **elements2 = new BucketElement *[b.elementsCount];

    for (int i = 0; i < a.elementsCount; ++i) {
        BucketElement *element1 = a.buckets[i];
        while (element1 != nullptr) {
            elements1[counter1++] = element1;
            element1 = element1->next;
        }

        BucketElement *element2 = b.buckets[i];
        while (element2 != nullptr) {
            elements2[counter2++] = element2;
            element2 = element2->next;
        }
    }

    auto comparator = [](const BucketElement *obj1, const BucketElement *obj2) {
        return std::tie(obj1->key, obj1->value) < std::tie(obj2->key, obj2->value);
    };

    std::sort(elements1, elements1 + a.elementsCount, comparator);
    std::sort(elements2, elements2 + b.elementsCount, comparator);

    bool equal = true;
    for (int i = 0; i < a.elementsCount; ++i) {
        if (elements1[i]->key != elements2[i]->key
            || elements1[i]->value != elements2[i]->value) {
            equal = false;
            break;
        }
    }

    delete[] elements1;
    delete[] elements2;

    return equal;
}

bool operator!=(const HashTable &a, const HashTable &b) {
    // via the == operator
    return !(a == b);
}
