#ifndef INDIVIDUALMAP_H
#define INDIVIDUALMAP_H

#include "../../ECF/ECF.h"
#include "problem.h"
#include <map>
#include <random>

class individualMap : public Genotype {
public:
	individualMap();
	individualMap* copy();
	bool initialize(StateP state);
	void write(XMLNode &xIndividualMap);
	void read(XMLNode& xMyGenotype);
	std::vector<MutationOpP> getMutationOp();
	std::vector<CrossoverOpP> getCrossoverOp();
	individualP inBetweenstep();
	problem* getProblem();
	void setProblem(problem* problem);
	map<string, vector<int>> getGenome();
	void setGenome(map<string, vector<int>> genome);
	void checkPPD();
	void registerParameters(StateP state);

private:
	map<string, vector<int>> genome;
	problem* inputProblem;
};
typedef boost::shared_ptr<individualMap> individualMapP;


class individualMapMutationOp : public MutationOp {

public:
	bool mutate(GenotypeP gene) {
		individualMap* individual = (individualMap*)gene.get();

		int customerNumber = individual->getProblem()->getNumCusto();
		int vehicleNumber = individual->getProblem()->getNumVehicles();
		map<string, vector<int>> genome = individual->getGenome();

		string indexVehicleOne;
		string indexVehicleTwo;
		int firstCustomerIndex;
		int secondCustomerIndex;
		int firstCustomer;
		int secondCustomer;
		vector<int> customersVehicle1;
		vector<int> customersVehicle2;

		do {
			auto it = genome.begin();
			std::advance(it, rand() % genome.size());
			indexVehicleOne = it->first; //v12
			customersVehicle1 = genome.at(indexVehicleOne);
		} while (customersVehicle1.empty());

		do {
			auto it = genome.begin();
			std::advance(it, rand() % genome.size());
			indexVehicleTwo = it->first; //v12
			customersVehicle2 = genome.at(indexVehicleTwo);
		} while ((indexVehicleOne == indexVehicleTwo && genome.at(indexVehicleOne).size() == 1) || customersVehicle2.empty());

		do {
			firstCustomerIndex = static_cast<int>(rand() % customersVehicle1.size());
			firstCustomer = customersVehicle1.at(firstCustomerIndex);
			if (!customersVehicle2.empty()) {
				secondCustomerIndex = static_cast<int>(rand() % customersVehicle2.size());
				secondCustomer = customersVehicle2.at(secondCustomerIndex);
			}
			else {
				secondCustomerIndex = 0;
			}
		} while ((indexVehicleOne == indexVehicleTwo) && (firstCustomerIndex == secondCustomerIndex));

		if (secondCustomer == NULL) {
			genome.at(indexVehicleTwo).push_back(firstCustomer);
			genome.at(indexVehicleOne).erase(genome.at(indexVehicleOne).begin() + firstCustomerIndex);
		}
		else {
			genome.at(indexVehicleOne).at(firstCustomerIndex) = secondCustomer;
			genome.at(indexVehicleTwo).at(secondCustomerIndex) = firstCustomer;
		}

		std::map<std::string, std::vector<int>>::iterator iterator;

		individual->setGenome(genome);

		if (individual->getProblem()->isPDVRP == 1) {
			individual->checkPPD();
		}

		return true;
	}
};
typedef boost::shared_ptr<individualMapMutationOp> individualMapMutationOpP;


class individualMapCrossoverOp : public CrossoverOp {

public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child) {
		individualMap* parent1 = (individualMap*)gen1.get();
		individualMap* parent2 = (individualMap*)gen2.get();
		individualMap* childInd = (individualMap*)child.get();
		std::map <std::string, std::vector<int>> parent1Genome = parent1->getGenome();
		std::map <std::string, std::vector<int>> parent2Genome = parent2->getGenome();

		int numberOfCustomers = parent1->getProblem()->getNumCusto();
		int numberOfVehicles = parent1->getProblem()->getNumVehicles();

		childInd->setProblem(parent1->getProblem());

		std::vector<int> customers1;
		std::vector<int> customers2;
		std::map <std::string, std::vector<int>> childGenome;

		std::map<std::string, std::vector<int>>::iterator iterator;
		for (iterator = parent1Genome.begin(); iterator != parent1Genome.end(); iterator++) {
			for (int i = 0; i < iterator->second.size(); i++) {
				customers1.push_back(iterator->second[i]);
			}
		}

		int customer1BreakIndex = static_cast<int>(rand() % customers1.size());

		for (iterator = parent2Genome.begin(); iterator != parent2Genome.end(); iterator++) {
			for (int i = 0; i < iterator->second.size(); i++) {
				customers2.push_back(iterator->second[i]);
			}
		}
		std::vector<int> indexedVector(customers1.begin(), customers1.begin() + customer1BreakIndex);

		for (int i = 0; i < customers2.size(); i++) {
			if (std::find(indexedVector.begin(), indexedVector.end(), customers2[i]) == indexedVector.end()) {
				indexedVector.push_back(customers2[i]);
			}
		}

		std::vector<int> randomIndexes;

		std::random_device rd;
		std::mt19937 eng(rd());
		std::uniform_int_distribution<> distr(1, numberOfCustomers);

		while (randomIndexes.size() != numberOfCustomers) {
			int candidate = distr(eng);
			if (candidate == 0) continue;
			if (std::find(randomIndexes.begin(), randomIndexes.end(), candidate) != randomIndexes.end()) {
				continue;
			}
			else {
				randomIndexes.push_back(candidate);
			}
		}

		std::vector<int> randomIndexesCopy = randomIndexes;
	
		bool lastIteration = false;
		for (int i = numberOfCustomers + 1; i <= numberOfCustomers + numberOfVehicles; i++) {
			if (i == numberOfCustomers + numberOfVehicles) lastIteration = true;
			std::uniform_int_distribution<> distr(0, randomIndexesCopy.size() / 2);
			int rndIndex = distr(eng);
			if (lastIteration) {
				rndIndex = randomIndexesCopy.size();
			}
			std::vector<int> vectorCustomers;
			for (int i = 0; i < rndIndex; i++) {
				vectorCustomers.push_back(randomIndexesCopy[i]);
			}
			randomIndexesCopy.erase(randomIndexesCopy.begin(), randomIndexesCopy.begin() + rndIndex);
			childGenome[std::to_string(i)] = vectorCustomers;
		}

		childInd->setGenome(childGenome);

		if (childInd->getProblem()->isPDVRP == 1) {
			childInd->checkPPD();
		}

		return true;
	}
};
typedef boost::shared_ptr<individualMapCrossoverOp> individualMapCrossoverOpP;
#endif

