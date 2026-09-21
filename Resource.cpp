#include "../include/Resource.h"
#include <iostream>
#include <iomanip>

// Default Constructor
Resource::Resource() 
    : resourceID(""), resourceName(""), category(""), status("Available") {}

// Parameterized Constructor
Resource::Resource(std::string ID, std::string name, std::string cat, std::string stat)
    : resourceID(ID), resourceName(name), category(cat), status(stat) {}

// Getters
std::string Resource::getResourceID() const {
    return resourceID;
}

std::string Resource::getResourceName() const {
    return resourceName;
}

std::string Resource::getCategory() const {
    return category;
}

std::string Resource::getStatus() const {
    return status;
}

// Setter
void Resource::setStatus(const std::string& newStatus) {
    status = newStatus;
}

// Formatted Output Display
void Resource::display() const {
    std::cout << std::left 
              << std::setw(12) << resourceID
              << std::setw(25) << resourceName
              << std::setw(18) << category
              << std::setw(15) << status << "\n";
}
