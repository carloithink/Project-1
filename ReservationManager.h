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
  Node(const Reservation& res):data9res),next(nullptr) {}
};
