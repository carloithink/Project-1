#include "ReservationManager.h"

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>

ReservationManager::ReservationManager() : nextReservationId(101) {}

bool ReservationManager::loadResources(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cerr << "[Error] Unable to open resource file: " << filename << std::endl;
        return false;
    }

    resources.clear();
    std::string line;

    while (std::getline(inFile, line)) {
        if (line.empty()) {
            continue;
        }

        std::stringstream ss(line);
        std::string id, name, type, availabilityToken;
        if (std::getline(ss, id, ',') &&
            std::getline(ss, name, ',') &&
            std::getline(ss, type, ',') &&
            std::getline(ss, availabilityToken, ',')) {
            const bool isAvailable = availabilityToken == "1" || availabilityToken == "true" || availabilityToken == "True";
            resources.emplace_back(id, name, type, isAvailable);
        }
    }

    inFile.close();
    return true;
}

bool ReservationManager::loadReservations(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cerr << "[Error] Unable to open reservation file: " << filename << std::endl;
        return false;
    }

    activeReservations.clear();
    std::string line;
    int maxId = 100;

    while (std::getline(inFile, line)) {
        if (line.empty()) {
            continue;
        }

        std::stringstream ss(line);
        std::string reservationIdToken, studentId, studentName, resourceId, date;
        if (std::getline(ss, reservationIdToken, ',') &&
            std::getline(ss, studentId, ',') &&
            std::getline(ss, studentName, ',') &&
            std::getline(ss, resourceId, ',') &&
            std::getline(ss, date, ',')) {
            const int reservationId = std::stoi(reservationIdToken);
            activeReservations.emplace_back(reservationId, studentId, studentName, resourceId, date);
            maxId = std::max(maxId, reservationId);

            const int resourceIndex = binarySearchResource(resourceId);
            if (resourceIndex != -1) {
                resources[resourceIndex].setAvailability(false);
                resources[resourceIndex].incrementReservationCount();
            }
        }
    }

    nextReservationId = maxId + 1;
    inFile.close();
    return true;
}

void ReservationManager::createReservation(const std::string& studentId,
                                          const std::string& studentName,
                                          const std::string& resourceId,
                                          const std::string& date) {
    const int resourceIndex = binarySearchResource(resourceId);
    if (resourceIndex == -1) {
        std::cout << "[Error] Resource ID '" << resourceId << "' does not exist.\n";
        return;
    }

    Resource& resource = resources[resourceIndex];
    if (resource.getAvailability()) {
        const int assignedId = nextReservationId++;
        activeReservations.emplace_back(assignedId, studentId, studentName, resourceId, date);
        resource.setAvailability(false);
        resource.incrementReservationCount();
        std::cout << "[Success] Reservation Created Successfully. Reservation ID: " << assignedId << "\n";
        return;
    }

    waitingLists[resourceId].push(Reservation(0, studentId, studentName, resourceId, date));
    std::cout << "[Waitlist] Resource '" << resourceId
              << "' is currently unavailable. Student " << studentName
              << " added to waitlist (Position: " << waitingLists[resourceId].size() << ").\n";
}

bool ReservationManager::cancelReservation(int reservationId) {
    for (auto it = activeReservations.begin(); it != activeReservations.end(); ++it) {
        if (it->getReservationId() == reservationId) {
            const Reservation cancelledRes = *it;
            const std::string resourceId = cancelledRes.getResourceId();

            activeReservations.erase(it);
            cancellationHistory.push(cancelledRes);

            if (waitingLists.count(resourceId) > 0 && !waitingLists[resourceId].empty()) {
                Reservation nextInLine = waitingLists[resourceId].front();
                waitingLists[resourceId].pop();

                const int assignedId = nextReservationId++;
                activeReservations.emplace_back(assignedId, nextInLine.getStudentId(), nextInLine.getStudentName(), resourceId, nextInLine.getReservationDate());

                const int resourceIndex = binarySearchResource(resourceId);
                if (resourceIndex != -1) {
                    resources[resourceIndex].incrementReservationCount();
                    resources[resourceIndex].setAvailability(false);
                }

                std::cout << "[Waitlist Promoted] Resource automatically assigned to waiting student: "
                          << nextInLine.getStudentName() << " (New Reservation ID: " << assignedId << ").\n";
            } else {
                const int resourceIndex = binarySearchResource(resourceId);
                if (resourceIndex != -1) {
                    resources[resourceIndex].setAvailability(true);
                }
            }

            return true;
        }
    }

    return false;
}

bool ReservationManager::undoCancellation() {
    if (cancellationHistory.empty()) {
        return false;
    }

    const Reservation restoredRes = cancellationHistory.top();
    cancellationHistory.pop();
    const std::string resourceId = restoredRes.getResourceId();

    activeReservations.push_back(restoredRes);
    const int resourceIndex = binarySearchResource(resourceId);
    if (resourceIndex != -1) {
        resources[resourceIndex].setAvailability(false);
    }

    std::cout << "[Undo] Restored Reservation ID: " << restoredRes.getReservationId()
              << " for Student: " << restoredRes.getStudentName() << "\n";
    return true;
}

int ReservationManager::binarySearchResource(const std::string& resourceId) {
    int low = 0;
    int high = static_cast<int>(resources.size()) - 1;

    while (low <= high) {
        const int mid = low + (high - low) / 2;
        if (resources[mid].getId() == resourceId) {
            return mid;
        }
        if (resources[mid].getId() < resourceId) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    return -1;
}

void ReservationManager::quickSortResources(int low, int high) {
    if (resources.empty()) {
        return;
    }

    if (low < 0) {
        low = 0;
    }

    if (high < low) {
        high = static_cast<int>(resources.size()) - 1;
    }

    if (low >= high) {
        if (resources.size() > 1 && low == 0) {
            high = static_cast<int>(resources.size()) - 1;
        } else {
            return;
        }
    }

    const std::string pivot = resources[high].getId();
    int i = low - 1;
    for (int j = low; j < high; ++j) {
        if (resources[j].getId() <= pivot) {
            ++i;
            std::swap(resources[i], resources[j]);
        }
    }
    std::swap(resources[i + 1], resources[high]);
    const int pivotIndex = i + 1;

    quickSortResources(low, pivotIndex - 1);
    quickSortResources(pivotIndex + 1, high);
}

void ReservationManager::displayAllResources() const {
    std::cout << std::left << std::setw(12) << "Resource ID"
              << std::setw(25) << "Name"
              << std::setw(18) << "Type"
              << std::setw(15) << "Status"
              << std::setw(10) << "Res. Count" << "\n";
    std::cout << std::string(80, '-') << "\n";

    for (const auto& resource : resources) {
        std::cout << std::left << std::setw(12) << resource.getId()
                  << std::setw(25) << resource.getName()
                  << std::setw(18) << resource.getType()
                  << std::setw(15) << (resource.getAvailability() ? "Available" : "Reserved")
                  << std::setw(10) << resource.getReservationCount() << "\n";
    }
}

void ReservationManager::displayActiveReservations() const {
    if (activeReservations.empty()) {
        std::cout << "No active reservations found.\n";
        return;
    }

    std::cout << std::left << std::setw(15) << "Res. ID"
              << std::setw(15) << "Student ID"
              << std::setw(22) << "Student Name"
              << std::setw(15) << "Resource ID"
              << std::setw(15) << "Date" << "\n";
    std::cout << std::string(82, '-') << "\n";

    for (const auto& reservation : activeReservations) {
        std::cout << std::left << std::setw(15) << reservation.getReservationId()
                  << std::setw(15) << reservation.getStudentId()
                  << std::setw(22) << reservation.getStudentName()
                  << std::setw(15) << reservation.getResourceId()
                  << std::setw(15) << reservation.getReservationDate() << "\n";
    }
}

void ReservationManager::generateReport() const {
    std::cout << "\n=============================================\n";
    std::cout << "          SYSTEM SUMMARY REPORT              \n";
    std::cout << "=============================================\n";
    std::cout << " Total Resources Managed: " << resources.size() << "\n";
    std::cout << " Active Reservations:     " << activeReservations.size() << "\n";
    std::cout << " Pending Cancellations:   " << cancellationHistory.size() << " item(s) on undo stack\n";

    std::cout << "\n--- Active Waiting Queue Sizes ---\n";
    bool activeWaitlists = false;
    for (const auto& pair : waitingLists) {
        if (!pair.second.empty()) {
            std::cout << " Resource " << pair.first << ": " << pair.second.size() << " student(s) waiting\n";
            activeWaitlists = true;
        }
    }
    if (!activeWaitlists) {
        std::cout << " No active waiting lists.\n";
    }

    std::cout << "\n--- Resource Utilization Top List ---\n";
    const Resource* mostReserved = nullptr;
    int maxCount = -1;

    for (const auto& resource : resources) {
        if (resource.getReservationCount() > maxCount) {
            maxCount = resource.getReservationCount();
            mostReserved = &resource;
        }
    }

    if (mostReserved && maxCount > 0) {
        std::cout << " Most Frequently Reserved: " << mostReserved->getName()
                  << " (ID: " << mostReserved->getId() << ") - "
                  << maxCount << " total reservation(s)\n";
    } else {
        std::cout << " No reservation utilization recorded yet.\n";
    }
    std::cout << "=============================================\n";
}
