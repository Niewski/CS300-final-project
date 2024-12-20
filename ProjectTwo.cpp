/*
 * ProjectTwo.cpp
 *
 *  Date: 12/15/2024
 *  Author: Jacob Wisniewski
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

// Course structure to store course information including course number, title, and prerequisites
struct Course {
    std::string courseNumber;
    std::string courseTitle;
    std::vector<std::string> prerequisites;

    Course() {}

    Course(const std::string& number, const std::string& title, const std::vector<std::string>& prereqs)
        : courseNumber(number), courseTitle(title), prerequisites(prereqs) {
    }
};

// TreeNode structure to represent nodes in the binary search tree
struct TreeNode {
    Course course;
    TreeNode* left;
    TreeNode* right;

    // Default constructor
    TreeNode() {
        left = nullptr;
        right = nullptr;
    }

    // Constructor to initialize node with a course
    TreeNode(Course aCourse) :
        TreeNode() {
        course = aCourse;
    }
};

// Binary Search Tree class to manage course data
class CourseBST {
private: 
    // Root node of the binary search tree
    TreeNode* root;

    // Helper function to recursively add a node to the tree
    void addNode(TreeNode* node, const Course& course);
    // Helper function for in-order traversal
    void inOrder(TreeNode* node) const;
    // Helper function to recursively remove a node from the tree
    TreeNode* removeNode(TreeNode* node, const std::string& courseNumber);
    // Helper function to search for a node by course number
    Course searchNode(TreeNode* node, const std::string& courseNumber) const;
    // Validate that all prerequisites exist in the tree
    void validatePrerequisites(TreeNode* node);
    // Check if a course exists in the tree
    bool courseExists(const std::string& courseNumber) const;
    // Helper function to clear the entire tree
    void clearTree(TreeNode*& node);

public:
    CourseBST();
    virtual ~CourseBST();

    void insert(const Course& course);
    void remove(const std::string& courseNumber);
    // Search for a course by course number
    Course search(const std::string& courseNumber) const; 
    // Load course data from a file
    void loadCourseData(const std::string& filename); 
    // Print all courses in alphabetical order
    void printAllCourses() const;         
    // Print details of a single course
    void printCourse(const Course& course) const; 

};

CourseBST::CourseBST() : root(nullptr) {}

CourseBST::~CourseBST() {
    while (root != nullptr) {
        remove(root->course.courseNumber);
    }
}

void CourseBST::insert(const Course& course) {
    if (root == nullptr) {
        root = new TreeNode(course);
    }
    else {
        addNode(root, course);
    }
}

// Recursive function to add a node to the tree
void CourseBST::addNode(TreeNode* node, const Course& course) {
    if (course.courseNumber < node->course.courseNumber) {
        if (node->left == nullptr) {
            node->left = new TreeNode(course);
        }
        else {
            addNode(node->left, course);
        }
    }
    else {
        if (node->right == nullptr) {
            node->right = new TreeNode(course);
        }
        else {
            addNode(node->right, course);
        }
    }
}

// Recursive in-order traversal to print courses in alphabetical order
void CourseBST::inOrder(TreeNode* node) const {
    if (node != nullptr) {
        inOrder(node->left);
		printCourse(node->course);
        inOrder(node->right);
    }
}

TreeNode* CourseBST::removeNode(TreeNode* node, const std::string& courseNumber) {
    if (node == nullptr) {
        return node;
    }

    if (courseNumber < node->course.courseNumber) {
        node->left = removeNode(node->left, courseNumber);
    }
    else if (courseNumber > node->course.courseNumber) {
        node->right = removeNode(node->right, courseNumber);
    }
    else {
        if (node->left == nullptr && node->right == nullptr) {
            delete node;
            node = nullptr;
        }
        else if (node->left == nullptr) {
            TreeNode* temp = node;
            node = node->right;
            delete temp;
        }
        else if (node->right == nullptr) {
            TreeNode* temp = node;
            node = node->left;
            delete temp;
        }
        else {
            TreeNode* temp = node->right;
            while (temp->left != nullptr) {
                temp = temp->left;
            }
            node->course = temp->course;
            node->right = removeNode(node->right, temp->course.courseNumber);
        }
    }
    return node;
}

void CourseBST::remove(const std::string& courseNumber) {
    root = removeNode(root, courseNumber);
}

Course CourseBST::search(const std::string& courseNumber) const {
    return searchNode(root, courseNumber);
}

Course CourseBST::searchNode(TreeNode* node, const std::string& courseNumber) const {
    if (node == nullptr) {
        throw std::runtime_error("Course not found");
    }

    if (courseNumber == node->course.courseNumber) {
        return node->course;
    }
    else if (courseNumber < node->course.courseNumber) {
        return searchNode(node->left, courseNumber);
    }
    else {
        return searchNode(node->right, courseNumber);
    }
}

void CourseBST::loadCourseData(const std::string& filename) {
    clearTree(root);
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Error: Could not open file " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::vector<std::string> fields;
        std::string field;
        while (std::getline(ss, field, ',')) {
            fields.push_back(field);
        }

        if (fields.size() < 2) {
            clearTree(root);
            throw std::runtime_error("Error: Invalid course data format in line: " + line);
        }

        std::string courseNumber = fields[0];
        std::string courseTitle = fields[1];
        std::vector<std::string> prerequisites;

        // Add prerequisites, skipping empty strings
        for (size_t i = 2; i < fields.size(); ++i) {
            if (!fields[i].empty()) {
                prerequisites.push_back(fields[i]);
            }
        }

        insert(Course(courseNumber, courseTitle, prerequisites));
    }
    file.close();

    try {
        validatePrerequisites(root);
    }
    catch (const std::runtime_error& e) {
        std::cerr << e.what() << "\nClearing tree due to validation failure.\n";
        clearTree(root);
    }
}

void CourseBST::validatePrerequisites(TreeNode* node) {
    if (node != nullptr) {
        for (const auto& prereq : node->course.prerequisites) {
            if (!courseExists(prereq)) {
                throw std::runtime_error("Error: Prerequisite " + prereq + " for course " + node->course.courseNumber + " not found.");
            }
        }
        validatePrerequisites(node->left);
        validatePrerequisites(node->right);
    }
}

bool CourseBST::courseExists(const std::string& courseNumber) const {
    try {
        search(courseNumber);
        return true;
    }
    catch (...) {
        return false;
    }
}

void CourseBST::clearTree(TreeNode*& node) {
    if (node != nullptr) {
        clearTree(node->left);
        clearTree(node->right);
        delete node;
        node = nullptr;
    }
}

void CourseBST::printAllCourses() const {
    inOrder(root);
}

void CourseBST::printCourse(const Course& course) const {
    std::cout << course.courseNumber << ": " << course.courseTitle << "\n";
    if (!course.prerequisites.empty()) {
        std::cout << "Prerequisites: ";
        for (size_t i = 0; i < course.prerequisites.size(); ++i) {
            std::cout << course.prerequisites[i];
            if (i < course.prerequisites.size() - 1) {
                std::cout << ", ";
            }
        }
        std::cout << "\n";
    }
    else {
        std::cout << "No prerequisites.\n";
    }
}


void displayMenu() {
    std::cout << "\n1. Load Data\n";
    std::cout << "2. Print Alphanumeric List of All Courses\n";
    std::cout << "3. Print Course Information\n";
    std::cout << "9. Exit\n\n";
    std::cout << "What would you like to do? ";
}

int main() {
    CourseBST courseTree;
    bool coursesLoaded = false;
    int choice = 0;
    std::cout << "Welcome to the course planner.";

    while (choice != 9) {
        displayMenu();

        // Read input as a string to validate it
        std::string input;
        std::cin >> input;

        // Validate that the input is a number
        try {
            choice = std::stoi(input); // Convert input to an integer

            switch (choice) {
            case 1: {
                std::string filename;
                std::cout << "Enter the file name: ";
                std::cin >> filename;
                try {
                    courseTree.loadCourseData(filename);
                    coursesLoaded = true;
                    std::cout << "Courses loaded successfully.\n";
                }
                catch (const std::exception& e) {
                    coursesLoaded = false;
                    std::cerr << e.what() << "\n";
                }
                break;
            }
            case 2: {
                if (!coursesLoaded) {
                    std::cout << "No courses have been loaded. Please load data first.\n";
                }
                else {
                    courseTree.printAllCourses();
                }
                break;
            }
            case 3: {
                if (!coursesLoaded) {
                    std::cout << "No courses have been loaded. Please load data first.\n";
                }
                else {
                    std::string courseNumber;
                    std::cout << "Enter the course number: ";
                    std::cin >> courseNumber;
                    try {
                        Course course = courseTree.search(courseNumber);
                        courseTree.printCourse(course);
                    }
                    catch (const std::exception& e) {
                        std::cout << e.what() << "\n";
                    }
                }
                break;
            }
            case 9:
                std::cout << "Thank you for using the course planner!\n";
                break;
            default:
                std::cout << "Invalid choice. Please try again.\n";
                break;
            }
        }
        catch (const std::invalid_argument&) {
            // Handle non-numeric input
            std::cout << "Invalid input. Please enter a number.\n";
        }
        catch (const std::out_of_range&) {
			// Handle numbers that do not fit in an int
            std::cout << "Input is out of range. Please enter a valid number.\n";
        }
    }

    return 0;
}