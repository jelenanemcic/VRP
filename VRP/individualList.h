#ifndef INDIVIDUALLIST_H
#define INDIVIDUALLIST_H

#include "../../ECF/ECF.h"
#include "customer.h"
#include "problem.h"
#include "individual.h"

class individualList : public Genotype {
public:
	individualList();
	bool initialize(StateP state);
	individualList* copy();
	void write(XMLNode &xMyGenotype);
	void read(XMLNode& xMyGenotype);
	std::vector<MutationOpP> getMutationOp();
	std::vector<CrossoverOpP> getCrossoverOp();
	void print();
	problem* getProblem();
	void setProblem(problem* problem);
	std::vector<int> getCustomerList();
	std::vector<int> getVehicleList();
	void setCustomerList(std::vector<int> customerList);
	void setVehicleList(std::vector<int> vehicleList);
	individualP inBetweenStep();
	void checkPPD();
	void registerParameters(StateP state);

private:
	std::vector<int> customerList;
	std::vector<int> vehicleList;
	problem* inputProblem;
};
typedef boost::shared_ptr<individualList> individualListP;


class individualListMutationOp : public MutationOp {

public:
	bool mutate(GenotypeP gene) {
		individualList* individual = (individualList*)gene.get();

		int customerNumber = individual->getProblem()->getNumCusto();
		int vehicleNumber = individual->getProblem()->getNumVehicles();
		std::vector<int> customerList = individual->getCustomerList();
		std::vector<int> vehicleList = individual->getVehicleList();

		// rand() % (max_number + 1 - minimum_number) + minimum_number

		int mutationNumber1;
		int mutationNumber2;

		do {
			mutationNumber1 = rand() % customerNumber;
			mutationNumber2 = rand() % customerNumber;
		} while (mutationNumber1 == mutationNumber2);

		int random = rand() % 2;
		int geneSwitch;

		if (random == 0) {
			geneSwitch = customerList[mutationNumber1];
			customerList[mutationNumber1] = customerList[mutationNumber2];
			customerList[mutationNumber2] = geneSwitch;
		}
		else {
			geneSwitch = rand() % vehicleNumber + customerNumber + 1;
			vehicleList[mutationNumber1] = geneSwitch;
		}

		individual->setCustomerList(customerList);
		individual->setVehicleList(vehicleList);

		if (individual->getProblem()->isPDVRP == 1) {
			individual->checkPPD();
		}

		return true;
	}
};
typedef boost::shared_ptr<individualListMutationOp> individualListMutationOpP;


class individualListCrossoverOp : public CrossoverOp {

public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child) {
		individualList* parent1 = (individualList*)gen1.get();
		individualList* parent2 = (individualList*)gen2.get();
		individualList* childInd = (individualList*)child.get();

		int customerNumber = parent1->getProblem()->getNumCusto();
		int vehicleNumber = parent1->getProblem()->getNumVehicles();

		childInd->setProblem(parent1->getProblem());

		std::vector<int> parent1CustomerList = parent1->getCustomerList();
		std::vector<int> parent1VehicleList = parent1->getVehicleList();
		std::vector<int> parent2CustomerList = parent2->getCustomerList();
		std::vector<int> parent2VehicleList = parent2->getVehicleList();

		int crossoverIndexStart = rand() % (customerNumber - 1);
		int crossoverIndexEnd = rand() % (customerNumber - (crossoverIndexStart + 1)) + crossoverIndexStart + 1;

		std::vector<int> childCustomerList(parent1CustomerList.begin() + crossoverIndexStart, parent1CustomerList.begin() + crossoverIndexEnd);
		std::vector<int> childVehicleList(parent1VehicleList.begin() + crossoverIndexStart, parent1VehicleList.begin() + crossoverIndexEnd);

		for (int i = 0; i < customerNumber; i++) {
			if (std::find(childCustomerList.begin(), childCustomerList.end(), parent2CustomerList[i]) == childCustomerList.end()) {
				childCustomerList.push_back(parent2CustomerList[i]);
				childVehicleList.push_back(parent2VehicleList[i]);
			}
		}

		childInd->setCustomerList(childCustomerList);
		childInd->setVehicleList(childVehicleList);

		if (childInd->getProblem()->isPDVRP == 1) {
			childInd->checkPPD();
		}

		return true;
	}
};
typedef boost::shared_ptr<individualListCrossoverOp> individualListCrossoverOpP;
#endif
