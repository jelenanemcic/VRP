#include "../../ECF/ECF.h"
#include <iostream>
#include "individualList.h"
#include "problem.h"
#include <algorithm>
#include <stdio.h>


individualList::individualList() {
	name_ = "individualList";
}

bool individualList::initialize(StateP state) {
	if (state->getContext()->environment == NULL) {
		voidP filenamePtr = state->getRegistry()->getEntry("vrp.infile"); // tu se cita
		string filename = *((string*)filenamePtr.get());
		problem* inputProblem = new problem(filename.c_str());
		state->getContext()->environment = inputProblem;
	}

	inputProblem = (problem*)state->getContext()->environment;
	int vehicleNumber = inputProblem->getNumVehicles();
	int customerNumber = inputProblem->getNumCusto();
	std::vector<int> vehicles(vehicleNumber);

	customerList.resize(customerNumber);
	vehicleList.resize(customerNumber);

	for (int i = 0; i < customerNumber; i++) {
		customerList[i] = i + 1;
	}

	random_shuffle(customerList.begin(), customerList.end());

	for (int i = 0; i < customerNumber; i++) {
		vehicleList[i] = rand() % (vehicleNumber)+customerNumber + 1;
	}

	if (inputProblem->isPDVRP == 1) {
		checkPPD();
	}
	return true;
}

individualList* individualList::copy() {
	individualList* list = new individualList();
	list->setCustomerList(customerList);
	list->setVehicleList(vehicleList);
	list->setProblem(inputProblem);
	return list;
}

void individualList::write(XMLNode &xMyGenotype) {
	xMyGenotype = XMLNode::createXMLTopNode("individualList");
	stringstream sValue;
	sValue << this->customerList.size();
	xMyGenotype.addAttribute("size", sValue.str().c_str());

	sValue.str("");

	// write genome to sValue
	std::vector<int>& customers = this->customerList;
	std::vector<int>& vehicles = this->vehicleList;

	sValue << "Customer list: ";
	for (int i = 0; i < customers.size(); i++) {
		sValue << customers[i] << " ";
	}
	sValue << "  ";
	sValue << "Vehicle list: ";
	for (int i = 0; i < vehicles.size(); i++) {
		sValue << vehicles[i] << " ";
	}

	xMyGenotype.addText(sValue.str().c_str());
}

void individualList::read(XMLNode& xMyGenotype) {

}

std::vector<MutationOpP> individualList::getMutationOp() {
	std::vector<MutationOpP> mut;
	mut.push_back(static_cast<MutationOpP> (new individualListMutationOp));
	return mut;
}

std::vector<CrossoverOpP> individualList::getCrossoverOp() {
	std::vector<CrossoverOpP> crx;
	crx.push_back(static_cast<CrossoverOpP> (new individualListCrossoverOp));
	return crx;
}

void individualList::print() {
	printf("C:");
	for (int i = 0; i < inputProblem->getNumCusto(); i++) {
		printf("%d ", customerList[i]);
	}
	printf("\n");

	printf("V:");
	for (int i = 0; i < inputProblem->getNumCusto(); i++) {
		printf("%d ", vehicleList[i]);
	}
	printf("\n");
}

problem* individualList::getProblem() {
	return inputProblem;
}

void individualList::setProblem(problem* problem) {
	inputProblem = problem;
}

std::vector<int> individualList::getCustomerList() {
	return customerList;
}

std::vector<int> individualList::getVehicleList(){
	return vehicleList;
}

void individualList::setCustomerList(std::vector<int> customers) {
	customerList = customers;
}

void individualList::setVehicleList(std::vector<int> vehicles) {
	vehicleList = vehicles;
}

individualP individualList::inBetweenStep() {
	int vehicleNumber = inputProblem->getNumVehicles();
	int customerNumber = inputProblem->getNumCusto();
	std::vector<int> genome;

	for (int i = 0; i < vehicleNumber; i++) {
		int vehicle = customerNumber + i + 1;
		genome.push_back(vehicle);
		for (int j = 0; j < customerNumber; j++) {
			if (vehicleList[j] == vehicle) {
				genome.push_back(customerList[j]);
			}
		}
	}

	return individualP(new individual(genome, inputProblem));
}

void individualList::checkPPD() {
	int currentVehicle;
	int numOfCustomers = inputProblem->getNumCusto();
	vector<int> currentCustomers;
	int current;
	int pair;
	int indexCurrent = 0;
	int indexPair = 0;
	vector<int> doNotCheck;
	customer* currentCustomer;
	bool makeSwitch = false;

	for (int i = 0; i < inputProblem->getNumVehicles(); i++) {
		currentVehicle = i + numOfCustomers;

		for (int j = 0; j < numOfCustomers; j++) {
			if (vehicleList[j] == currentVehicle) {
				currentCustomers.push_back(customerList[j]);
			}
		}

		for (int j = 0; j < currentCustomers.size(); j++) {
			current = currentCustomers[j];
			auto it = std::find(customerList.begin(), customerList.end(), current);
			indexCurrent = distance(customerList.begin(), it);

			if (std::find(doNotCheck.begin(), doNotCheck.end(), current) == doNotCheck.end()) {
				currentCustomer = &(inputProblem->allCustomer[current - 1]);
				if ((currentCustomer->pickup) != 0) {
					pair = currentCustomer->pickup;
				}
				else {
					pair = currentCustomer->delivery;
				}

				auto it2 = std::find(currentCustomers.begin(), currentCustomers.end(), pair);
				if (it2 == currentCustomers.end()) {	// ako nema para
					auto it3 = std::find(customerList.begin(), customerList.end(), pair);
					indexPair = distance(customerList.begin(), it3);
					if (currentCustomer->pickup == 0) {			// ako je to onaj koji ceka - dobiva vozilo od para
						vehicleList[indexCurrent] = vehicleList[indexPair];
					}
					else {		// ako je onaj koji daje, par dobiva njegovo vozilo
						vehicleList[indexPair] = vehicleList[indexCurrent];
					}
				}

				// provjera jesu li dobrim redoslijedom
				if (currentCustomer->pickup != 0) { // onaj koji daje je current, mora biti prije pair
					if (indexCurrent > indexPair) {
						makeSwitch = true;
					}
				}
				else {			// current prima, mora biti iza pair
					if (indexCurrent < indexPair) {
						makeSwitch = true;
					}
				}
				doNotCheck.push_back(pair);		// da ga ne provjeravamo opet bezveze


				if (makeSwitch) {			// zamjena
					customerList[indexCurrent] = customerList[indexPair];
					customerList[indexPair] = current;
				}
			}
		}

		currentCustomers.clear();
	}
}

void individualList::registerParameters(StateP state) {
	registerParameter(state, "size", (voidP)(new uint(1)), ECF::UINT, "size");
}
