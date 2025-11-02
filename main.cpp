#include "Student.h"
#include "StudentDatabase.h"
#include "AuthManager.h"
#include <iostream>
#include <limits>
#include <iomanip>

// Global databases
IIITStudentDatabase* iitdDatabase = nullptr;
IITStudentDatabase* iitDatabase = nullptr;

void clearInputStream() {
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

void adminMenu(IIITStudentDatabase* db, AuthManager& auth) {
    int choice;
    bool logout = false;

    while (!logout) {
        std::cout << "\n========================================\n";
        std::cout << "     ADMIN MENU\n";
        std::cout << "========================================\n";
        std::cout << "1. Load student data from CSV\n";
        std::cout << "2. Add new student\n";
        std::cout << "3. Update student information\n";
        std::cout << "4. Perform parallel sorting\n";
        std::cout << "5. Find students by grade in course\n";
        std::cout << "6. Display all students (insertion order)\n";
        std::cout << "7. Display sorted students\n";
        std::cout << "8. Save students to CSV\n";
        std::cout << "9. Save sorted students to CSV\n";
        std::cout << "10. Logout\n";
        std::cout << "========================================\n";
        std::cout << "Enter your choice: ";

        if (!(std::cin >> choice)) {
            clearInputStream();
            std::cout << "Invalid choice!\n";
            continue;
        }

        switch (choice) {
            case 1: {
                std::string filename;
                std::cout << "Enter CSV filename: ";
                clearInputStream();
                std::getline(std::cin, filename);
                std::cout << "Loading students from " << filename << "...\n";
                db->readFromCSV(filename);
                break;
            }

            case 2: {
                std::string rollNo, name, branch, yearStr;
                std::cout << "\n--- ADD NEW STUDENT ---\n";
                std::cout << "Roll Number: ";
                clearInputStream();
                std::getline(std::cin, rollNo);
                std::cout << "Name: ";
                std::getline(std::cin, name);
                std::cout << "Branch: ";
                std::getline(std::cin, branch);
                std::cout << "Starting Year: ";
                std::getline(std::cin, yearStr);

                try {
                    int year = std::stoi(yearStr);
                    Student<std::string, std::string> newStudent(rollNo, name, branch, year);
                    db->addStudent(newStudent);
                    std::cout << "Student added successfully!\n";
                } catch (...) {
                    std::cout << "Error: Invalid input!\n";
                }
                break;
            }

            case 3: {
                std::string rollNo, name, branch;
                std::cout << "\n--- UPDATE STUDENT ---\n";
                std::cout << "Enter roll number of student to update: ";
                clearInputStream();
                std::getline(std::cin, rollNo);

                auto student = db->findStudent(rollNo);
                if (student) {
                    std::cout << "Current Name: " << student->getName() << "\n";
                    std::cout << "Enter new name (or press Enter to skip): ";
                    std::getline(std::cin, name);
                    if (!name.empty()) {
                        student->setName(name);
                    }

                    std::cout << "Current Branch: " << student->getBranch() << "\n";
                    std::cout << "Enter new branch (or press Enter to skip): ";
                    std::getline(std::cin, branch);
                    if (!branch.empty()) {
                        student->setBranch(branch);
                    }

                    std::cout << "Student updated successfully!\n";
                } else {
                    std::cout << "Student not found!\n";
                }
                break;
            }

            case 4: {
                if (db->getTotalStudents() == 0) {
                    std::cout << "No students to sort! Please load data first.\n";
                } else {
                    db->parallelSort();
                    db->buildCourseGradeIndex();
                }
                break;
            }

            case 5: {
                std::string courseCode;
                std::cout << "\n--- FIND STUDENTS BY GRADE ---\n";
                std::cout << "Enter course name: ";
                clearInputStream();
                std::getline(std::cin, courseCode);

                const auto* students = db->getStudentsByGradeAndCourse(courseCode);
                if (students && !students->empty()) {
                    std::cout << "Found " << students->size() << " students with grade >= 9 in " 
                              << courseCode << ":\n";
                    for (const auto* student : *students) {
                        std::cout << "  - " << student->getName() << " (" 
                                  << student->getRollNumber() << ")\n";
                    }
                } else {
                    std::cout << "No students found with grade >= 9 in " << courseCode << "\n";
                }
                break;
            }

            case 6: {
                std::cout << "\n--- ALL STUDENTS (INSERTION ORDER) ---\n";
                const auto& students = db->getStudentsInsertionOrder();
                int total = students.size();
                
                if (total == 0) {
                    std::cout << "No students in database!\n";
                } else {
                    int displayCount = std::min(10, total);
                    std::cout << "Displaying first " << displayCount << " students (Total: " 
                              << total << ")\n";
                    
                    for (int i = 0; i < displayCount; i++) {
                        const auto& student = students[i];
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
                break;
            }

            case 7: {
                db->displaySorted(10);
                break;
            }

            case 8: {
                std::string filename;
                std::cout << "\n--- SAVE STUDENTS TO CSV ---\n";
                std::cout << "Enter CSV filename (default: students_output.csv): ";
                clearInputStream();
                std::getline(std::cin, filename);
                if (filename.empty()) {
                    filename = "students_output.csv";
                }
                db->saveToCSV(filename);
                break;
            }

            case 9: {
                std::string filename;
                std::cout << "\n--- SAVE SORTED STUDENTS TO CSV ---\n";
                std::cout << "Enter CSV filename (default: students_sorted.csv): ";
                clearInputStream();
                std::getline(std::cin, filename);
                if (filename.empty()) {
                    filename = "students_sorted.csv";
                }
                db->saveSortedToCSV(filename);
                break;
            }

            case 10: {
                auth.logout();
                std::cout << "Logged out successfully!\n";
                logout = true;
                break;
            }

            default: {
                std::cout << "Invalid choice!\n";
                break;
            }
        }
    }
}

void studentMenu(IIITStudentDatabase* db, AuthManager& auth) {
    int choice;
    bool logout = false;

    while (!logout) {
        std::cout << "\n========================================\n";
        std::cout << "     STUDENT MENU (READ-ONLY)\n";
        std::cout << "========================================\n";
        std::cout << "1. View students (insertion order)\n";
        std::cout << "2. View students (sorted order)\n";
        std::cout << "3. Search student by roll number\n";
        std::cout << "4. Logout\n";
        std::cout << "========================================\n";
        std::cout << "Enter your choice: ";

        if (!(std::cin >> choice)) {
            clearInputStream();
            std::cout << "Invalid choice!\n";
            continue;
        }

        switch (choice) {
            case 1: {
                std::cout << "\n--- STUDENTS (INSERTION ORDER) ---\n";
                const auto& students = db->getStudentsInsertionOrder();
                int total = students.size();
                
                if (total == 0) {
                    std::cout << "No students in database!\n";
                } else {
                    int displayCount = std::min(20, total);
                    std::cout << "Displaying first " << displayCount << " students (Total: " 
                              << total << ")\n";
                    
                    for (int i = 0; i < displayCount; i++) {
                        const auto& student = students[i];
                        std::cout << i + 1 << ". " << student.getRollNumber() 
                                  << " - " << student.getName() 
                                  << " (" << student.getBranch() << ")\n";
                    }
                }
                break;
            }

            case 2: {
                std::cout << "\n--- STUDENTS (SORTED ORDER) ---\n";
                const auto& students = db->getSortedStudents();
                int total = students.size();
                
                if (total == 0) {
                    std::cout << "No sorted data! Admin must perform sorting first.\n";
                } else {
                    int displayCount = std::min(20, total);
                    std::cout << "Displaying first " << displayCount << " students (Total: " 
                              << total << ")\n";
                    
                    for (int i = 0; i < displayCount; i++) {
                        const auto& student = students[i];
                        std::cout << i + 1 << ". " << student.getRollNumber() 
                                  << " - " << student.getName() 
                                  << " (" << student.getBranch() << ")\n";
                    }
                }
                break;
            }

            case 3: {
                std::string rollNo;
                std::cout << "\n--- SEARCH STUDENT ---\n";
                std::cout << "Enter roll number: ";
                clearInputStream();
                std::getline(std::cin, rollNo);

                auto student = db->findStudent(rollNo);
                if (student) {
                    std::cout << "\nStudent Found:\n";
                    std::cout << "Roll No: " << student->getRollNumber() << "\n";
                    std::cout << "Name: " << student->getName() << "\n";
                    std::cout << "Branch: " << student->getBranch() << "\n";
                    std::cout << "Starting Year: " << student->getStartingYear() << "\n";
                    
                    std::cout << "Current Courses: ";
                    for (const auto& course : student->getCurrentCourses()) {
                        std::cout << course << " ";
                    }
                    std::cout << "\n";
                    
                    std::cout << "Previous Courses: ";
                    for (const auto& [course, grade] : student->getPreviousCourses()) {
                        std::cout << course << ":" << grade << " ";
                    }
                    std::cout << "\n";
                } else {
                    std::cout << "Student not found!\n";
                }
                break;
            }

            case 4: {
                auth.logout();
                std::cout << "Logged out successfully!\n";
                logout = true;
                break;
            }

            default: {
                std::cout << "Invalid choice!\n";
                break;
            }
        }
    }
}

void loginMenu(AuthManager& auth) {
    int choice;
    std::string username, password;

    std::cout << "\n--- LOGIN MENU ---\n";
    std::cout << "1. Login as Admin\n";
    std::cout << "2. Login as Professor\n";
    std::cout << "========================================\n";
    std::cout << "Enter your choice: ";

    if (!(std::cin >> choice)) {
        clearInputStream();
        std::cout << "Invalid choice!\n";
        return;
    }

    UserRole role;

    switch (choice) {
        case 1: {
            std::cout << "\n--- ADMIN LOGIN ---\n";
            std::cout << "Username: ";
            clearInputStream();
            std::getline(std::cin, username);
            std::cout << "Password: ";
            std::getline(std::cin, password);

            if (auth.login(username, password, role)) {
                std::cout << "Login successful! Welcome Admin.\n";
                adminMenu(iitdDatabase, auth);
            } else {
                std::cout << "Login failed! Invalid credentials.\n";
            }
            break;
        }

        case 2: {
            std::cout << "\n--- STUDENT LOGIN ---\n";
            std::cout << "Username: ";
            clearInputStream();
            std::getline(std::cin, username);
            std::cout << "Password: ";
            std::getline(std::cin, password);

            if (auth.login(username, password, role)) {
                std::cout << "Login successful! Welcome Student.\n";
                studentMenu(iitdDatabase, auth);
            } else {
                std::cout << "Login failed! Invalid credentials.\n";
            }
            break;
        }

        default: {
            std::cout << "Invalid choice!\n";
            break;
        }
    }
}

int main() {
    iitdDatabase = new IIITStudentDatabase();
    iitDatabase = new IITStudentDatabase();
    AuthManager authManager;

    int choice;
    bool exit_program = false;

    while (!exit_program) {
        std::cout << "\n========================================\n";
        std::cout << "   Welcome to University ERP System\n";
        std::cout << "========================================\n\n";
        std::cout << "========================================\n";
        std::cout << "     UNIVERSITY ERP SYSTEM\n";
        std::cout << "========================================\n";
        std::cout << "1. Login as Admin\n";
        std::cout << "2. Login as Student\n";
        std::cout << "3. Exit\n";
        std::cout << "========================================\n";
        std::cout << "Enter your choice: ";

        if (!(std::cin >> choice)) {
            clearInputStream();
            std::cout << "Invalid choice!\n";
            continue;
        }

        switch (choice) {
            case 1: {
                std::string username, password;
                std::cout << "\n--- ADMIN LOGIN ---\n";
                std::cout << "Username: ";
                clearInputStream();
                std::getline(std::cin, username);
                std::cout << "Password: ";
                std::getline(std::cin, password);

                UserRole role;
                if (authManager.login(username, password, role)) {
                    std::cout << "Login successful! Welcome Admin.\n";
                    adminMenu(iitdDatabase, authManager);
                } else {
                    std::cout << "Login failed! Invalid credentials.\n";
                }
                break;
            }

            case 2: {
                std::string username, password;
                std::cout << "\n--- STUDENT LOGIN ---\n";
                std::cout << "Username: ";
                clearInputStream();
                std::getline(std::cin, username);
                std::cout << "Password: ";
                std::getline(std::cin, password);

                UserRole role;
                if (authManager.login(username, password, role)) {
                    std::cout << "Login successful! Welcome Professor.\n";
                    studentMenu(iitdDatabase, authManager);
                } else {
                    std::cout << "Login failed! Invalid credentials.\n";
                }
                break;
            }

            case 3: {
                std::cout << "Thank you for using University ERP System!\n";
                exit_program = true;
                break;
            }

            default: {
                std::cout << "Invalid choice!\n";
                break;
            }
        }
    }

    delete iitdDatabase;
    delete iitDatabase;
    return 0;
}
