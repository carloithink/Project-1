C++
#include "../include/Reservation.h"
#include <iostream>
#include <iomanip>

// Default Constructor
Reservation::Reservation() 
    : reservationId(0), studentId(""), studentName(""), resourceId(""), reservationDate("") {}

// Parameterized Constructor
Reservation::Reservation(int resId, std::string stId, std::string stName, std::string rId, std::string date)
    : reservationId(resId), studentId(stId), studentName(stName), resourceId(rId), reservationDate(date) {}

// Getters
int Reservation::getReservationId() const {
    return reservationId;
}

std::string Reservation::getStudentId() const {
    return studentId;
}

std::string Reservation::getStudentName() const {
    return studentName;
}

std::string Reservation::getResourceId() const {
    return resourceId;
}

std::string Reservation::getReservationDate() const {
    return reservationDate;
}

// Display method for a single reservation record
void Reservation::display() const {
    std::cout << std::left 
              << std::setw(15) << reservationId
              << std::setw(15) << studentId
              << std::setw(22) << studentName
              << std::setw(15) << resourceId
              << std::setw(15) << reservationDate << "\n";
}
