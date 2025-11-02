#ifndef ITERATOR_H
#define ITERATOR_H

#include <iterator>
#include <vector>
#include <memory>

// Iterator for records in order of entry (insertion order)
template <typename T>
class InsertionOrderIterator {
private:
    std::vector<T>* data;
    std::vector<int>* indices;
    size_t currentIndex;

public:
    InsertionOrderIterator(std::vector<T>* d, std::vector<int>* idx)
        : data(d), indices(idx), currentIndex(0) {}

    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::forward_iterator_tag;

    InsertionOrderIterator& operator++() {
        ++currentIndex;
        return *this;
    }

    T& operator*() {
        return (*data)[(*indices)[currentIndex]];
    }

    T* operator->() {
        return &(*data)[(*indices)[currentIndex]];
    }

    bool operator==(const InsertionOrderIterator& other) const {
        return currentIndex == other.currentIndex;
    }

    bool operator!=(const InsertionOrderIterator& other) const {
        return !(*this == other);
    }

    size_t getSize() const { return indices->size(); }
};

// Iterator for sorted order records
template <typename T>
class SortedOrderIterator {
private:
    std::vector<T>* data;
    std::vector<int>* indices;
    size_t currentIndex;

public:
    SortedOrderIterator(std::vector<T>* d, std::vector<int>* idx)
        : data(d), indices(idx), currentIndex(0) {}

    using difference_type = std::ptrdiff_t;
    using value_type = T;
    using pointer = T*;
    using reference = T&;
    using iterator_category = std::random_access_iterator_tag;

    SortedOrderIterator& operator++() {
        ++currentIndex;
        return *this;
    }

    SortedOrderIterator operator++(int) {
        SortedOrderIterator tmp = *this;
        ++currentIndex;
        return tmp;
    }

    SortedOrderIterator& operator--() {
        --currentIndex;
        return *this;
    }

    SortedOrderIterator operator--(int) {
        SortedOrderIterator tmp = *this;
        --currentIndex;
        return tmp;
    }

    SortedOrderIterator operator+(difference_type n) const {
        SortedOrderIterator tmp = *this;
        tmp.currentIndex += n;
        return tmp;
    }

    SortedOrderIterator& operator+=(difference_type n) {
        currentIndex += n;
        return *this;
    }

    T& operator*() {
        return (*data)[(*indices)[currentIndex]];
    }

    T* operator->() {
        return &(*data)[(*indices)[currentIndex]];
    }

    bool operator==(const SortedOrderIterator& other) const {
        return currentIndex == other.currentIndex;
    }

    bool operator!=(const SortedOrderIterator& other) const {
        return !(*this == other);
    }

    bool operator<(const SortedOrderIterator& other) const {
        return currentIndex < other.currentIndex;
    }

    difference_type operator-(const SortedOrderIterator& other) const {
        return currentIndex - other.currentIndex;
    }

    size_t getSize() const { return indices->size(); }

    T& operator[](size_t idx) {
        return (*data)[(*indices)[idx]];
    }
};

#endif // ITERATOR_H
