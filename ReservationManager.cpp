#include "ReservationManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

ReservationManager::ReservationManager() : nextReservationId(101) {}

// File Parsing & Data Loading

bool ReservationManager::loadResources(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile.is_open()) {
        std::cerr << "[Error] Unable to open resource file: " << filename << std::endl;
        return false;
    }

    resources.clear();
    std::string line;
    
    // File format expected: ID,Name,Type,AvailabilityStatus
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string id, name, type, availStr;

        if (std::getline(ss, id, ',') &&
            std::getline(ss, name, ',') &&
            std::getline(ss, type, ',') &&
            std::getline(ss, availStr, ',')) {

            bool isAvailable = (availStr == "1" || availStr == "true" || availStr == "True");
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

    // File format expected: ReservationID,StudentID,StudentName,ResourceID,Date
    while (std::getline(inFile, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string resIdStr, studentId, studentName, resourceId, date;

        if (std::getline(ss, resIdStr, ',') &&
            std::getline(ss, studentId, ',') &&
            std::getline(ss, studentName, ',') &&
            std::getline(ss, resourceId, ',') &&
            std::getline(ss, date, ',')) {

            int resId = std::stoi(resIdStr);
            activeReservations.emplace_back(resId, studentId, studentName, resourceId, date);

            if (resId > maxId) {
                maxId = resId;
            }

            // Mark resource as unavailable and increment usage counter
            int resIndex = binarySearchResource(resourceId);
            if (resIndex != -1) {
                resources[resIndex].setAvailability(false);
                resources[resIndex].incrementReservationCount();
            }
        }
    }

    nextReservationId = maxId + 1;
    inFile.close();
    return true;
}

// ==========================================
// Reservation Operations & Queue/Stack Mgmt
// ==========================================

void ReservationManager::createReservation(const std::string& studentId, 
                                           const std::string& studentName, 
                                           const std::string& resourceId, 
                                           const std::string& date) {
    int resIndex = binarySearchResource(resourceId);

    if (resIndex == -1) {
        std::cout << "[Error] Resource ID '" << resourceId << "' does not exist.\n";
        return;
    }

    Resource& res = resources[resIndex];

    // Case 1: Resource is available - Create immediate reservation
    if (res.getAvailability()) {
        int assignedId = nextReservationId++;
        Reservation newRes(assignedId, studentId, studentName, resourceId, date);
        
        activeReservations.push_back(newRes); // Linked List insertion
        res.setAvailability(false);
        res.incrementReservationCount();

        std::cout << "[Success] Reservation Created Successfully. Reservation ID: " << assignedId << "\n";
    } 
    // Case 2: Resource unavailable - Push to waiting queue
    else {
        Reservation waitRequest(0, studentId, studentName, resourceId, date);
        waitingLists[resourceId].push(waitRequest); // FIFO Queue push

        std::cout << "[Waitlist] Resource '" << resourceId 
                  << "' is currently unavailable. Student " << studentName 
                  << " added to waitlist (Position: " << waitingLists[resourceId].size() << ").\n";
    }
}

bool ReservationManager::cancelReservation(int reservationId) {
    // Traverse active reservations linked list
    for (auto it = activeReservations.begin(); it != activeReservations.end(); ++it) {
        if (it->getReservationId() == reservationId) {
            Reservation cancelledRes = *it;
            std::string resId = cancelledRes.getResourceId();

            // 1. Remove from linked list
            activeReservations.erase(it);

            // 2. Push to cancellation stack (LIFO)
            cancellationHistory.push(cancelledRes);

            std::cout << "[Info] Reservation " << reservationId << " cancelled and added to undo history.\n";

            // 3. Process waiting list queue if pending requests exist
            if (waitingLists.count(resId) > 0 && !waitingLists[resId].empty()) {
                Reservation nextInLine = waitingLists[resId].front();
                waitingLists[resId].pop(); // FIFO Queue pop

                int assignedId = nextReservationId++;
                Reservation promotedRes(assignedId, nextInLine.getStudentId(), 
                                       nextInLine.getStudentName(), resId, 
                                       nextInLine.getReservationDate());

                activeReservations.push_back(promotedRes);

                int resIndex = binarySearchResource(resId);
                if (resIndex != -1) {
                    resources[resIndex].incrementReservationCount();
                }

                std::cout << "[Waitlist Promoted] Resource automatically assigned to waiting student: " 
                          << promotedRes.getStudentName() << " (New Reservation ID: " << assignedId << ").\n";
            } else {
                // No waitlist requests - mark resource as available
                int resIndex = binarySearchResource(resId);
                if (resIndex != -1) {
                    resources[resIndex].setAvailability(true);
                }
            }

            return true;
        }
    }

    return false;
}

bool ReservationManager::undoCancellation() {
    // Handle empty stack edge case
    if (cancellationHistory.empty()) {
        return false;
    }

    // 1. Pop most recent cancellation from stack (LIFO)
    Reservation restoredRes = cancellationHistory.top();
    cancellationHistory.pop();

    std::string resId = restoredRes.getResourceId();
    int resIndex = binarySearchResource(resId);

    // 2. Re-insert restored reservation into active linked list
    activeReservations.push_back(restoredRes);

    if (resIndex != -1) {
        resources[resIndex].setAvailability(false);
    }

    std::cout << "[Undo] Restored Reservation ID: " << restoredRes.getReservationId() 
              << " for Student: " << restoredRes.getStudentName() << "\n";

    return true;
}

// ==========================================
// Searching & Sorting Algorithms
// ==========================================

int ReservationManager::binarySearchResource(const std::string& resourceId) {
    int low = 0;
    int high = static_cast<int>(resources.size()) - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        if (resources[mid].getId() == resourceId) {
            return mid;
        }
        if (resources[mid].getId() < resourceId) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }

    // Return -1 if not found
    return -1;
}

void ReservationManager::quickSortResources(int low, int high) {
    if (low < high) {
        // Partitioning process
        std::string pivot = resources[high].getId();
        int i = low - 1;

        for (int j = low; j < high; ++j) {
            if (resources[j].getId() <= pivot) {
                i++;
                std::swap(resources[i], resources[j]);
            }
        }
        std::swap(resources[i + 1], resources[high]);
        int pivotIndex = i + 1;

        // Recursive quicksort calls
        quickSortResources(low, pivotIndex - 1);
        quickSortResources(pivotIndex + 1, high);
    }
}

// ==========================================
// Reporting & Views
// ==========================================

void ReservationManager::displayAllResources() const {
    std::cout << std::left << std::setw(12) << "Resource ID" 
              << std::setw(25) << "Name" 
              << std::setw(18) << "Type" 
              << std::setw(15) << "Status" 
              << std::setw(10) << "Res. Count" << "\n";
    std::cout << std::string(80, '-') << "\n";

    for (const auto& res : resources) {
        std::cout << std::left << std::setw(12) << res.getId()
                  << std::setw(25) << res.getName()
                  << std::setw(18) << res.getType()
                  << std::setw(15) << (res.getAvailability() ? "Available" : "Reserved")
                  << std::setw(10) << res.getReservationCount() << "\n";
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

    for (const auto& res : activeReservations) {
        std::cout << std::left << std::setw(15) << res.getReservationId()
                  << std::setw(15) << res.getStudentId()
                  << std::setw(22) << res.getStudentName()
                  << std::setw(15) << res.getResourceId()
                  << std::setw(15) << res.getReservationDate() << "\n";
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

    for (const auto& res : resources) {
        if (res.getReservationCount() > maxCount) {
            maxCount = res.getReservationCount();
            mostReserved = &res;
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
