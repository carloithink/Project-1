#ifndef RESOURCE_H
#define RESOURCE_H

#include <string>
using namespace std;

class Resource {
private:
  string resourceID;
  string resourceName;
  string category;
  string status;

public:
  Resource();
  Resource(string ID, string name, string cat, string stat);

  string getResourceID() const;
  string getResourceName() const;
  string getCategory() const;
  string getStatus() const;
  void setStatus(const string& newStatus);
  void display() const;
};
#endif


