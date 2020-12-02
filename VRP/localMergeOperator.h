#ifndef ZAVRSNI_LOCALMERGEOPERATOR_H
#define ZAVRSNI_LOCALMERGEOPERATOR_H

#include "../../ECF/ECF.h"
#include <string>
#include "individualMap.h"
#include "individualList.h"
#include "localSearchEvalOp.h"
#include "individualPermutation.h"

class localMergeOperator : public Operator {
public:
	uint frequency;

	void registerParameters(StateP state) {
		state->getRegistry()->registerEntry("my.frequency", (voidP)(new uint(0)), ECF::UINT,
			"local operator frequency");
	}

	bool initialize(StateP state) {
		if (!state->getRegistry()->isModified("my.frequency")) {
			return false;
		}
		else {
			voidP frequencyPtr = state->getRegistry()->getEntry("my.frequency");
			frequency = *((uint*)frequencyPtr.get());
			return true;
		}
	}

	bool operate(StateP state) {
		if (state->getGenerationNo() % frequency == 0) {
			DemeP deme = state->getPopulation()->getLocalDeme();
			IndividualP ind = deme->hof_->getBest()[0];  // ako cemo uzimati najbolju iz hall of fame
			int indIndex = rand() % deme->size();

			GenotypeP gen = ind->getGenotype();
			FitnessP fitness;
			localSearchEvalOp evalOp;
			double fitness1;
			double fitness2;

			if (gen->getName() == "individualMap") {
				IndividualP indBefore;
				IndividualP newIndividual;
				localSearchEvalOp evalOp;
				indBefore = (IndividualP)ind->copy();
				fitness = state->getEvalOp()->evaluate(ind);
				fitness1 = evalOp.evaluate(ind).get()->getValue();

				individualMap* indG = (individualMap*)gen.get();
				individualP indGen = indG->inBetweenstep();
				int min = indGen->getGenome().size() + 1;
				int current = 0;
				int currentVehicle = indGen->getGenome()[0];
				int indexMinVehicle = indGen->getGenome()[0];
				for (int i = 1; i <= indGen->getGenome().size(); i++) {
					if ((i == indGen->getGenome().size()) || (indGen->getGenome()[i] > indG->getProblem()->getNumCusto())) {
						if (current < min && current != 0) {
							min = current;
							indexMinVehicle = currentVehicle;
						}
						current = 0;
						if (i < indGen->getGenome().size()) {
							currentVehicle = indGen->getGenome()[i];
						}
					}
					else {
						current++;
					}
				}
				min = indGen->getGenome().size() + 1;
				current = 0;
				currentVehicle = indGen->getGenome()[0];
				int indexSecondVehicle = indGen->getGenome()[0];
				for (int i = 1; i <= indGen->getGenome().size(); i++) {
					if ((i == indGen->getGenome().size()) || (indGen->getGenome()[i] > indG->getProblem()->getNumCusto())) {
						if (current < min && current != 0 && currentVehicle != indexMinVehicle) {
							min = current;
							indexSecondVehicle = currentVehicle;
						}
						current = 0;
						if (i < indGen->getGenome().size()) {
							currentVehicle = indGen->getGenome()[i];
						}
					}
					else {
						current++;
					}
				}

				if (indexMinVehicle != indexSecondVehicle && min < indGen->getGenome().size() + 1) {
					std::vector<int>::iterator it1;
					std::vector<int>::iterator it2;
					individual* indcopy = indGen->copy();
					std::vector<int> copyGenome = indGen->getGenome();
					it1 = std::find(copyGenome.begin(), copyGenome.end(), indexSecondVehicle);
					int secondVehicle = std::distance(copyGenome.begin(), it1);

					it2 = std::find(copyGenome.begin(), copyGenome.end(), indexMinVehicle);
					int firstVehicle = std::distance(copyGenome.begin(), it2);

					it1++;
					for (int i = firstVehicle + 1; i < indcopy->getGenome().size() && indcopy->getGenome()[i] <= indG->getProblem()->getNumCusto(); i++) {
						copyGenome.insert(it1, indcopy->getGenome()[i]);
						it1 = std::find(copyGenome.begin(), copyGenome.end(), indcopy->getGenome()[i]);
					}
					it2 = std::find(copyGenome.begin(), copyGenome.end(), indexMinVehicle);
					it2++;
					for (int i = firstVehicle + 1; i < indcopy->getGenome().size() && indcopy->getGenome()[i] <= indG->getProblem()->getNumCusto(); i++) {
						copyGenome.erase(it2);
						it2 = std::find(copyGenome.begin(), copyGenome.end(), indexMinVehicle);
						it2++;
					}

					indGen->setGenome(copyGenome);
					gen = (GenotypeP)indGen;
					ind->getGenotype() = gen;
					fitness2 = (state->getEvalOp()->evaluate(ind)).get()->getValue();

					newIndividual = (IndividualP)ind->copy();
					newIndividual->fitness.swap(state->getEvalOp()->evaluate(ind));

					if (fitness1 < fitness2) {
						state->getPopulation()->getLocalDeme()->replace(indIndex, indBefore);
					}
					else {
						state->getPopulation()->getLocalDeme()->replace(indIndex, newIndividual);
					}
				}

				state->getPopulation()->updateDemeStats();

				return true;
			}
			else if (gen->getName() == "individualList") {
				IndividualP indBefore;
				IndividualP newIndividual;
				localSearchEvalOp evalOp;
				indBefore = (IndividualP)ind->copy();
				fitness = state->getEvalOp()->evaluate(ind);
				fitness1 = evalOp.evaluate(ind).get()->getValue();

				individualList* indG = (individualList*)gen.get();
				individualP indGen = indG->inBetweenStep();
				int min = indGen->getGenome().size() + 1;
				int current = 0;
				int currentVehicle = indGen->getGenome()[0];
				int indexMinVehicle = indGen->getGenome()[0];
				for (int i = 1; i <= indGen->getGenome().size(); i++) {
					if ((i == indGen->getGenome().size()) || (indGen->getGenome()[i] > indG->getProblem()->getNumCusto())) {
						if (current < min && current != 0) {
							min = current;
							indexMinVehicle = currentVehicle;
						}
						current = 0;
						if (i < indGen->getGenome().size()) {
							currentVehicle = indGen->getGenome()[i];
						}
					}
					else {
						current++;
					}
				}
				min = indGen->getGenome().size() + 1;
				current = 0;
				currentVehicle = indGen->getGenome()[0];
				int indexSecondVehicle = indGen->getGenome()[0];
				for (int i = 1; i <= indGen->getGenome().size(); i++) {
					if ((i == indGen->getGenome().size()) || (indGen->getGenome()[i] > indG->getProblem()->getNumCusto())) {
						if (current < min && current != 0 && currentVehicle != indexMinVehicle) {
							min = current;
							indexSecondVehicle = currentVehicle;
						}
						current = 0;
						if (i < indGen->getGenome().size()) {
							currentVehicle = indGen->getGenome()[i];
						}
					}
					else {
						current++;
					}
				}

				if (indexMinVehicle != indexSecondVehicle && min < indGen->getGenome().size() + 1) {
					std::vector<int>::iterator it1;
					std::vector<int>::iterator it2;
					individual* indcopy = indGen->copy();
					std::vector<int> copyGenome = indGen->getGenome();
					it1 = std::find(copyGenome.begin(), copyGenome.end(), indexSecondVehicle);
					int secondVehicle = std::distance(copyGenome.begin(), it1);

					it2 = std::find(copyGenome.begin(), copyGenome.end(), indexMinVehicle);
					int firstVehicle = std::distance(copyGenome.begin(), it2);

					it1++;
					for (int i = firstVehicle + 1; i < indcopy->getGenome().size() && indcopy->getGenome()[i] <= indG->getProblem()->getNumCusto(); i++) {
						copyGenome.insert(it1, indcopy->getGenome()[i]);
						it1 = std::find(copyGenome.begin(), copyGenome.end(), indcopy->getGenome()[i]);
					}
					it2 = std::find(copyGenome.begin(), copyGenome.end(), indexMinVehicle);
					it2++;
					for (int i = firstVehicle + 1; i < indcopy->getGenome().size() && indcopy->getGenome()[i] <= indG->getProblem()->getNumCusto(); i++) {
						copyGenome.erase(it2);
						it2 = std::find(copyGenome.begin(), copyGenome.end(), indexMinVehicle);
						it2++;
					}

					indGen->setGenome(copyGenome);
					gen = (GenotypeP)indGen;
					ind->getGenotype() = gen;
					fitness2 = (state->getEvalOp()->evaluate(ind)).get()->getValue();

					newIndividual = (IndividualP)ind->copy();
					newIndividual->fitness.swap(state->getEvalOp()->evaluate(ind));

					if (fitness1 < fitness2) {
						state->getPopulation()->getLocalDeme()->replace(indIndex, indBefore);
					}
					else {
						state->getPopulation()->getLocalDeme()->replace(indIndex, newIndividual);
					}
				}

				state->getPopulation()->updateDemeStats();

				return true;
			}
			else if (gen->getName() == "individualPermutation") {
				IndividualP indBefore;
				IndividualP newIndividual;

				localSearchEvalOp evalOp;
				indBefore = (IndividualP)ind->copy();
				fitness = state->getEvalOp()->evaluate(ind);
				fitness1 = evalOp.evaluate(ind).get()->getValue();

				individualPermutation* indGen = (individualPermutation*)gen.get();

				int min = indGen->getGenome().size() + 1;
				int current = 0;
				int currentVehicle = indGen->getGenome()[0];
				int indexMinVehicle = indGen->getGenome()[0];
				for (int i = 1; i <= indGen->getGenome().size(); i++) {
					if ((i== indGen->getGenome().size()) || (indGen->getGenome()[i] > indGen->getProblem()->getNumCusto())) {
						if (current < min && current != 0) {
							min = current;
							indexMinVehicle = currentVehicle;
						}
						current = 0;
						if (i < indGen->getGenome().size()) {
							currentVehicle = indGen->getGenome()[i];
						}
					}
					else {
						current++;
					}
				} 
				min = indGen->getGenome().size() + 1;
				current = 0;
				currentVehicle = indGen->getGenome()[0];
				int indexSecondVehicle = indGen->getGenome()[0];
				for (int i = 1; i <= indGen->getGenome().size(); i++) {
					if ((i == indGen->getGenome().size()) || (indGen->getGenome()[i] > indGen->getProblem()->getNumCusto())) {
						if (current < min && current != 0 && currentVehicle!= indexMinVehicle) {
							min = current;
							indexSecondVehicle = currentVehicle;
						}
						current = 0;
						if (i < indGen->getGenome().size()) {
							currentVehicle = indGen->getGenome()[i];
						}
					}
					else {
						current++;
					}
				}

				if (indexMinVehicle != indexSecondVehicle && min < indGen->getGenome().size() + 1) {
					std::vector<int>::iterator it1;
					std::vector<int>::iterator it2;
					individualPermutation* indcopy = indGen->copy();
					std::vector<int> copyGenome = indGen->getGenome();
					it1 = std::find(copyGenome.begin(), copyGenome.end(), indexSecondVehicle);
					int secondVehicle = std::distance(copyGenome.begin(), it1);

					it2 = std::find(copyGenome.begin(), copyGenome.end(), indexMinVehicle);
					int firstVehicle = std::distance(copyGenome.begin(), it2);

					it1++;
					for (int i = firstVehicle + 1; i < indcopy->getGenome().size() && indcopy->getGenome()[i] <= indGen->getProblem()->getNumCusto(); i++) {
						copyGenome.insert(it1, indcopy->getGenome()[i]);
						it1 = std::find(copyGenome.begin(), copyGenome.end(), indcopy->getGenome()[i]);
					}
					it2 = std::find(copyGenome.begin(), copyGenome.end(), indexMinVehicle);
					it2++;
					for (int i = firstVehicle + 1; i < indcopy->getGenome().size() && indcopy->getGenome()[i] <= indGen->getProblem()->getNumCusto(); i++) {
						copyGenome.erase(it2);
						it2 = std::find(copyGenome.begin(), copyGenome.end(), indexMinVehicle);
						it2++;
					}

					indGen->setGenome(copyGenome);
					fitness2 = (state->getEvalOp()->evaluate(ind)).get()->getValue();

					newIndividual = (IndividualP)ind->copy();
					newIndividual->fitness.swap(state->getEvalOp()->evaluate(ind));
		
					if (fitness1 < fitness2) {
						state->getPopulation()->getLocalDeme()->replace(indIndex, indBefore);
					}
					else {
						state->getPopulation()->getLocalDeme()->replace(indIndex, newIndividual);
					}
					indGen = NULL;
				}
			}

			state->getPopulation()->updateDemeStats();

			return true;
		}
		return true;
	}
};

typedef boost::shared_ptr<localMergeOperator> localMergeOperatorP;

#endif //ZAVRSNI_LOCALMERGEOPERATOR_H
