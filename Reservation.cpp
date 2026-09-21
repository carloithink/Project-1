C++
#include "../include/Reservation.h"
#include <iostream>
#include <iomanip>

// Default Constructor
Reservation::Reservation() 
    : reservationID(0), studentID(""), studentName(""), resourceID(""), date("") {}

// Parameterized Constructor
Reservation::Reservation(int resID, std::string stuID, std::string stuName, std::string resID_Param, std::string d)
    : reservationID(resID), studentID(stuID), studentName(stuName), resourceID(resID_Param), date(d) {}

// Getters
int Reservation::getReservationID() const {
    return reservationID;
}

std::string Reservation::getStudentID() const {
    return studentID;
}

std::string Reservation::getStudentName() const {
    return studentName;
}

std::string Reservation::getResourceID() const {
    return resourceID;
}

std::string Reservation::getDate() const {
    return date;
}

// Display method
void Reservation::display() const {
    std::cout << std::left 
              << std::setw(15) << reservationID
              << std::setw(15) << studentID
              << std::setw(22) << studentName
              << std::setw(15) << resourceID
              << std::setw(15) << date << "\n";
}
