#ifndef INDIVIDUALPERMUTATION_H
#define INDIVIDUALPERMUTATION_H

#include "../../ECF/ECF.h"
#include "customer.h"
#include "problem.h"
#include "individual.h"
#include <time.h> 
#include <algorithm>
#include <stdio.h>
#include <iostream>

class individualPermutation : public Genotype {
public:
	individualPermutation();
	bool initialize(StateP state);
	individualPermutation* copy();
	void write(XMLNode &xMyGenotype);
	void read(XMLNode& xMyGenotype);
	std::vector<MutationOpP> getMutationOp();
	std::vector<CrossoverOpP> getCrossoverOp();
	problem* getProblem();
	void setProblem(problem* problem);
	void setGenome(std::vector <int> genome);
	std::vector<int> getGenome();
	individualP inBetweenStep();
	void checkPPD();
	void registerParameters(StateP state);

private:
	std::vector<int> genome;
	problem* inputProblem;
};
typedef boost::shared_ptr<individualPermutation> individualPermutationP;


class individualPermutationMutationOp : public MutationOp {

public:
	bool mutate(GenotypeP gene) {
		individualPermutation* individual = (individualPermutation*)gene.get();

		int customerNumber = individual->getProblem()->getNumCusto();
		int vehicleNumber = individual->getProblem()->getNumVehicles();
		std::vector<int> genome = individual->getGenome();

		int mutationNumber1;
		int mutationNumber2;
		int mutationGene1;
		int mutationGene2;
		std::vector<int>::iterator it;

		// rand() % (max_number + 1 - minimum_number) + minimum_number

		do {
			mutationGene1 = rand() % customerNumber + 1;
			mutationGene2 = rand() % customerNumber + 1;
		} while (mutationGene1 == mutationGene2);

		it = std::find(genome.begin(), genome.end(), mutationGene1);
		mutationNumber1 = std::distance(genome.begin(), it);

		it = std::find(genome.begin(), genome.end(), mutationGene2);
		mutationNumber2 = std::distance(genome.begin(), it);

		genome[mutationNumber1] = mutationGene2;
		genome[mutationNumber2] = mutationGene1;

		individual->setGenome(genome);

		if (individual->getProblem()->isPDVRP == 1) {
			individual->checkPPD();
		}

		return true;
	}	
};
typedef boost::shared_ptr<individualPermutationMutationOp> individualPermutationMutationOpP;


class individualPermutationCrossoverOp : public CrossoverOp {

public:
	bool mate(GenotypeP gen1, GenotypeP gen2, GenotypeP child) {
		individualPermutation* parent1 = (individualPermutation*)gen1.get();
		individualPermutation* parent2 = (individualPermutation*)gen2.get();
		individualPermutation* childInd = (individualPermutation*)child.get();

		int customerNumber = parent1->getProblem()->getNumCusto();
		int vehicleNumber = parent1->getProblem()->getNumVehicles();

		childInd->setProblem(parent1->getProblem());

		std::vector<int> parent1Genome = parent1->getGenome();
		std::vector<int> parent2Genome = parent2->getGenome();
		int length = parent1Genome.size();

		int crossoverIndexStart = rand() % (length - 1);
		int crossoverIndexEnd = rand() % (length - crossoverIndexStart) + crossoverIndexStart;

		std::vector<int> childGenome(parent1Genome.begin() + crossoverIndexStart, parent1Genome.begin() + crossoverIndexEnd);

		for (int i = 0; i < length; i++) {
			if (std::find(childGenome.begin(), childGenome.end(), parent2Genome[i]) == childGenome.end()) {
				childGenome.push_back(parent2Genome[i]);
			}
		}

		if (childGenome[0] <= customerNumber) {
			int i = 1;
			while (childGenome[i] <= customerNumber) {
				i++;
			}
			iter_swap(childGenome.begin(), childGenome.begin() + i);
		}

		childInd->setGenome(childGenome);

		if (childInd->getProblem()->isPDVRP == 1) {
			childInd->checkPPD();
		}

		return true;
	}
};
typedef boost::shared_ptr<individualPermutationCrossoverOp> individualPermutationCrossoverOpP;
#endif
