#include "../../ECF/ECF.h"
#include <iostream>
#include "individualPermutation.h"
#include "problem.h"
#include "customer.h"
#include <time.h> 
#include <algorithm>
#include <stdio.h>


individualPermutation::individualPermutation() {
	name_ = "individualPermutation";
}

bool individualPermutation::initialize(StateP state) {
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

	for (int i = 0; i < vehicleNumber; i++) {
		vehicles[i] = customerNumber + i + 1;
	}

	genome.resize(vehicleNumber + customerNumber);
	for (int i = 1; i < vehicleNumber + customerNumber; i++) {
		genome[i] = i;
	}
	genome[0] = vehicleNumber + customerNumber;
	random_shuffle(genome.begin() + 1, genome.end());

	if (inputProblem->isPDVRP == 1) {
		checkPPD();
	}
	
	return true;
}

individualPermutation* individualPermutation::copy() {
	individualPermutation* permutation = new individualPermutation();
	permutation->setGenome(genome);
	permutation->setProblem(inputProblem);
	return permutation;
}

void individualPermutation::write(XMLNode &xMyGenotype) {
	xMyGenotype = XMLNode::createXMLTopNode("individualPermutation");
	stringstream sValue;
	sValue << this->genome.size();
	xMyGenotype.addAttribute("size", sValue.str().c_str());

	sValue.str("");

	// write genome to sValue
	std::vector<int>& genome = this->genome;

	sValue << "Genome: ";
	for (int i = 0; i < genome.size(); i++) {
		sValue << genome[i] << " ";
	}

	xMyGenotype.addText(sValue.str().c_str());
}

void individualPermutation::read(XMLNode& xMyGenotype) {

}

std::vector<MutationOpP> individualPermutation::getMutationOp() {
	std::vector<MutationOpP> mut;
	mut.push_back(static_cast<MutationOpP> (new individualPermutationMutationOp));
	return mut;
}

std::vector<CrossoverOpP> individualPermutation::getCrossoverOp() {
	std::vector<CrossoverOpP> crx;
	crx.push_back(static_cast<CrossoverOpP> (new individualPermutationCrossoverOp));
	return crx;
}

problem* individualPermutation::getProblem() {
	return inputProblem;
}

void individualPermutation::setProblem(problem* problem) {
	inputProblem = problem;
}

void individualPermutation::setGenome(std::vector<int> gen) {
	genome = gen;
}

std::vector<int> individualPermutation::getGenome() {
	return genome;
}

individualP individualPermutation::inBetweenStep() {
	return individualP(new individual(genome, inputProblem));
}

void individualPermutation::checkPPD() {
	int currentVehicle = genome[0];
	int currentVehicleIndex = 0;
	vector<int> currentCustomers;
	customer* currentCustomer;
	int current;
	int pair;
	int indexPair = 0;
	int indexCurrent = 0;
	bool makeSwitch = false;
	vector<int> copyGenome = genome;
	vector<int> doNotCheck;

	for (int i = 1; i < copyGenome.size(); i++) {
		if (copyGenome[i] > inputProblem->getNumCusto()) {		// dosli smo do novog vozila - radimo provjeru
			currentVehicle = copyGenome[i];
			auto thisVehicle = std::find(genome.begin(), genome.end(), currentVehicle);
			currentVehicleIndex = distance(genome.begin(), thisVehicle);
			for (int j = 0; j < currentCustomers.size(); j++) {	
				current = currentCustomers[j];
				auto it = std::find(genome.begin(), genome.end(), current);
				indexCurrent = distance(genome.begin(), it);

				if (std::find(doNotCheck.begin(), doNotCheck.end(), current) == doNotCheck.end()) {
					currentCustomer = &(inputProblem->allCustomer[current - 1]);
						if ((currentCustomer->pickup) != 0) {
							pair = currentCustomer->pickup;
						}
						else {
							pair = currentCustomer->delivery;
						}

						auto it2 = std::find(genome.begin(), genome.end(), pair);
						indexPair = distance(genome.begin(), it2);

						auto it3 = std::find(currentCustomers.begin(), currentCustomers.end(), pair);
						if (it3 == currentCustomers.end()) {	// ako nema para
							if (currentCustomer->pickup == 0) {			// ako je to onaj koji ceka - brisemo ga
								genome.insert(genome.begin() + indexPair + 1, current);
								genome.erase(genome.begin() + indexCurrent);
								doNotCheck.push_back(pair);
							}
							else {		// ako je onaj koji daje, dodajemo mu para na kraj tog vozila
								if (it2 != genome.end()) {
									genome.erase(genome.begin() + indexPair);
								}
								genome.insert(genome.begin() + currentVehicleIndex, pair);
								indexPair = i;
								doNotCheck.push_back(pair);
							}
						}
						else {			// nasli smo para, provjera jesu li dobrim redoslijedom
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
						}

					if (makeSwitch) {			// zamjena
						it = std::find(genome.begin(), genome.end(), current);
						indexCurrent = distance(genome.begin(), it);
						it2 = std::find(genome.begin(), genome.end(), pair);
						indexPair = distance(genome.begin(), it2);

						genome[indexCurrent] = pair;
						genome[indexPair] = current;
						makeSwitch = false;
					}
				}
			}

			currentCustomers.clear();
		}
		else {
			currentCustomers.push_back(copyGenome[i]);
		}
	}
}

void individualPermutation::registerParameters(StateP state) {
	registerParameter(state, "size", (voidP)(new uint(1)), ECF::UINT, "size");
}
