#ifndef STUDENTDATABASE_H
#define STUDENTDATABASE_H

#include "Student.h"
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <thread>
#include <mutex>
#include <chrono>

// Base database class with common functionality
template<typename RollNoType, typename CourseCodeType>
class StudentDatabase {
protected:
    std::vector<Student<RollNoType, CourseCodeType>> studentsData;
    std::vector<Student<RollNoType, CourseCodeType>> sortedStudentsData;
    std::map<CourseCodeType, std::vector<Student<RollNoType, CourseCodeType>*>> courseGradeIndex;
    mutable std::mutex dbMutex;

    // Helper function: Merge two sorted ranges
    void merge(std::vector<Student<RollNoType, CourseCodeType>>& data, 
               int left, int mid, int right) {
        std::vector<Student<RollNoType, CourseCodeType>> temp;
        int i = left, j = mid + 1;

        while (i <= mid && j <= right) {
            if (data[i] < data[j]) {
                temp.push_back(data[i++]);
            } else {
                temp.push_back(data[j++]);
            }
        }

        while (i <= mid) {
            temp.push_back(data[i++]);
        }

        while (j <= right) {
            temp.push_back(data[j++]);
        }

        for (int i = left; i <= right; i++) {
            data[i] = temp[i - left];
        }
    }

    // Helper function: Sort a range using merge sort
    void mergeSort(std::vector<Student<RollNoType, CourseCodeType>>& data, 
                   int left, int right) {
        if (left < right) {
            int mid = left + (right - left) / 2;
            mergeSort(data, left, mid);
            mergeSort(data, mid + 1, right);
            merge(data, left, mid, right);
        }
    }

public:
    virtual ~StudentDatabase() = default;

    // Add a new student (thread-safe)
    void addStudent(const Student<RollNoType, CourseCodeType>& student) {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        // Check if student already exists
        for (const auto& s : studentsData) {
            if (s.getRollNumber() == student.getRollNumber()) {
                std::cout << "Error: Student with this roll number already exists!" << std::endl;
                return;
            }
        }
        
        studentsData.push_back(student);
    }

    // Update existing student
    void updateStudent(const RollNoType& rollNo, const Student<RollNoType, CourseCodeType>& updatedStudent) {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        for (auto& s : studentsData) {
            if (s.getRollNumber() == rollNo) {
                s = updatedStudent;
                return;
            }
        }
        std::cout << "Student not found!" << std::endl;
    }

    // Find student by roll number (thread-safe)
    Student<RollNoType, CourseCodeType>* findStudent(const RollNoType& rollNo) {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        for (auto& s : studentsData) {
            if (s.getRollNumber() == rollNo) {
                return &s;
            }
        }
        return nullptr;
    }

    // Get insertion order iterator (thread-safe)
    const std::vector<Student<RollNoType, CourseCodeType>>& getStudentsInsertionOrder() const {
        return studentsData;
    }

    // Get sorted order iterator (thread-safe)
    const std::vector<Student<RollNoType, CourseCodeType>>& getSortedStudents() const {
        return sortedStudentsData;
    }

    // Parallel sorting implementation (thread-safe)
    void parallelSort() {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        if (studentsData.empty()) {
            std::cout << "No students to sort!" << std::endl;
            return;
        }

        // Create a copy for sorting
        sortedStudentsData = studentsData;
        int n = sortedStudentsData.size();
        int mid = n / 2;

        // Thread variables
        auto start_total = std::chrono::high_resolution_clock::now();

        // Thread 1: Sort first half
        auto start1 = std::chrono::high_resolution_clock::now();
        std::thread thread1([this, mid]() {
            if (mid > 0) {
                this->mergeSort(this->sortedStudentsData, 0, mid - 1);
            }
        });

        // Thread 2: Sort second half
        auto start2 = std::chrono::high_resolution_clock::now();
        std::thread thread2([this, mid, n]() {
            if (mid < n) {
                this->mergeSort(this->sortedStudentsData, mid, n - 1);
            }
        });

        // Wait for both threads to complete
        thread1.join();
        thread2.join();

        auto end1 = std::chrono::high_resolution_clock::now();
        auto end2 = std::chrono::high_resolution_clock::now();

        // Merge the two sorted halves
        if (n > 1) {
            merge(sortedStudentsData, 0, mid - 1, n - 1);
        }

        auto end_total = std::chrono::high_resolution_clock::now();

        // Calculate timings
        auto duration1 = std::chrono::duration_cast<std::chrono::milliseconds>(end1 - start1);
        auto duration2 = std::chrono::duration_cast<std::chrono::milliseconds>(end2 - start2);
        auto duration_total = std::chrono::duration_cast<std::chrono::milliseconds>(end_total - start_total);

        std::cout << "Performing parallel sorting..." << std::endl;
        std::cout << "Thread 1 (First half) sorting time: " << duration1.count() << " ms" << std::endl;
        std::cout << "Thread 2 (Second half) sorting time: " << duration2.count() << " ms" << std::endl;
        std::cout << "Total parallel sorting time (including merge): " << duration_total.count() << " ms" << std::endl;
        std::cout << "Sorting completed!" << std::endl;
    }

    // Build course grade index for fast lookup (thread-safe)
    void buildCourseGradeIndex() {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        courseGradeIndex.clear();

        for (auto& student : studentsData) {
            const auto& previousCourses = student.getPreviousCourses();
            for (const auto& [courseCode, grade] : previousCourses) {
                if (grade >= 9) {
                    courseGradeIndex[courseCode].push_back(&student);
                }
            }
        }
    }

    // Get students by grade and course (O(1) lookup)
    const std::vector<Student<RollNoType, CourseCodeType>*>* getStudentsByGradeAndCourse(const CourseCodeType& courseCode) const {
        auto it = courseGradeIndex.find(courseCode);
        if (it != courseGradeIndex.end()) {
            return &(it->second);
        }
        return nullptr;
    }

    // Get total number of students
    int getTotalStudents() const {
        return studentsData.size();
    }
};

// IIIT-Delhi database using string course codes
class IIITStudentDatabase : public StudentDatabase<std::string, std::string> {
public:
    // Read from CSV file - FIXED CSV parsing
    void readFromCSV(const std::string& filename) {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Could not open file " << filename << std::endl;
            return;
        }

        std::string line;
        int count = 0;

        // Skip header if present
        std::getline(file, line);

        while (std::getline(file, line) && count < 3000) {
            if (line.empty()) continue;

            std::stringstream ss(line);
            std::string rollNo, name, branch, yearStr, currentCoursesStr, previousCoursesStr;

            if (!std::getline(ss, rollNo, ',')) continue;
            if (!std::getline(ss, name, ',')) continue;
            if (!std::getline(ss, branch, ',')) continue;
            if (!std::getline(ss, yearStr, ',')) continue;
            if (!std::getline(ss, currentCoursesStr, ',')) continue;
            if (!std::getline(ss, previousCoursesStr)) continue;

            // Remove quotes if present
            rollNo.erase(std::remove(rollNo.begin(), rollNo.end(), '"'), rollNo.end());
            name.erase(std::remove(name.begin(), name.end(), '"'), name.end());
            branch.erase(std::remove(branch.begin(), branch.end(), '"'), branch.end());
            yearStr.erase(std::remove(yearStr.begin(), yearStr.end(), '"'), yearStr.end());
            currentCoursesStr.erase(std::remove(currentCoursesStr.begin(), currentCoursesStr.end(), '"'), currentCoursesStr.end());
            previousCoursesStr.erase(std::remove(previousCoursesStr.begin(), previousCoursesStr.end(), '"'), previousCoursesStr.end());

            // Trim whitespace
            rollNo.erase(0, rollNo.find_first_not_of(" \t\r\n"));
            rollNo.erase(rollNo.find_last_not_of(" \t\r\n") + 1);
            name.erase(0, name.find_first_not_of(" \t\r\n"));
            name.erase(name.find_last_not_of(" \t\r\n") + 1);

            int year = std::stoi(yearStr);
            Student<std::string, std::string> student(rollNo, name, branch, year);

            // Parse current courses - FIXED: properly split by space
            std::stringstream currentSS(currentCoursesStr);
            std::string course;
            while (currentSS >> course) {
                if (!course.empty()) {
                    student.addCurrentCourse(course);
                }
            }

            // Parse previous courses with grades - FIXED: properly parse course:grade
            std::stringstream previousSS(previousCoursesStr);
            std::string courseGrade;
            while (previousSS >> courseGrade) {
                if (!courseGrade.empty()) {
                    size_t colonPos = courseGrade.find(':');
                    if (colonPos != std::string::npos) {
                        std::string courseCode = courseGrade.substr(0, colonPos);
                        try {
                            int grade = std::stoi(courseGrade.substr(colonPos + 1));
                            student.addPreviousCourse(courseCode, grade);
                        } catch (...) {
                            // Skip invalid entries
                        }
                    }
                }
            }

            studentsData.push_back(student);
            count++;
        }

        file.close();
        std::cout << "Successfully loaded " << count << " students!" << std::endl;
    }

    // Save students to CSV file - NEW FUNCTION
    void saveToCSV(const std::string& filename) {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Could not create file " << filename << std::endl;
            return;
        }

        // Write header
        file << "RollNo,Name,Branch,Year,CurrentCourses,PreviousCourses\n";

        // Write all students
        for (const auto& student : studentsData) {
            file << student.getRollNumber() << ",";
            file << student.getName() << ",";
            file << student.getBranch() << ",";
            file << student.getStartingYear() << ",";

            // Write current courses
            const auto& currentCourses = student.getCurrentCourses();
            for (size_t i = 0; i < currentCourses.size(); i++) {
                file << currentCourses[i];
                if (i < currentCourses.size() - 1) file << " ";
            }
            file << ",";

            // Write previous courses with grades
            const auto& previousCourses = student.getPreviousCourses();
            for (auto it = previousCourses.begin(); it != previousCourses.end(); ++it) {
                file << it->first << ":" << it->second;
                auto next = std::next(it);
                if (next != previousCourses.end()) file << " ";
            }
            file << "\n";
        }

        file.close();
        std::cout << "Successfully saved " << studentsData.size() << " students to " << filename << std::endl;
    }

    // Save sorted students to CSV file - NEW FUNCTION
    void saveSortedToCSV(const std::string& filename) {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        if (sortedStudentsData.empty()) {
            std::cout << "No sorted data to save! Please perform sorting first." << std::endl;
            return;
        }

        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Could not create file " << filename << std::endl;
            return;
        }

        // Write header
        file << "RollNo,Name,Branch,Year,CurrentCourses,PreviousCourses\n";

        // Write sorted students
        for (const auto& student : sortedStudentsData) {
            file << student.getRollNumber() << ",";
            file << student.getName() << ",";
            file << student.getBranch() << ",";
            file << student.getStartingYear() << ",";

            // Write current courses
            const auto& currentCourses = student.getCurrentCourses();
            for (size_t i = 0; i < currentCourses.size(); i++) {
                file << currentCourses[i];
                if (i < currentCourses.size() - 1) file << " ";
            }
            file << ",";

            // Write previous courses with grades
            const auto& previousCourses = student.getPreviousCourses();
            for (auto it = previousCourses.begin(); it != previousCourses.end(); ++it) {
                file << it->first << ":" << it->second;
                auto next = std::next(it);
                if (next != previousCourses.end()) file << " ";
            }
            file << "\n";
        }

        file.close();
        std::cout << "Successfully saved " << sortedStudentsData.size() << " sorted students to " << filename << std::endl;
    }

    // Display sorted students - NEW FUNCTION
    void displaySorted(int limit = 10) {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        if (sortedStudentsData.empty()) {
            std::cout << "No sorted data! Please perform sorting first." << std::endl;
            return;
        }

        std::cout << "\n--- SORTED STUDENTS (By Roll Number) ---\n";
        int total = sortedStudentsData.size();
        int displayCount = std::min(limit, total);
        std::cout << "Displaying first " << displayCount << " students (Total: " << total << ")\n";
        
        for (int i = 0; i < displayCount; i++) {
            const auto& student = sortedStudentsData[i];
            std::cout << "Roll No: " << student.getRollNumber() 
                      << " | Name: " << student.getName() 
                      << " | Branch: " << student.getBranch() 
                      << " | Year: " << student.getStartingYear() << "\n";
            
            std::cout << "  Current Courses: ";
            for (const auto& course : student.getCurrentCourses()) {
                std::cout << course << " ";
            }
            std::cout << "\n";
            
            std::cout << "  Previous Courses: ";
            for (const auto& [course, grade] : student.getPreviousCourses()) {
                std::cout << course << ":" << grade << " ";
            }
            std::cout << "\n";
        }
    }
};

// IIT-Delhi database using integer course codes
class IITStudentDatabase : public StudentDatabase<std::string, int> {
public:
    // Read from CSV file
    void readFromCSV(const std::string& filename) {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Could not open file " << filename << std::endl;
            return;
        }

        std::string line;
        int count = 0;

        // Skip header if present
        std::getline(file, line);

        while (std::getline(file, line) && count < 3000) {
            if (line.empty()) continue;

            std::stringstream ss(line);
            std::string rollNo, name, branch, yearStr, currentCoursesStr, previousCoursesStr;

            if (!std::getline(ss, rollNo, ',')) continue;
            if (!std::getline(ss, name, ',')) continue;
            if (!std::getline(ss, branch, ',')) continue;
            if (!std::getline(ss, yearStr, ',')) continue;
            if (!std::getline(ss, currentCoursesStr, ',')) continue;
            if (!std::getline(ss, previousCoursesStr)) continue;

            // Remove quotes if present
            rollNo.erase(std::remove(rollNo.begin(), rollNo.end(), '"'), rollNo.end());
            name.erase(std::remove(name.begin(), name.end(), '"'), name.end());
            branch.erase(std::remove(branch.begin(), branch.end(), '"'), branch.end());

            int year = std::stoi(yearStr);
            Student<std::string, int> student(rollNo, name, branch, year);

            // Parse current courses (convert to integers)
            std::stringstream currentSS(currentCoursesStr);
            std::string course;
            while (currentSS >> course) {
                if (!course.empty()) {
                    try {
                        student.addCurrentCourse(std::stoi(course));
                    } catch (...) {
                        // Skip if not a valid integer
                    }
                }
            }

            // Parse previous courses with grades
            std::stringstream previousSS(previousCoursesStr);
            std::string courseGrade;
            while (previousSS >> courseGrade) {
                if (!courseGrade.empty()) {
                    size_t colonPos = courseGrade.find(':');
                    if (colonPos != std::string::npos) {
                        try {
                            int courseCode = std::stoi(courseGrade.substr(0, colonPos));
                            int grade = std::stoi(courseGrade.substr(colonPos + 1));
                            student.addPreviousCourse(courseCode, grade);
                        } catch (...) {
                            // Skip invalid entries
                        }
                    }
                }
            }

            studentsData.push_back(student);
            count++;
        }

        file.close();
        std::cout << "Successfully loaded " << count << " students!" << std::endl;
    }

    // Save students to CSV file
    void saveToCSV(const std::string& filename) {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Could not create file " << filename << std::endl;
            return;
        }

        // Write header
        file << "RollNo,Name,Branch,Year,CurrentCourses,PreviousCourses\n";

        // Write all students
        for (const auto& student : studentsData) {
            file << student.getRollNumber() << ",";
            file << student.getName() << ",";
            file << student.getBranch() << ",";
            file << student.getStartingYear() << ",";

            // Write current courses
            const auto& currentCourses = student.getCurrentCourses();
            for (size_t i = 0; i < currentCourses.size(); i++) {
                file << currentCourses[i];
                if (i < currentCourses.size() - 1) file << " ";
            }
            file << ",";

            // Write previous courses with grades
            const auto& previousCourses = student.getPreviousCourses();
            for (auto it = previousCourses.begin(); it != previousCourses.end(); ++it) {
                file << it->first << ":" << it->second;
                auto next = std::next(it);
                if (next != previousCourses.end()) file << " ";
            }
            file << "\n";
        }

        file.close();
        std::cout << "Successfully saved " << studentsData.size() << " students to " << filename << std::endl;
    }

    // Save sorted students to CSV file
    void saveSortedToCSV(const std::string& filename) {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        if (sortedStudentsData.empty()) {
            std::cout << "No sorted data to save! Please perform sorting first." << std::endl;
            return;
        }

        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cout << "Error: Could not create file " << filename << std::endl;
            return;
        }

        // Write header
        file << "RollNo,Name,Branch,Year,CurrentCourses,PreviousCourses\n";

        // Write sorted students
        for (const auto& student : sortedStudentsData) {
            file << student.getRollNumber() << ",";
            file << student.getName() << ",";
            file << student.getBranch() << ",";
            file << student.getStartingYear() << ",";

            // Write current courses
            const auto& currentCourses = student.getCurrentCourses();
            for (size_t i = 0; i < currentCourses.size(); i++) {
                file << currentCourses[i];
                if (i < currentCourses.size() - 1) file << " ";
            }
            file << ",";

            // Write previous courses with grades
            const auto& previousCourses = student.getPreviousCourses();
            for (auto it = previousCourses.begin(); it != previousCourses.end(); ++it) {
                file << it->first << ":" << it->second;
                auto next = std::next(it);
                if (next != previousCourses.end()) file << " ";
            }
            file << "\n";
        }

        file.close();
        std::cout << "Successfully saved " << sortedStudentsData.size() << " sorted students to " << filename << std::endl;
    }

    // Display sorted students
    void displaySorted(int limit = 10) {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        if (sortedStudentsData.empty()) {
            std::cout << "No sorted data! Please perform sorting first." << std::endl;
            return;
        }

        std::cout << "\n--- SORTED STUDENTS (By Roll Number) ---\n";
        int total = sortedStudentsData.size();
        int displayCount = std::min(limit, total);
        std::cout << "Displaying first " << displayCount << " students (Total: " << total << ")\n";
        
        for (int i = 0; i < displayCount; i++) {
            const auto& student = sortedStudentsData[i];
            std::cout << "Roll No: " << student.getRollNumber() 
                      << " | Name: " << student.getName() 
                      << " | Branch: " << student.getBranch() 
                      << " | Year: " << student.getStartingYear() << "\n";
            
            std::cout << "  Current Courses: ";
            for (const auto& course : student.getCurrentCourses()) {
                std::cout << course << " ";
            }
            std::cout << "\n";
            
            std::cout << "  Previous Courses: ";
            for (const auto& [course, grade] : student.getPreviousCourses()) {
                std::cout << course << ":" << grade << " ";
            }
            std::cout << "\n";
        }
    }
};

#endif // STUDENTDATABASE_H
