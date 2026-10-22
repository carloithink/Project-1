#include "Resource.h"
#include <iomanip>
#include <iostream>

using namespace std;

Resource::Resource() 
    : resourceID(""), resourceName(""), category(""), status("Available") {}

Resource::Resource(string ID, string name, string cat, string stat) 
    : resourceID(ID), resourceName(name), category(cat), status(stat) {}

string Resource::getResourceID() const {
    return resourceID;
}

string Resource::getResourceName() const {
    return resourceName;
}

string Resource::getCategory() const {
    return category;
}

string Resource::getStatus() const {
    return status;
}

void Resource::setStatus(const string& newStatus) {
    status = newStatus;
}

void Resource::display() const {
    cout << left
         << setw(12) << resourceID
         << setw(25) << resourceName
         << setw(22) << category
         << setw(15) << status << "\n";
}
