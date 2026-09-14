#ifndef RESERVATIONMANAGER_H
#define RESERVATIONMANAGER_H

#include "Resource.h"
#include "Reservation.h"
#include <vector>
#include <queue>
#include <string>
#include <stack>

using namespace std;

struct Node {
  Reservation data;
  Node* next;
  Node(const Reservation& res):data(res),next(nullptr) {}
};

class ReservationManager {
private:
  vector<Resource> resources;
  Node* reservationHead;
  queue<Reservation> waitlist;
  stack<Reservation> undoStack

public:
  ReservationManager();
  ~ReservationManager();

  void loadResourcesFromFile(const string& fileName);
  void loadReservationsFromFile(const string& fileName);

  void displayAllResources() const;
  void displayAvailableResources() const;
  Resource* findResourcebyID(const string& resID);

  void makeReservation(int resID, int stuID, const string& stuName, const string& sourID, const string& d);
  bool cancelReservation(int resID);
  void showActiveReservations() const;
  Reservation* findReservationbyID(int resID);

  void addToWaitlist(const Reservation& res);
  void processWaitlist();
  void showWaitlist() const;

  void undoCancelReservation();
  void showCancelHistory() const;

  void sortResourcesbyName();
  void findReservationsbyStudent(int stuID);
  void generateReports() const;
};
#endif
