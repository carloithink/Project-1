#ifndef RESERVATION_H
#define RESERVATION_H

#include <string>
using namespace std;

class Reservation {
private:
  int reservationID;
  int studentID;
  string studentName;
  string resourceID;
  string date;

public:
  Reservation();
  Reservation(int resID, int stuID, string stuName, string sourID, string d);

  int getReservationID() const;
  int getStudentID() const;
  string getStudentName() const;
  string getResourceID() const;
  string getDate() const;
  void display() const;
};
#endif
