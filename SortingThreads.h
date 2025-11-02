#ifndef SORTING_THREADS_H
#define SORTING_THREADS_H

#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <ctime>

class SortingThreadsManager {
private:
    struct ThreadStats {
        int threadId;
        long long startTime;
        long long endTime;
        int recordsProcessed;

        long long getDurationMs() const {
            return endTime - startTime;
        }
    };

    std::vector<ThreadStats> threadStats;
    std::mutex statsMutex;

public:
    SortingThreadsManager() {}

    // Merge sort implementation for use in threads
    template <typename T>
    void mergeSort(std::vector<T>& arr, int left, int right, int threadId, int totalRecords) {
        auto startTime = std::chrono::high_resolution_clock::now();
        long long startMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            startTime.time_since_epoch()).count();

        mergeSortUtil(arr, left, right);

        auto endTime = std::chrono::high_resolution_clock::now();
        long long endMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            endTime.time_since_epoch()).count();

        // Record statistics
        {
            std::lock_guard<std::mutex> lock(statsMutex);
            ThreadStats stats;
            stats.threadId = threadId;
            stats.startTime = startMs;
            stats.endTime = endMs;
            stats.recordsProcessed = (right - left + 1);
            threadStats.push_back(stats);
        }
    }

    // Utility merge sort function
    template <typename T>
    void mergeSortUtil(std::vector<T>& arr, int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            mergeSortUtil(arr, left, mid);
            mergeSortUtil(arr, mid + 1, right);
            merge(arr, left, mid, right);
        }
    }

    // Merge function
    template <typename T>
    void merge(std::vector<T>& arr, int left, int mid, int right) {
        std::vector<T> temp(right - left + 1);
        int i = left, j = mid + 1, k = 0;

        while (i <= mid && j <= right) {
            if (arr[i] < arr[j]) {
                temp[k++] = arr[i++];
            } else {
                temp[k++] = arr[j++];
            }
        }

        while (i <= mid) {
            temp[k++] = arr[i++];
        }

        while (j <= right) {
            temp[k++] = arr[j++];
        }

        for (int idx = 0; idx < k; idx++) {
            arr[left + idx] = temp[idx];
        }
    }

    // Parallel sorting with multiple threads
    template <typename T>
    void parallelSort(std::vector<T>& data, int numThreads = 2) {
        threadStats.clear();

        if (numThreads < 2) numThreads = 2;
        if (numThreads > (int)data.size()) numThreads = data.size();

        std::vector<std::thread> threads;
        int recordsPerThread = data.size() / numThreads;

        std::cout << "\n=== Starting Parallel Sort ===" << std::endl;
        std::cout << "Total Records: " << data.size() << std::endl;
        std::cout << "Number of Threads: " << numThreads << std::endl;
        std::cout << "Records per thread (approx): " << recordsPerThread << std::endl;

        // Create and launch threads
        for (int i = 0; i < numThreads; i++) {
            int start = i * recordsPerThread;
            int end = (i == numThreads - 1) ? (data.size() - 1) : ((i + 1) * recordsPerThread - 1);

            threads.emplace_back(&SortingThreadsManager::mergeSort<T>, this, 
                                std::ref(data), start, end, i, data.size());
        }

        // Wait for all threads to complete
        for (auto& t : threads) {
            t.join();
        }

        // Now merge the sorted segments
        for (int size = recordsPerThread; size < (int)data.size(); size *= 2) {
            for (int start = 0; start < (int)data.size(); start += size * 2) {
                int mid = start + size - 1;
                int end = std::min(start + size * 2 - 1, (int)data.size() - 1);

                if (mid < end) {
                    merge(data, start, mid, end);
                }
            }
        }

        printThreadStatistics();
    }

    void printThreadStatistics() const {
        std::cout << "\n=== Thread Statistics ===" << std::endl;
        std::cout << std::left << std::setw(12) << "Thread ID" 
                  << std::setw(15) << "Duration (ms)" 
                  << std::setw(15) << "Records" << std::endl;
        std::cout << std::string(42, '-') << std::endl;

        for (const auto& stat : threadStats) {
            std::cout << std::setw(12) << stat.threadId
                      << std::setw(15) << stat.getDurationMs()
                      << std::setw(15) << stat.recordsProcessed << std::endl;
        }

        long long totalTime = 0;
        for (const auto& stat : threadStats) {
            totalTime += stat.getDurationMs();
        }

        std::cout << std::string(42, '-') << std::endl;
        std::cout << "Total Time (all threads): " << totalTime << " ms" << std::endl;

        // Log to file
        std::ofstream logFile("sorting_thread_log.txt", std::ios::app);
        logFile << "\n=== Sorting Log " << std::time(nullptr) << " ===" << std::endl;
        for (const auto& stat : threadStats) {
            logFile << "Thread " << stat.threadId << ": " << stat.getDurationMs() << " ms" << std::endl;
        }
        logFile.close();
    }
};

#endif // SORTING_THREADS_H
