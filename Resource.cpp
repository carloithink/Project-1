#include "include/Resource.h"

#include <iomanip>
#include <iostream>

Resource::Resource()
    : id(""), name(""), type(""), available(true), reservationCount(0) {}

Resource::Resource(std::string resourceId, std::string resourceName, std::string resourceType, bool isAvailable)
    : id(std::move(resourceId)), name(std::move(resourceName)), type(std::move(resourceType)), available(isAvailable), reservationCount(0) {}

std::string Resource::getId() const {
    return id;
}

std::string Resource::getName() const {
    return name;
}

std::string Resource::getType() const {
    return type;
}

bool Resource::getAvailability() const {
    return available;
}

int Resource::getReservationCount() const {
    return reservationCount;
}

void Resource::setAvailability(bool isAvailable) {
    available = isAvailable;
}

void Resource::incrementReservationCount() {
    ++reservationCount;
}

void Resource::setStatus(const std::string& newStatus) {
    if (newStatus == "Available" || newStatus == "available" || newStatus == "1") {
        available = true;
    } else if (newStatus == "Reserved" || newStatus == "reserved" || newStatus == "Unavailable" || newStatus == "unavailable" || newStatus == "0") {
        available = false;
    }
}

void Resource::display() const {
    std::cout << std::left
              << std::setw(12) << id
              << std::setw(25) << name
              << std::setw(18) << type
              << std::setw(15) << (available ? "Available" : "Reserved") << "\n";
}
