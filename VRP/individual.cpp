#include "../../ECF/ECF.h"
#include <iostream>
#include "individual.h"
#include "problem.h"
#include <algorithm>
#include <stdio.h>

individual::individual() {
	name_ = "ind";
}

bool individual::initialize(StateP state) {
	return true;
}

void individual::write(XMLNode& xMyGenotype) {

}

void individual::read(XMLNode& xMyGenotype) {

}

individual::individual(std::vector<int> gen, problem* inputProblem) {
	genome = gen;
	input = inputProblem;
}

void individual::setGenome(std::vector<int> gen) {
	genome = gen;
}

void individual::setFitness(double newFitness) {
	fitness = newFitness;
}

individual* individual::copy() {
	individual* ind = new individual();
	ind->setGenome(this->genome);
	ind->setFitness(this->fitness);
	return ind;
}

std::vector<int> individual::getGenome() {
	return genome;
}

double individual::getFitness() {
	return fitness;
}

double individual::fitnessFunction() {

	double totalDistance = 0;
	int overCapacity = 0;
	double overTime = 0;
	double fitness = 0;
	std::vector<customer> customers = input->allCustomer;
	std::vector<int> currentCustomers;
	std::vector<customer>::iterator it;

	int currentVehicle = genome[0];
	int currentDemand = 0;
	double currentTime = 0;
	bool isFeasible = true;
	int capacity = input->getCapacity();

	for (int i = 1; i < genome.size(); i++) {
		if (genome[i] <= input->getNumCusto()) {
			currentCustomers.push_back(genome[i]);
			if (currentCustomers.size() == 1) {
				currentTime += input->distance[0][currentCustomers[0]];
			}
			else {
				currentTime += input->distance[currentCustomers[currentCustomers.size() - 2]][currentCustomers.size() - 1];
			}
			for (int j = 0; j < customers.size(); j++) {
				if (customers[j].id == genome[i]) {
					currentDemand += customers[j].demand;
					if ((currentDemand > capacity) && (currentDemand - capacity) > overCapacity) {
						overCapacity = (currentDemand - capacity);
					}
					if (customers[j].start > currentTime) {
						overTime += (customers[j].start - currentTime);
						currentTime = customers[j].start;
					}
					if (currentTime > customers[j].end) {
						isFeasible = false;
					}
					currentTime += customers[j].unload;
					break;
				}

			}
		}
		if (genome[i] > input->getNumCusto() || i == genome.size() - 1) {
			if (!currentCustomers.empty()) {
				totalDistance += input->distance[0][currentCustomers[0]];
				for (int j = 0; j < currentCustomers.size() - 1; j++) {
					totalDistance += input->distance[currentCustomers[j]][currentCustomers[j + 1]];
				}
				totalDistance += input->distance[currentCustomers.back()][0];
			}

			currentVehicle = genome[i];
			currentCustomers.clear();
			
			currentDemand = 0;
			currentTime = 0;
		}
	}

	if (input->noTW) {
		fitness = totalDistance + 1000 * overCapacity;
	}
	else {
		fitness = totalDistance + 1000 * overCapacity + overTime;
	}
	
	if (!isFeasible) {
		fitness *= input->getNumVehicles();
	}
	setFitness(fitness);

	return fitness;
}