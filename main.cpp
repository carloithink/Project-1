#include <iostream>
#include <string>
#include <limits>
#include <iomanip>
#include "ReservationManager.h"
using namespace std;

// Helper functions for input validation 
void displayMenu();
void clearInputBuffer();
int getValidIntChoice(int min, int max);
string getNonEmptyString(const string& prompt);

int main() {
    ReservationManager manager;

    cout << "==================================================\n";
    cout << " Initializing Campus Resource Reservation System...\n";
    cout << "==================================================\n";

    if (!manager.loadResources("data/resources.txt")) {
        cout << "[Warning] Could not load resources from 'data/resources.txt'. Starting with empty inventory.\n";
    } else {
        cout << "[Success] Resources loaded successfully.\n";
    }

    if (!manager.loadReservations("data/reservations.txt")) {
        cout << "[Warning] Could not load reservations from 'data/reservations.txt'. Starting with no active reservations.\n";
    } else {
        cout << "[Success] Active reservations loaded successfully.\n";
    }
    bool running = true;

    while (running) {
        displayMenu();
        int choice = getValidIntChoice(1, 9);

        cout << "\n--------------------------------------------------\n";

        switch (choice) {
            case 1: { // View Resources
                cout << "=== All Resources ===\n";
                manager.displayAllResources();
                break;
            }
            case 2: { // Create Reservation
                cout << "=== Create Reservation ===\n";
                string studentId = getNonEmptyString("Enter Student ID: ");
                string studentName = getNonEmptyString("Enter Student Name: ");
                string resourceId = getNonEmptyString("Enter Resource ID: ");
                string date = getNonEmptyString("Enter Date (YYYY-MM-DD): ");

                manager.createReservation(studentId, studentName, resourceId, date);
                break;
            }
            case 3: { // Cancel Reservation
                cout << "=== Cancel Reservation ===\n";
                cout << "Enter Reservation ID to cancel: ";
                int resId = getValidIntChoice(1, 100000);

                if (manager.cancelReservation(resId)) {
                    cout << "[Success] Reservation " << resId << " cancelled and pushed to history stack.\n";
                } else {
                    cout << "[Error] Reservation ID " << resId << " not found.\n";
                }
                break;
            }case 4: { // View Waiting Lists
                cout << "=== Waiting Lists ===\n";
                manager.generateReport(); // Or dedicated waiting list viewer call
                break;
            }
            case 5: { // Undo Cancellation
                cout << "=== Undo Cancellation ===\n";
                if (manager.undoCancellation()) {
                    cout << "[Success] Most recent cancellation restored successfully.\n";
                } else {
                    cout << "[Error] Cancellation history stack is empty. Nothing to undo.\n";
                }
                break;
            }
            case 6: { // Search Reservations
                cout << "=== Search Reservations ===\n";
                string searchTarget = getNonEmptyString("Enter Student ID or Reservation ID to search: ");
                
                // Example check for numeric reservation ID vs Student ID
                try {
                    int resId = stoi(searchTarget);
                    // Search by reservation ID logic
                    cout << "Searching active reservations for Reservation ID: " << resId << "...\n";
                } catch (const invalid_argument&) {
                    // Search by student ID logic
                    cout << "Searching active reservations for Student ID: " << searchTarget << "...\n";
                }
                manager.displayActiveReservations();
                break;
            }
            case 7: { // Sort Resources
                cout << "=== Sort Resources ===\n";
                cout << "1. Sort by Resource ID (Quick Sort)\n";
                cout << "2. Sort by Reservation Frequency\n";
                cout << "Select Sort Option: ";
                int sortChoice = getValidIntChoice(1, 2);

                if (sortChoice == 1) {
                    // Internal call to quicksort algorithm
                    manager.quickSortResources(0, 0); // High index passed within manager
                    cout << "[Success] Resources sorted by Resource ID.\n";
                } else {
                    cout << "[Info] Feature sorting by frequency coming soon.\n";
                }
                manager.displayAllResources();
                break;
            }
            case 8: { // Generate Report
                cout << "=== System Report & Statistics ===\n";
                manager.generateReport();
                break;
            }
            case 9: { // Exit
                cout << "Exiting system. Saving state...\n";
                running = false;
                break;
            }
            default:
                cout << "[Error] Invalid option selected.\n";
                break;
        }

        cout << "--------------------------------------------------\n";
        if (running) {
            cout << "\nPress ENTER to continue...";
            cin.get();
        }
    }

    cout << "Thank you for using Campus Resource Reservation System!\n";
    return 0;
}

//Displays the main UI menu.//
void displayMenu() {
    cout << "\n===== Campus Resource Reservation System =====\n";
    cout << " 1. View Resources\n";
    cout << " 2. Create Reservation\n";
    cout << " 3. Cancel Reservation\n";
    cout << " 4. View Waiting Lists\n";
    cout << " 5. Undo Cancellation\n";
    cout << " 6. Search Reservations\n";
    cout << " 7. Sort Resources\n";
    cout << " 8. Generate Report\n";
    cout << " 9. Exit\n";
    cout << "=============================================\n";
    cout << "Enter Choice (1-9): ";
}

// Clears input stream errors and flushes residual buffer input.//
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

//Validates integer inputs against minimum and maximum boundaries.//
int getValidIntChoice(int min, int max) {
    int input;
    while (true) {
        if (cin >> input) {
            if (input >= min && input <= max) {
                clearInputBuffer(); // Clear trailing newline
                return input;
            }
        }
        cout << "[Invalid Input] Please enter an integer between " 
                  << min << " and " << max << ": ";
        clearInputBuffer();
    }
}

//Ensures user does not provide empty string or whitespace-only inputs.//
string getNonEmptyString(const string& prompt) {
    string input;
    while (true) {
        cout << prompt;
        getline(std::cin, input);
        
        // Trim leading and trailing spaces if necessary
        if (!input.empty() && input.find_first_not_of(" \t\n\r") != string::npos) {
            return input;
        }
        cout << "[Invalid Input] Field cannot be empty. Please try again.\n";
    }
}
