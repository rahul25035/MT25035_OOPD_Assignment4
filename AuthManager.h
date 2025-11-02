#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <string>

enum class UserRole {
    NONE,
    ADMIN,
    STUDENT
};

class AuthManager {
private:
    UserRole currentRole;
    bool isLoggedIn;
    std::string currentUsername;

    // Hardcoded credentials (in production, use hashed storage)
    const std::string ADMIN_USERNAME = "admin";
    const std::string ADMIN_PASSWORD = "admin123";
    const std::string STUDENT_USERNAME = "student";
    const std::string STUDENT_PASSWORD = "student123";

public:
    AuthManager() : currentRole(UserRole::NONE), isLoggedIn(false), currentUsername("") {}

    // Login method
    bool login(const std::string& username, const std::string& password, UserRole& role) {
        if (username == ADMIN_USERNAME && password == ADMIN_PASSWORD) {
            currentRole = UserRole::ADMIN;
            isLoggedIn = true;
            currentUsername = username;
            role = UserRole::ADMIN;
            return true;
        } else if (username == STUDENT_USERNAME && password == STUDENT_PASSWORD) {
            currentRole = UserRole::STUDENT;
            isLoggedIn = true;
            currentUsername = username;
            role = UserRole::STUDENT;
            return true;
        }
        return false;
    }

    // Logout method
    void logout() {
        currentRole = UserRole::NONE;
        isLoggedIn = false;
        currentUsername = "";
    }

    // Check if authenticated
    bool isAuthenticated() const {
        return isLoggedIn;
    }

    // Check if admin
    bool isAdmin() const {
        return currentRole == UserRole::ADMIN;
    }

    // Check if student
    bool isStudent() const {
        return currentRole == UserRole::STUDENT;
    }

    // Get current username
    std::string getCurrentUsername() const {
        return currentUsername;
    }

    // Get current role
    UserRole getCurrentRole() const {
        return currentRole;
    }
};

#endif // AUTHMANAGER_H
