#ifndef STUDENTDATABASE_H
#define STUDENTDATABASE_H

#include "Student.h"
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <chrono>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>

// Helper function to trim whitespace and quotes
std::string trimQuotes(const std::string& str) {
    std::string result = str;
    // Remove leading whitespace
    size_t start = result.find_first_not_of(" \t\r\n\"");
    if (start == std::string::npos) return "";
    result = result.substr(start);
    // Remove trailing whitespace
    size_t end = result.find_last_not_of(" \t\r\n\"");
    if (end == std::string::npos) return "";
    result = result.substr(0, end + 1);
    return result;
}

// Student database for IIIT-Delhi using string-based course codes
class IIITStudentDatabase {
private:
    std::vector<Student<std::string, std::string>> students; // Vector maintains insertion order
    std::map<std::string, std::vector<Student<std::string, std::string>*>> courseGradeIndex; // course -> list of students with grade >= 9
    std::vector<Student<std::string, std::string>> sortedStudents;
    mutable std::mutex dbMutex;
    
public:
    // Add a student
    void addStudent(const Student<std::string, std::string>& student) {
        std::lock_guard<std::mutex> lock(dbMutex);
        students.push_back(student);
    }
    
    // Update student information (Admin function)
    bool updateStudent(const std::string& rollNo, const std::string& name, 
                      const std::string& branch, int year) {
        std::lock_guard<std::mutex> lock(dbMutex);
        for (auto& student : students) {
            if (student.getRollNumber() == rollNo) {
                student.setName(name);
                student.setBranch(branch);
                student.setStartingYear(year);
                return true;
            }
        }
        return false;
    }
    
    // Find student by roll number
    Student<std::string, std::string>* findStudent(const std::string& rollNo) {
        std::lock_guard<std::mutex> lock(dbMutex);
        for (auto& student : students) {
            if (student.getRollNumber() == rollNo) {
                return &student;
            }
        }
        return nullptr;
    }
    
    // Get all students in insertion order (const iterator)
    const std::vector<Student<std::string, std::string>>& getStudentsInsertionOrder() const {
        return students;
    }
    
    // Get sorted students
    const std::vector<Student<std::string, std::string>>& getSortedStudents() const {
        return sortedStudents;
    }
    
    // Build course-grade index for fast lookup (requirement 5)
    void buildCourseGradeIndex() {
        std::lock_guard<std::mutex> lock(dbMutex);
        courseGradeIndex.clear();
        
        for (auto& student : students) {
            for (const auto& prevCourse : student.getPreviousCourses()) {
                if (prevCourse.second >= 9) { // Grade >= 9
                    courseGradeIndex[prevCourse.first].push_back(&student);
                }
            }
        }
    }
    
    // Get students with grade >= 9 in a specific course
    std::vector<Student<std::string, std::string>*> getStudentsByGradeAndCourse(
        const std::string& course, int minGrade = 9) {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        std::vector<Student<std::string, std::string>*> result;
        auto it = courseGradeIndex.find(course);
        
        if (it != courseGradeIndex.end()) {
            for (auto student : it->second) {
                if (student->getGradeForCourse(course) >= minGrade) {
                    result.push_back(student);
                }
            }
        }
        
        return result;
    }
    
    // Read students from CSV file - FIXED CSV PARSING
    void readFromCSV(const std::string& filename) {
        std::ifstream file(filename);
        std::string line;
        bool firstLine = true;
        
        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }
        
        while (std::getline(file, line)) {
            if (firstLine) {
                firstLine = false;
                continue; // Skip header
            }
            
            // Parse CSV with proper handling of quoted fields
            std::vector<std::string> fields;
            std::string field;
            bool inQuotes = false;
            
            for (size_t i = 0; i < line.length(); i++) {
                char c = line[i];
                
                if (c == '"') {
                    inQuotes = !inQuotes;
                } else if (c == ',' && !inQuotes) {
                    fields.push_back(trimQuotes(field));
                    field.clear();
                } else {
                    field += c;
                }
            }
            fields.push_back(trimQuotes(field)); // Add last field
            
            if (fields.size() < 6) continue; // Skip malformed lines
            
            std::string rollNo = trimQuotes(fields[0]);
            std::string name = trimQuotes(fields[1]);
            std::string branch = trimQuotes(fields[2]);
            std::string yearStr = trimQuotes(fields[3]);
            std::string currentCoursesStr = trimQuotes(fields[4]);
            std::string prevCoursesStr = trimQuotes(fields[5]);
            
            try {
                int year = std::stoi(yearStr);
                Student<std::string, std::string> student(rollNo, name, branch, year);
                
                // Add current courses
                if (!currentCoursesStr.empty()) {
                    std::stringstream ccs(currentCoursesStr);
                    std::string course;
                    while (std::getline(ccs, course, ',')) {
                        course = trimQuotes(course);
                        if (!course.empty()) {
                            student.addCurrentCourse(course);
                        }
                    }
                }
                
                // Add previous courses with grades
                if (!prevCoursesStr.empty()) {
                    std::stringstream pcs(prevCoursesStr);
                    std::string courseGrade;
                    while (std::getline(pcs, courseGrade, ',')) {
                        courseGrade = trimQuotes(courseGrade);
                        if (!courseGrade.empty()) {
                            size_t colonPos = courseGrade.find(':');
                            if (colonPos != std::string::npos) {
                                std::string courseName = trimQuotes(courseGrade.substr(0, colonPos));
                                int grade = std::stoi(trimQuotes(courseGrade.substr(colonPos + 1)));
                                student.addPreviousCourse(courseName, grade);
                            }
                        }
                    }
                }
                
                addStudent(student);
            } catch (const std::exception& e) {
                // Skip lines with parsing errors
                continue;
            }
        }
        
        file.close();
    }
    
    // Parallel sorting with at least 2 threads
    void parallelSort() {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        sortedStudents = students;
        int n = sortedStudents.size();
        
        if (n == 0) {
            std::cout << "No students to sort." << std::endl;
            return;
        }
        
        int mid = n / 2;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Thread 1: Sort first half
        auto sortFirstHalf = [this, mid]() {
            auto t1Start = std::chrono::high_resolution_clock::now();
            std::sort(sortedStudents.begin(), sortedStudents.begin() + mid,
                     [](const Student<std::string, std::string>& a, 
                        const Student<std::string, std::string>& b) {
                         return a.getRollNumber() < b.getRollNumber();
                     });
            auto t1End = std::chrono::high_resolution_clock::now();
            auto t1Duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1End - t1Start);
            std::cout << "Thread 1 (First half) sorting time: " << t1Duration.count() << " ms" << std::endl;
        };
        
        // Thread 2: Sort second half
        auto sortSecondHalf = [this, mid, n]() {
            auto t2Start = std::chrono::high_resolution_clock::now();
            std::sort(sortedStudents.begin() + mid, sortedStudents.end(),
                     [](const Student<std::string, std::string>& a, 
                        const Student<std::string, std::string>& b) {
                         return a.getRollNumber() < b.getRollNumber();
                     });
            auto t2End = std::chrono::high_resolution_clock::now();
            auto t2Duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2End - t2Start);
            std::cout << "Thread 2 (Second half) sorting time: " << t2Duration.count() << " ms" << std::endl;
        };
        
        std::thread t1(sortFirstHalf);
        std::thread t2(sortSecondHalf);
        
        t1.join();
        t2.join();
        
        // Merge the two sorted halves
        std::vector<Student<std::string, std::string>> temp(n);
        int i = 0, j = mid, k = 0;
        
        while (i < mid && j < n) {
            if (sortedStudents[i].getRollNumber() < sortedStudents[j].getRollNumber()) {
                temp[k++] = sortedStudents[i++];
            } else {
                temp[k++] = sortedStudents[j++];
            }
        }
        
        while (i < mid) {
            temp[k++] = sortedStudents[i++];
        }
        
        while (j < n) {
            temp[k++] = sortedStudents[j++];
        }
        
        sortedStudents = temp;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        std::cout << "Total parallel sorting time: " << totalDuration.count() << " ms" << std::endl;
    }
    
    // Get number of students
    size_t getStudentCount() const {
        return students.size();
    }
    
    // Display all students
    void displayAll() const {
        std::lock_guard<std::mutex> lock(dbMutex);
        for (const auto& student : students) {
            student.display();
        }
    }
};

// Student database for IIT-Delhi using integer-based course codes
class IITStudentDatabase {
private:
    std::vector<Student<std::string, int>> students;
    std::map<int, std::vector<Student<std::string, int>*>> courseGradeIndex;
    std::vector<Student<std::string, int>> sortedStudents;
    mutable std::mutex dbMutex;
    
public:
    // Add a student
    void addStudent(const Student<std::string, int>& student) {
        std::lock_guard<std::mutex> lock(dbMutex);
        students.push_back(student);
    }
    
    // Update student information (Admin function)
    bool updateStudent(const std::string& rollNo, const std::string& name,
                      const std::string& branch, int year) {
        std::lock_guard<std::mutex> lock(dbMutex);
        for (auto& student : students) {
            if (student.getRollNumber() == rollNo) {
                student.setName(name);
                student.setBranch(branch);
                student.setStartingYear(year);
                return true;
            }
        }
        return false;
    }
    
    // Find student by roll number
    Student<std::string, int>* findStudent(const std::string& rollNo) {
        std::lock_guard<std::mutex> lock(dbMutex);
        for (auto& student : students) {
            if (student.getRollNumber() == rollNo) {
                return &student;
            }
        }
        return nullptr;
    }
    
    // Get all students in insertion order
    const std::vector<Student<std::string, int>>& getStudentsInsertionOrder() const {
        return students;
    }
    
    // Get sorted students
    const std::vector<Student<std::string, int>>& getSortedStudents() const {
        return sortedStudents;
    }
    
    // Build course-grade index for fast lookup
    void buildCourseGradeIndex() {
        std::lock_guard<std::mutex> lock(dbMutex);
        courseGradeIndex.clear();
        
        for (auto& student : students) {
            for (const auto& prevCourse : student.getPreviousCourses()) {
                if (prevCourse.second >= 9) {
                    courseGradeIndex[prevCourse.first].push_back(&student);
                }
            }
        }
    }
    
    // Get students with grade >= 9 in a specific course
    std::vector<Student<std::string, int>*> getStudentsByGradeAndCourse(
        int course, int minGrade = 9) {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        std::vector<Student<std::string, int>*> result;
        auto it = courseGradeIndex.find(course);
        
        if (it != courseGradeIndex.end()) {
            for (auto student : it->second) {
                if (student->getGradeForCourse(course) >= minGrade) {
                    result.push_back(student);
                }
            }
        }
        
        return result;
    }
    
    // Parallel sorting
    void parallelSort() {
        std::lock_guard<std::mutex> lock(dbMutex);
        
        sortedStudents = students;
        int n = sortedStudents.size();
        
        if (n == 0) {
            std::cout << "No students to sort." << std::endl;
            return;
        }
        
        int mid = n / 2;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        auto sortFirstHalf = [this, mid]() {
            auto t1Start = std::chrono::high_resolution_clock::now();
            std::sort(sortedStudents.begin(), sortedStudents.begin() + mid,
                     [](const Student<std::string, int>& a,
                        const Student<std::string, int>& b) {
                         return a.getRollNumber() < b.getRollNumber();
                     });
            auto t1End = std::chrono::high_resolution_clock::now();
            auto t1Duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1End - t1Start);
            std::cout << "Thread 1 (First half) sorting time: " << t1Duration.count() << " ms" << std::endl;
        };
        
        auto sortSecondHalf = [this, mid, n]() {
            auto t2Start = std::chrono::high_resolution_clock::now();
            std::sort(sortedStudents.begin() + mid, sortedStudents.end(),
                     [](const Student<std::string, int>& a,
                        const Student<std::string, int>& b) {
                         return a.getRollNumber() < b.getRollNumber();
                     });
            auto t2End = std::chrono::high_resolution_clock::now();
            auto t2Duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2End - t2Start);
            std::cout << "Thread 2 (Second half) sorting time: " << t2Duration.count() << " ms" << std::endl;
        };
        
        std::thread t1(sortFirstHalf);
        std::thread t2(sortSecondHalf);
        
        t1.join();
        t2.join();
        
        // Merge
        std::vector<Student<std::string, int>> temp(n);
        int i = 0, j = mid, k = 0;
        
        while (i < mid && j < n) {
            if (sortedStudents[i].getRollNumber() < sortedStudents[j].getRollNumber()) {
                temp[k++] = sortedStudents[i++];
            } else {
                temp[k++] = sortedStudents[j++];
            }
        }
        
        while (i < mid) {
            temp[k++] = sortedStudents[i++];
        }
        
        while (j < n) {
            temp[k++] = sortedStudents[j++];
        }
        
        sortedStudents = temp;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        std::cout << "Total parallel sorting time: " << totalDuration.count() << " ms" << std::endl;
    }
    
    size_t getStudentCount() const {
        return students.size();
    }
    
    void displayAll() const {
        std::lock_guard<std::mutex> lock(dbMutex);
        for (const auto& student : students) {
            student.display();
        }
    }
};

#endif // STUDENTDATABASE_H
