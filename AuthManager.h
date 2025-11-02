#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <string>
#include <iostream>

// Enumeration for user roles
enum class UserRole {
    ADMIN,
    STUDENT
};

// Authentication and Authorization Manager
class AuthManager {
private:
    UserRole currentRole;
    std::string currentUser;
    
    // Hardcoded credentials (in real system, would be hashed and stored securely)
    const std::string ADMIN_USERNAME = "admin";
    const std::string ADMIN_PASSWORD = "admin123";
    const std::string STUDENT_USERNAME = "student";
    const std::string STUDENT_PASSWORD = "student123";
    
public:
    AuthManager() : currentRole(UserRole::STUDENT), currentUser("") {}
    
    // Login as Admin or Student
    bool login(const std::string& username, const std::string& password, UserRole role) {
        if (role == UserRole::ADMIN) {
            if (username == ADMIN_USERNAME && password == ADMIN_PASSWORD) {
                currentRole = UserRole::ADMIN;
                currentUser = username;
                return true;
            }
        } else if (role == UserRole::STUDENT) {
            if (username == STUDENT_USERNAME && password == STUDENT_PASSWORD) {
                currentRole = UserRole::STUDENT;
                currentUser = username;
                return true;
            }
        }
        return false;
    }
    
    // Logout
    void logout() {
        currentUser = "";
    }
    
    // Get current role
    UserRole getCurrentRole() const {
        return currentRole;
    }
    
    // Get current user
    std::string getCurrentUser() const {
        return currentUser;
    }
    
    // Check if user is admin
    bool isAdmin() const {
        return currentRole == UserRole::ADMIN;
    }
    
    // Check if user is student
    bool isStudent() const {
        return currentRole == UserRole::STUDENT;
    }
    
    // Check if user is authenticated
    bool isAuthenticated() const {
        return !currentUser.empty();
    }
};

#endif // AUTHMANAGER_H
