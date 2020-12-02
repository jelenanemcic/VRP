#include "problem.h"
#include "individual.h"
#include "customer.h"
#include <iostream>

using namespace std;

problem::problem() {
}

problem::problem(const char* filename) {
	load(filename);
}

bool problem::load(const char* filename) {
	FILE *fp = fopen(filename, "rb");

	if (fp == NULL) {
		return false;
	}

	if (fscanf(fp, "%d", &capacity) != 1) {
		fclose(fp);
		return false;
	}

	if (fscanf(fp, "%d", &vehicleNum) != 1) {
		fclose(fp);
		return false;
	}

	if (fscanf(fp, "%d", &isPDVRP) != 1) {
		fclose(fp);
		return false;
	}

	noTW = 0;

	if (isPDVRP == 0) {
		fseek(fp, -((long)sizeof(int)), SEEK_CUR);
	}
	else if (isPDVRP == 2) {
		isPDVRP = 0;
		noTW = 1;
	}

	int totalDemand = 0;
	int i = 0;
	double x, y;
	int d, s, e, u, p, del;
	if (isPDVRP == 0) {
		while (fscanf(fp, "%*d%lf%lf%d%d%d%d", &x, &y, &d, &s, &e, &u) == 6) {

			totalDemand += d;
			customer newCustomer(i, x, y, d, s, e, u, 0, 0);
			allCustomer.push_back(newCustomer);
			i++;
		}
	}
	else {
		while (fscanf(fp, "%*d%lf%lf%d%d%d%d%d%d", &x, &y, &d, &s, &e, &u, &p, &del) == 8) {

			totalDemand += d;
			customer newCustomer(i, x, y, d, s, e, u, p, del);
			allCustomer.push_back(newCustomer);
			i++;
		}
	}
	fclose(fp);

	calDistances();
	allCustomer.erase(allCustomer.begin());	 // ignore depot 
	customerNumber = allCustomer.size();	 

	return true;
}

int problem::getCapacity() const {
	return capacity;
}

int problem::getNumCusto() const {
	return customerNumber;  
}

int problem::getNumVehicles() const {
	return vehicleNum;
}

double problem::getDistance(int id1, int id2) const {
	return distance[id1][id2];
}

const customer& problem::operator [] (int id) const {
	return allCustomer[id];
}

// minimum # of route = ceil(total demand / vehicle's capacity);
int problem::calMinRoute() const {
	int totalDemand = getCapacity() - 1;    // ceiling
	for (int i = 1; i <= getNumCusto(); ++i)
	{
		totalDemand += allCustomer[i].demand;
	}
	return totalDemand / getCapacity();
}

void problem::calDistances() {
	distance.resize(allCustomer.size());

	for (unsigned int i = 0; i < allCustomer.size(); ++i) {
		distance[i].resize(allCustomer.size());
	}

	for (unsigned int x = 0; x < allCustomer.size(); ++x) {
		for (unsigned int y = x + 1; y < allCustomer.size(); ++y) {
			double ans = hypot(allCustomer[x].x_pos - allCustomer[y].x_pos,
				allCustomer[x].y_pos - allCustomer[y].y_pos);
			distance[x][y] = distance[y][x] = ans;
		}
	}
}
