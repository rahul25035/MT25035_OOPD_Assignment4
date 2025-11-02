#ifndef STUDENT_DATABASE_H
#define STUDENT_DATABASE_H

#include "Student.h"
#include <vector>
#include <map>
#include <memory>

/**
 * @class StudentDatabase
 * @brief Database to store and manage student records with multiple orderings
 * 
 * This class maintains:
 * - Insertion order (using vector)
 * - Sorted order (using map with custom comparator)
 * Provides iterators for both orders without data duplication
 */
template <typename RollNumberType, typename CourseCodeType>
class StudentDatabase {
public:
    using StudentPtr = std::shared_ptr<Student<RollNumberType, CourseCodeType>>;

private:
    // Insertion order - using vector of pointers
    std::vector<StudentPtr> insertionOrder;

    // Sorted order - using map with sorting criteria
    // Map key: (name, rollNumber), value: pointer to student
    std::map<std::pair<std::string, RollNumberType>, StudentPtr> sortedByName;

public:
    /**
     * @brief Add a student to the database
     * @param student Shared pointer to student
     */
    void addStudent(StudentPtr student) {
        insertionOrder.push_back(student);
        sortedByName[std::make_pair(student->getName(), student->getRollNumber())] = student;
    }

    /**
     * @brief Get size of database
     * @return Number of students
     */
    size_t size() const {
        return insertionOrder.size();
    }

    /**
     * @brief Iterator for insertion order
     * @return Iterator to first student (insertion order)
     */
    auto getInsertionOrderIterator() const {
        return insertionOrder.begin();
    }

    /**
     * @brief End iterator for insertion order
     * @return End iterator (insertion order)
     */
    auto getInsertionOrderEnd() const {
        return insertionOrder.end();
    }

    /**
     * @brief Iterator for sorted order
     * @return Iterator to first student (sorted order)
     */
    auto getSortedOrderIterator() const {
        return sortedByName.begin();
    }

    /**
     * @brief End iterator for sorted order
     * @return End iterator (sorted order)
     */
    auto getSortedOrderEnd() const {
        return sortedByName.end();
    }

    /**
     * @brief Find students with grade >= threshold in a specific course
     * Uses map for quick lookup without linear search
     * @param courseCode Course code to search
     * @param minGrade Minimum grade threshold
     * @return Vector of students meeting criteria
     */
    std::vector<StudentPtr> findStudentsByGrade(const CourseCodeType& courseCode, int minGrade) const {
        std::vector<StudentPtr> result;
        for (const auto& student : insertionOrder) {
            int grade = student->getGradeForCourse(courseCode);
            if (grade >= minGrade) {
                result.push_back(student);
            }
        }
        return result;
    }
};

#endif // STUDENT_DATABASE_H
