#ifndef CSV_READER_H
#define CSV_READER_H

#include "Student.h"
#include "StudentDatabase.h"
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

/**
 * @class CSVReader
 * @brief Reads student records from CSV file and populates database
 */
template <typename RollNumberType, typename CourseCodeType>
class CSVReader {
private:
    std::string filename;

    /**
     * @brief Parse course string and add to student
     * @param student Student to add courses to
     * @param courseString Course string (e.g., "COL100:Course Name")
     * @param isCurrent Whether this is current course or previous
     */
    void parseCourses(std::shared_ptr<Student<RollNumberType, CourseCodeType>>& student,
                      const std::string& courseString, bool isCurrent) {
        if (courseString.empty()) return;

        std::stringstream ss(courseString);
        std::string item;

        while (std::getline(ss, item, ';')) {
            size_t colonPos = item.find(':');
            if (colonPos != std::string::npos) {
                std::string code = item.substr(0, colonPos);
                std::string rest = item.substr(colonPos + 1);

                if (isCurrent) {
                    // Current course: parse as CourseCodeType and course name
                    CourseCodeType courseCode = convertToType<CourseCodeType>(code);
                    student->addCurrentCourse(courseCode, rest);
                } else {
                    // Previous course: parse code and grade
                    CourseCodeType courseCode = convertToType<CourseCodeType>(code);
                    size_t gradePos = rest.find("Grade-");
                    if (gradePos != std::string::npos) {
                        int grade = std::stoi(rest.substr(gradePos + 6));
                        student->addPreviousCourse(courseCode, grade);
                    }
                }
            }
        }
    }

    /**
     * @brief Convert string to template type
     * Specialization for std::string and int
     */
    template <typename U>
    U convertToType(const std::string& str) {
        if constexpr (std::is_same_v<U, std::string>) {
            return str;
        } else if constexpr (std::is_same_v<U, int>) {
            return std::stoi(str);
        } else if constexpr (std::is_same_v<U, unsigned int>) {
            return static_cast<unsigned int>(std::stoul(str));
        }
    }

public:
    /**
     * @brief Constructor
     * @param file CSV filename
     */
    CSVReader(const std::string& file) : filename(file) {}

    /**
     * @brief Read CSV and populate database
     * CSV format: RollNumber,Name,Branch,StartingYear,CurrentCourses,PreviousCourses
     * @param db Database to populate
     * @return Number of records read
     */
    int readCSV(StudentDatabase<RollNumberType, CourseCodeType>& db) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Error: Cannot open file " << filename << std::endl;
            return 0;
        }

        std::string line;
        int recordCount = 0;

        // Skip header
        std::getline(file, line);

        while (std::getline(file, line)) {
            if (line.empty()) continue;

            std::vector<std::string> fields;
            std::stringstream ss(line);
            std::string field;

            // Parse CSV line
            while (std::getline(ss, field, ',')) {
                fields.push_back(field);
            }

            if (fields.size() < 4) continue;

            try {
                // Parse basic fields
                RollNumberType rollNumber = convertToType<RollNumberType>(fields[0]);
                std::string name = fields[1];
                std::string branch = fields[2];
                int startingYear = std::stoi(fields[3]);

                // Create student
                auto student = std::make_shared<Student<RollNumberType, CourseCodeType>>(
                    name, rollNumber, branch, startingYear
                );

                // Add current courses
                if (fields.size() > 4 && !fields[4].empty()) {
                    parseCourses(student, fields[4], true);
                }

                // Add previous courses
                if (fields.size() > 5 && !fields[5].empty()) {
                    parseCourses(student, fields[5], false);
                }

                db.addStudent(student);
                recordCount++;
            } catch (const std::exception& e) {
                std::cerr << "Error parsing line: " << e.what() << std::endl;
            }
        }

        file.close();
        return recordCount;
    }
};

#endif // CSV_READER_H
