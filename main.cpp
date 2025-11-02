#include "Student.h"
#include "StudentDatabase.h"
#include "AuthManager.h"
#include <iostream>
#include <string>
#include <cctype>
#include <iterator>

// Namespace for utility functions
namespace Utils {
    std::string toLower(const std::string& str) {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    }
    
    void clearInput() {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }
}

// Global databases and auth manager
IIITStudentDatabase iiitDB;
IITStudentDatabase iitDB;
AuthManager authManager;

// Display main menu
void displayMainMenu() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "     UNIVERSITY ERP SYSTEM" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "1. Login as Admin" << std::endl;
    std::cout << "2. Login as Student" << std::endl;
    std::cout << "3. Exit" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Enter your choice: ";
}

// Admin menu
void displayAdminMenu() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "     ADMIN MENU" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "1. Load student data from CSV" << std::endl;
    std::cout << "2. Add new student" << std::endl;
    std::cout << "3. Update student information" << std::endl;
    std::cout << "4. Perform parallel sorting" << std::endl;
    std::cout << "5. Find students by grade in course" << std::endl;
    std::cout << "6. Display all students" << std::endl;
    std::cout << "7. Logout" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Enter your choice: ";
}

// Student menu
void displayStudentMenu() {
    std::cout << "\n========================================" << std::endl;
    std::cout << "     STUDENT MENU (VIEW ONLY)" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "1. View all students (insertion order)" << std::endl;
    std::cout << "2. View sorted students" << std::endl;
    std::cout << "3. Search student by roll number" << std::endl;
    std::cout << "4. Logout" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << "Enter your choice: ";
}

// Admin login
void adminLogin() {
    std::string username, password;
    std::cout << "\n--- ADMIN LOGIN ---" << std::endl;
    std::cout << "Username: ";
    std::getline(std::cin, username);
    std::cout << "Password: ";
    std::getline(std::cin, password);
    
    if (authManager.login(username, password, UserRole::ADMIN)) {
        std::cout << "Login successful! Welcome Admin." << std::endl;
    } else {
        std::cout << "Invalid credentials!" << std::endl;
    }
}

// Student login
void studentLogin() {
    std::string username, password;
    std::cout << "\n--- STUDENT LOGIN ---" << std::endl;
    std::cout << "Username: ";
    std::getline(std::cin, username);
    std::cout << "Password: ";
    std::getline(std::cin, password);
    
    if (authManager.login(username, password, UserRole::STUDENT)) {
        std::cout << "Login successful! Welcome Student." << std::endl;
    } else {
        std::cout << "Invalid credentials!" << std::endl;
    }
}

// Admin: Load CSV
void adminLoadCSV() {
    std::string filename;
    std::cout << "Enter CSV filename: ";
    std::getline(std::cin, filename);
    
    std::cout << "Loading students from " << filename << "..." << std::endl;
    iiitDB.readFromCSV(filename);
    std::cout << "Successfully loaded " << iiitDB.getStudentCount() << " students!" << std::endl;
}

// Admin: Add new student
void adminAddStudent() {
    std::string rollNo, name, branch, yearStr;
    
    std::cout << "\n--- ADD NEW STUDENT ---" << std::endl;
    std::cout << "Roll Number: ";
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
        iiitDB.addStudent(newStudent);
        std::cout << "Student added successfully!" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Error: Invalid input format!" << std::endl;
    }
}

// Admin: Update student
void adminUpdateStudent() {
    std::string rollNo, name, branch, yearStr;
    
    std::cout << "\n--- UPDATE STUDENT ---" << std::endl;
    std::cout << "Roll Number to update: ";
    std::getline(std::cin, rollNo);
    std::cout << "New Name: ";
    std::getline(std::cin, name);
    std::cout << "New Branch: ";
    std::getline(std::cin, branch);
    std::cout << "New Starting Year: ";
    std::getline(std::cin, yearStr);
    
    try {
        int year = std::stoi(yearStr);
        if (iiitDB.updateStudent(rollNo, name, branch, year)) {
            std::cout << "Student updated successfully!" << std::endl;
        } else {
            std::cout << "Student not found!" << std::endl;
        }
    } catch (const std::exception& e) {
        std::cout << "Error: Invalid input format!" << std::endl;
    }
}

// Admin: Parallel sort
void adminParallelSort() {
    std::cout << "\nPerforming parallel sorting..." << std::endl;
    iiitDB.parallelSort();
    std::cout << "Sorting completed!" << std::endl;
}

// Admin: Find students by grade
void adminFindByGrade() {
    std::string course;
    std::cout << "\n--- FIND STUDENTS BY GRADE ---" << std::endl;
    std::cout << "Enter course name: ";
    std::getline(std::cin, course);
    
    // First build the index if not done
    iiitDB.buildCourseGradeIndex();
    
    auto students = iiitDB.getStudentsByGradeAndCourse(course, 9);
    
    if (students.empty()) {
        std::cout << "No students found with grade >= 9 in " << course << std::endl;
    } else {
        std::cout << "Found " << students.size() << " students with grade >= 9 in " << course << ":" << std::endl;
        for (const auto& student : students) {
            std::cout << "  - " << student->getName() << " (" << student->getRollNumber() << ")" << std::endl;
        }
    }
}

// Admin: Display all students
void adminDisplayAll() {
    std::cout << "\n--- ALL STUDENTS ---" << std::endl;
    const auto& students = iiitDB.getStudentsInsertionOrder();
    if (students.empty()) {
        std::cout << "No students in database." << std::endl;
    } else {
        std::cout << "Displaying first 10 students (Total: " << students.size() << ")" << std::endl;
        for (size_t i = 0; i < std::min(students.size(), size_t(10)); ++i) {
            students[i].display();
        }
    }
}

// Admin menu handler
void adminMenuHandler() {
    while (authManager.isAuthenticated() && authManager.isAdmin()) {
        displayAdminMenu();
        std::string choice;
        std::getline(std::cin, choice);
        
        if (choice == "1") {
            adminLoadCSV();
        } else if (choice == "2") {
            adminAddStudent();
        } else if (choice == "3") {
            adminUpdateStudent();
        } else if (choice == "4") {
            adminParallelSort();
        } else if (choice == "5") {
            adminFindByGrade();
        } else if (choice == "6") {
            adminDisplayAll();
        } else if (choice == "7") {
            authManager.logout();
            std::cout << "Logged out successfully!" << std::endl;
        } else {
            std::cout << "Invalid choice!" << std::endl;
        }
    }
}

// Student: View insertion order
void studentViewInsertionOrder() {
    std::cout << "\n--- STUDENTS (INSERTION ORDER) ---" << std::endl;
    const auto& students = iiitDB.getStudentsInsertionOrder();
    if (students.empty()) {
        std::cout << "No students in database." << std::endl;
    } else {
        std::cout << "Displaying first 10 students (Total: " << students.size() << ")" << std::endl;
        for (size_t i = 0; i < std::min(students.size(), size_t(10)); ++i) {
            students[i].display();
        }
    }
}

// Student: View sorted
void studentViewSorted() {
    std::cout << "\n--- STUDENTS (SORTED BY ROLL NUMBER) ---" << std::endl;
    const auto& students = iiitDB.getSortedStudents();
    if (students.empty()) {
        std::cout << "Students not yet sorted. Please ask admin to perform sorting." << std::endl;
    } else {
        std::cout << "Displaying first 10 students (Total: " << students.size() << ")" << std::endl;
        for (size_t i = 0; i < std::min(students.size(), size_t(10)); ++i) {
            students[i].display();
        }
    }
}

// Student: Search student
void studentSearch() {
    std::string rollNo;
    std::cout << "\n--- SEARCH STUDENT ---" << std::endl;
    std::cout << "Enter roll number: ";
    std::getline(std::cin, rollNo);
    
    auto student = iiitDB.findStudent(rollNo);
    if (student) {
        student->display();
    } else {
        std::cout << "Student not found!" << std::endl;
    }
}

// Student menu handler
void studentMenuHandler() {
    while (authManager.isAuthenticated() && authManager.isStudent()) {
        displayStudentMenu();
        std::string choice;
        std::getline(std::cin, choice);
        
        if (choice == "1") {
            studentViewInsertionOrder();
        } else if (choice == "2") {
            studentViewSorted();
        } else if (choice == "3") {
            studentSearch();
        } else if (choice == "4") {
            authManager.logout();
            std::cout << "Logged out successfully!" << std::endl;
        } else {
            std::cout << "Invalid choice!" << std::endl;
        }
    }
}

// Main function
int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "   Welcome to University ERP System" << std::endl;
    std::cout << "========================================" << std::endl;
    
    bool running = true;
    while (running) {
        displayMainMenu();
        std::string choice;
        std::getline(std::cin, choice);
        
        if (choice == "1") {
            adminLogin();
            if (authManager.isAuthenticated()) {
                adminMenuHandler();
            }
        } else if (choice == "2") {
            studentLogin();
            if (authManager.isAuthenticated()) {
                studentMenuHandler();
            }
        } else if (choice == "3") {
            std::cout << "Thank you for using University ERP System. Goodbye!" << std::endl;
            running = false;
        } else {
            std::cout << "Invalid choice!" << std::endl;
        }
    }
    
    return 0;
}
