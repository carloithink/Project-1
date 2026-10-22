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

    // Load files using exact header method names
    manager.loadResourcesFromFile("resources.txt");
    manager.loadReservationsFromFile("reservations.txt");

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
                int studentId = getValidIntChoice(1, 999999);
                string studentName = getNonEmptyString("Enter Student Name: ");
                string resourceId = getNonEmptyString("Enter Resource ID (e.g., R101): ");
                string date = getNonEmptyString("Enter Date (MM/DD/YYYY): ");

                // Generate a pseudo-random or sequential reservation ID for demo
                int resId = 321; // or derived uniquely
                manager.makeReservation(resId, studentId, studentName, resourceId, date);
                break;
            }
            case 3: { // Cancel Reservation
                cout << "=== Cancel Reservation ===\n";
                cout << "Enter Reservation ID to cancel: ";
                int resId = getValidIntChoice(1, 100000);

                manager.cancelReservation(resId);
                break;
            }
            case 4: { // View Waiting Lists
                cout << "=== Waiting Lists ===\n";
                manager.showWaitlist();
                break;
            }
            case 5: { // Undo Cancellation
                cout << "=== Undo Cancellation ===\n";
                manager.undoCancelReservation();
                break;
            }
            case 6: { // Search Reservations
                cout << "=== Search Reservations ===\n";
                string searchTarget = getNonEmptyString("Enter Student ID to search reservations: ");
                
                try {
                    int stuId = stoi(searchTarget);
                    manager.findReservationsbyStudent(stuId);
                } catch (const invalid_argument&) {
                    cout << "[Error] Please enter a valid numeric Student ID.\n";
                }
                break;
            }
            case 7: { // Sort Resources
                cout << "=== Sort Resources ===\n";
                cout << "Sorting resources alphabetically by name using Quick Sort...\n";
                manager.sortResourcesbyName();
                manager.displayAllResources();
                break;
            }
            case 8: { // Generate Report
                cout << "=== System Report & Statistics ===\n";
                manager.generateReports();
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

// Displays the main UI menu.
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

// Clears input stream errors and flushes residual buffer input.
void clearInputBuffer() {
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

// Validates integer inputs against minimum and maximum boundaries.
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

// Ensures user does not provide empty string or whitespace-only inputs.
string getNonEmptyString(const string& prompt) {
    string input;
    while (true) {
        cout << prompt;
        getline(cin, input);
        
        if (!input.empty() && input.find_first_not_of(" \t\n\r") != string::npos) {
            return input;
        }
        cout << "[Invalid Input] Field cannot be empty. Please try again.\n";
    }
}

