#ifndef PROBLEM_H
#define PROBLEM_H

#include <vector>
#include "customer.h"

class individual;

class problem {
public:
	problem();
	problem(const char* filename);
	bool load(const char* filename);
	int getCapacity() const;
	int getNumCusto() const;
	int getNumVehicles() const;
	double getDistance(int id1, int id2) const;
	const customer& operator [] (int id) const;
	int calMinRoute() const;
	std::vector<std::vector<double>> distance;
	std::vector<customer> allCustomer;
	int isPDVRP;
	int noTW;


private:
	void calDistances();
	int capacity;
	int vehicleNum;
	int customerNumber;
};
#endif