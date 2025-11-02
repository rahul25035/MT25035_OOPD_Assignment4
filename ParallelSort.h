#ifndef PARALLEL_SORT_H
#define PARALLEL_SORT_H

#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <memory>

/**
 * @class ParallelSort
 * @brief Performs parallel sorting using multiple threads with no race conditions
 */
template <typename T>
class ParallelSort {
private:
    std::vector<T>& data;
    int numThreads;
    std::vector<double> threadExecutionTimes;
    std::mutex mtx;

    /**
     * @brief Sort a range of elements
     * @param start Start index
     * @param end End index
     * @param threadId Thread identifier
     */
    void sortRange(int start, int end, int threadId) {
        auto startTime = std::chrono::high_resolution_clock::now();

        std::sort(data.begin() + start, data.begin() + end);

        auto endTime = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = endTime - startTime;

        {
            std::lock_guard<std::mutex> lock(mtx);
            threadExecutionTimes[threadId] = duration.count();
        }
    }

    /**
     * @brief Merge two sorted ranges
     * @param left Start of left range
     * @param mid End of left range / start of merge
     * @param right End of right range
     */
    void merge(int left, int mid, int right) {
        std::vector<T> temp;
        int i = left, j = mid + 1;

        while (i <= mid && j <= right) {
            if (data[i] <= data[j]) {
                temp.push_back(data[i++]);
            } else {
                temp.push_back(data[j++]);
            }
        }

        while (i <= mid) temp.push_back(data[i++]);
        while (j <= right) temp.push_back(data[j++]);

        for (int i = left, k = 0; i <= right; i++, k++) {
            data[i] = temp[k];
        }
    }

public:
    /**
     * @brief Constructor
     * @param d Reference to data vector
     * @param threads Number of threads to use
     */
    ParallelSort(std::vector<T>& d, int threads = 2) 
        : data(d), numThreads(threads), threadExecutionTimes(threads, 0.0) {}

    /**
     * @brief Execute parallel sort
     * Sorts data using specified number of threads
     */
    void sort() {
        if (data.size() <= 1 || numThreads <= 1) {
            std::sort(data.begin(), data.end());
            return;
        }

        // Calculate chunk size for each thread
        int chunkSize = data.size() / numThreads;
        std::vector<std::thread> threads;

        // Create and launch threads
        for (int i = 0; i < numThreads; i++) {
            int start = i * chunkSize;
            int end = (i == numThreads - 1) ? data.size() : (i + 1) * chunkSize;
            threads.push_back(std::thread(&ParallelSort::sortRange, this, start, end, i));
        }

        // Wait for all threads to complete
        for (auto& t : threads) {
            t.join();
        }

        // Merge the sorted chunks
        for (int size = chunkSize; size < data.size(); size *= 2) {
            for (int leftStart = 0; leftStart < data.size(); leftStart += size * 2) {
                int mid = leftStart + size - 1;
                int rightEnd = std::min(leftStart + size * 2 - 1, (int)data.size() - 1);

                if (mid < rightEnd) {
                    merge(leftStart, mid, rightEnd);
                }
            }
        }
    }

    /**
     * @brief Get execution time for a specific thread
     * @param threadId Thread identifier
     * @return Time in milliseconds
     */
    double getThreadTime(int threadId) const {
        if (threadId < threadExecutionTimes.size()) {
            return threadExecutionTimes[threadId];
        }
        return -1.0;
    }

    /**
     * @brief Log all thread execution times
     */
    void logThreadTimes() const {
        std::cout << "\n=== Thread Execution Times ===" << std::endl;
        for (int i = 0; i < threadExecutionTimes.size(); i++) {
            std::cout << "Thread " << i << ": " << threadExecutionTimes[i] << " ms" << std::endl;
        }
    }
};

#endif // PARALLEL_SORT_H
