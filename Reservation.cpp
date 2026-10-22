#include "Reservation.h"
#include <iomanip>
#include <iostream>

Reservation::Reservation()
    : reservationID(0), studentID(0), studentName(""), resourceID(""), date("") {}

Reservation::Reservation(int resID, int stuID, string stuName, string sourID, string d)
    : reservationID(resID), studentID(stuID), studentName(stuName), resourceID(sourID), date(d) {}

int Reservation::getReservationID() const {
    return reservationID;
}

int Reservation::getStudentID() const {
    return studentID;
}

string Reservation::getStudentName() const {
    return studentName;
}

string Reservation::getResourceID() const {
    return resourceID;
}

string Reservation::getDate() const {
    return date;
}

void Reservation::display() const {
    cout << left
         << setw(15) << reservationID
         << setw(15) << studentID
         << setw(22) << studentName
         << setw(15) << resourceID
         << setw(15) << date << "\n";
}

