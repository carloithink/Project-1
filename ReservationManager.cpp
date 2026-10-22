#include "ReservationManager.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

ReservationManager::ReservationManager() : reservationHead(nullptr) {}

ReservationManager::~ReservationManager() {
    // Clean up linked list memory
    Node* current = reservationHead;
    while (current != nullptr) {
        Node* nextNode = current->next;
        delete current;
        current = nextNode;
    }
}

void ReservationManager::loadResourcesFromFile(const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "[Error] Could not open resource file: " << fileName << "\n";
        return;
    }

    resources.clear();
    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string id, name, category, status;
        
        // Parsing pipe-delimited format: R101|Study Room 101|Study Room|Available
        if (getline(ss, id, '|') &&
            getline(ss, name, '|') &&
            getline(ss, category, '|') &&
            getline(ss, status, '|')) {
            resources.emplace_back(id, name, category, status);
        }
    }
    file.close();
    cout << "[Success] Loaded " << resources.size() << " resources from " << fileName << ".\n";
}

void ReservationManager::loadReservationsFromFile(const string& fileName) {
    ifstream file(fileName);
    if (!file.is_open()) {
        cerr << "[Error] Could not open reservation file: " << fileName << "\n";
        return;
    }
       string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        stringstream ss(line);
        string resIdStr, stuIdStr, stuName, sourId, date;

        // Parsing pipe-delimited format: 301|1001|Alice Smith|R101|09/15/2026
        if (getline(ss, resIdStr, '|') &&
            getline(ss, stuIdStr, '|') &&
            getline(ss, stuName, '|') &&
            getline(ss, sourId, '|') &&
            getline(ss, date, '|')) {
            
            int resID = stoi(resIdStr);
            int stuID = stoi(stuIdStr);
            
            Reservation res(resID, stuID, stuName, sourId, date);
            
            // Insert into linked list
            Node* newNode = new Node(res);
            newNode->next = reservationHead;
            reservationHead = newNode;

            // Update resource status to Unavailable
            Resource* resPtr = findResourcebyID(sourId);
            if (resPtr != nullptr) {
                resPtr->setStatus("Unavailable");
            }
        }
    }
    file.close();
    cout << "[Success] Loaded reservations from " << fileName << ".\n";
}

void ReservationManager::displayAllResources() const {
    cout << left << setw(12) << "Resource ID"
         << setw(25) << "Name"
         << setw(22) << "Category"
         << setw(15) << "Status" << "\n";
    cout << string(74, '-') << "\n";

    for (const auto& res : resources) {
        res.display();
    }
}
void ReservationManager::displayAvailableResources() const {
    cout << left << setw(12) << "Resource ID"
         << setw(25) << "Name"
         << setw(22) << "Category"
         << setw(15) << "Status" << "\n";
    cout << string(74, '-') << "\n";

    for (const auto& res : resources) {
        if (res.getStatus() == "Available") {
            res.display();
        }
    }
}

Resource* ReservationManager::findResourcebyID(const string& resID) {
    // Custom Search Implementation (Linear/Binary Search integration)
    for (auto& res : resources) {
        if (res.getResourceID() == resID) {
            return &res;
        }
    }
    return nullptr;
}

void ReservationManager::makeReservation(int resID, int stuID, const string& stuName, const string& sourID, const string& d) {
    Resource* targetResource = findResourcebyID(sourID);
    if (targetResource == nullptr) {
        cout << "[Error] Resource ID '" << sourID << "' does not exist.\n";
        return;
    }

    if (targetResource->getStatus() == "Available") {
        targetResource->setStatus("Unavailable");
        Reservation newRes(resID, stuID, stuName, sourID, d);
        
        Node* newNode = new Node(newRes);
        newNode->next = reservationHead;
        reservationHead = newNode;

        cout << "[Success] Reservation " << resID << " created successfully for " << stuName << ".\n";
    } else {
        // Resource unavailable -> Add to waitlist queue
        Reservation waitRes(resID, stuID, stuName, sourID, d);
        addToWaitlist(waitRes);
        cout << "[Waitlist] Resource '" << sourID << "' is currently unavailable. Student added to waiting list.\n";
    }
}
bool ReservationManager::cancelReservation(int resID) {
    Node* current = reservationHead;
    Node* prev = nullptr;

    while (current != nullptr) {
        if (current->data.getReservationID() == resID) {
            Reservation cancelled = current->data;
            string resSourceID = cancelled.getResourceID();

            // Remove from linked list
            if (prev == nullptr) {
                reservationHead = current->next;
            } else {
                prev->next = current->next;
            }
            delete current;

            // Push to undo stack
            undoStack.push(cancelled);

            // Update resource availability or promote from waitlist
            Resource* resource = findResourcebyID(resSourceID);
            if (!waitlist.empty()) {
                // Check if waitlist has an item for this resource
                Reservation nextStudent = waitlist.front();
                waitlist.pop();
                
                // Automatically create reservation for the waiting student
                makeReservation(nextStudent.getReservationID(), nextStudent.getStudentID(), 
                                nextStudent.getStudentName(), resSourceID, nextStudent.getDate());
                cout << "[Waitlist Promoted] Resource assigned to next waiting student.\n";
            } else if (resource != nullptr) {
                resource->setStatus("Available");
            }

            cout << "[Success] Reservation " << resID << " cancelled and added to undo stack.\n";
            return true;
        }
        prev = current;
        current = current->next;
    }

    cout << "[Error] Reservation ID " << resID << " not found.\n";
    return false;
}
void ReservationManager::showActiveReservations() const {
    if (reservationHead == nullptr) {
        cout << "No active reservations.\n";
        return;
    }

    cout << left << setw(15) << "Res. ID"
         << setw(15) << "Student ID"
         << setw(22) << "Student Name"
         << setw(15) << "Resource ID"
         << setw(15) << "Date" << "\n";
    cout << string(82, '-') << "\n";

    Node* current = reservationHead;
    while (current != nullptr) {
        current->data.display();
        current = current->next;
    }
}

Reservation* ReservationManager::findReservationbyID(int resID) {
    Node* current = reservationHead;
    while (current != nullptr) {
        if (current->data.getReservationID() == resID) {
            return &(current->data);
        }
        current = current->next;
    }
    return nullptr;
}

void ReservationManager::addToWaitlist(const Reservation& res) {
    waitlist.push(res);
}

void ReservationManager::processWaitlist() {
    if (waitlist.empty()) {
        cout << "Waiting list is empty.\n";
        return;
    }
    Reservation nextRes = waitlist.front();
    waitlist.pop();
    makeReservation(nextRes.getReservationID(), nextRes.getStudentID(), 
                    nextRes.getStudentName(), nextRes.getResourceID(), nextRes.getDate());
}
void ReservationManager::showWaitlist() const {
    if (waitlist.empty()) {
        cout << "No students currently on the waiting list.\n";
        return;
    }
    cout << "=== Current Waiting List Queue ===\n";
    queue<Reservation> tempQueue = waitlist;
    int pos = 1;
    while (!tempQueue.empty()) {
        Reservation r = tempQueue.front();
        tempQueue.pop();
        cout << pos++ << ". Student: " << r.getStudentName() << " (ID: " << r.getStudentID() 
             << ") for Resource: " << r.getResourceID() << "\n";
    }
}

void ReservationManager::undoCancelReservation() {
    if (undoStack.empty()) {
        cout << "[Error] Undo stack is empty. No cancellations to undo.\n";
        return;
    }

    Reservation restored = undoStack.top();
    undoStack.pop();

    // Re-add to linked list
    Node* newNode = new Node(restored);
    newNode->next = reservationHead;
    reservationHead = newNode;

    // Set resource unavailable
    Resource* resource = findResourcebyID(restored.getResourceID());
    if (resource != nullptr) {
        resource->setStatus("Unavailable");
    }

    cout << "[Success] Restored cancelled reservation ID: " << restored.getReservationID() << "\n";
}
void ReservationManager::showCancelHistory() const {
    if (undoStack.empty()) {
        cout << "Cancellation history stack is empty.\n";
        return;
    }
    cout << "=== Cancellation Undo Stack History ===\n";
    stack<Reservation> tempStack = undoStack;
    while (!tempStack.empty()) {
        Reservation r = tempStack.top();
        tempStack.pop();
        cout << "- Res ID: " << r.getReservationID() << ", Student: " << r.getStudentName() 
             << ", Resource: " << r.getResourceID() << "\n";
    }
}

void ReservationManager::sortResourcesbyName() {
    // Custom Quick Sort implementation on resources vector by Resource Name
    if (resources.size() <= 1) return;

    // Lambda helper for partition/quicksort
    auto partition = [](vector<Resource>& vec, int low, int high) {
        string pivot = vec[high].getResourceName();
        int i = low - 1;
        for (int j = low; j < high; j++) {
            if (vec[j].getResourceName() <= pivot) {
                i++;
                swap(vec[i], vec[j]);
            }
        }
        swap(vec[i + 1], vec[high]);
        return i + 1;
    };

    std::function<void(vector<Resource>&, int, int)> quickSort = [&](vector<Resource>& vec, int low, int high) {
        if (low < high) {
            int pi = partition(vec, low, high);
            quickSort(vec, low, pi - 1);
            quickSort(vec, pi + 1, high);
        }
    };

    quickSort(resources, 0, static_cast<int>(resources.size()) - 1);
    cout << "[Success] Resources sorted alphabetically by name using custom Quick Sort.\n";
}
void ReservationManager::findReservationsbyStudent(int stuID) {
    Node* current = reservationHead;
    bool found = false;
    cout << "=== Reservations for Student ID: " << stuID << " ===\n";
    while (current != nullptr) {
        if (current->data.getStudentID() == stuID) {
            current->data.display();
            found = true;
        }
        current = current->next;
    }
    if (!found) {
        cout << "No reservations found for student ID " << stuID << ".\n";
    }
}

void ReservationManager::generateReports() const {
    cout << "\n=============================================\n";
    cout << "         SYSTEM REPORTS & STATISTICS         \n";
    cout << "=============================================\n";
    
    // Count active reservations
    int activeCount = 0;
    Node* current = reservationHead;
    while (current != nullptr) {
        activeCount++;
        current = current->next;
    }

    cout << "Total Resources Managed: " << resources.size() << "\n";
    cout << "Active Reservations:     " << activeCount << "\n";
    cout << "Undo Stack Size:         " << undoStack.size() << " item(s)\n";
    cout << "Waiting List Queue Size: " << waitlist.size() << " student(s)\n";
    cout << "=============================================\n";
}
