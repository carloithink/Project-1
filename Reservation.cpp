#include "include/Reservation.h"

#include <iomanip>
#include <iostream>

Reservation::Reservation()
    : reservationId(0), studentId(""), studentName(""), resourceId(""), date("") {}

Reservation::Reservation(int resId, std::string stuId, std::string stuName,
                       std::string resIdValue, std::string reservationDate)
    : reservationId(resId), studentId(std::move(stuId)), studentName(std::move(stuName)),
      resourceId(std::move(resIdValue)), date(std::move(reservationDate)) {}

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
    return date;
}

void Reservation::display() const {
    std::cout << std::left
              << std::setw(15) << reservationId
              << std::setw(15) << studentId
              << std::setw(22) << studentName
              << std::setw(15) << resourceId
              << std::setw(15) << date << "\n";
}

