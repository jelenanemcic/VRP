#ifndef ZAVRSNI_LOCALSEARCHOPERATOR_H
#define ZAVRSNI_LOCALSEARCHOPERATOR_H

#include "../../ECF/ECF.h"
#include <string>
#include "individualMap.h"
#include "individualList.h"
#include "localSearchEvalOp.h"
#include "individualPermutation.h"

class localSearchOperator : public Operator {
public:

	uint frequency;
	uint budget;
	uint attempts;
	uint attemptsCopy;

	void registerParameters(StateP state) {
		state->getRegistry()->registerEntry("my.frequency", (voidP)(new uint(0)), ECF::UINT,
			"local operator frequency");
		state->getRegistry()->registerEntry("my.budget", (voidP)(new uint(0)), ECF::UINT,
			"max number of local searches to run");
		state->getRegistry()->registerEntry("my.attempts", (voidP)(new uint(0)), ECF::UINT,
			"number of attempts in a sequence");
	}

	bool initialize(StateP state) {
		if (!state->getRegistry()->isModified("my.budget") || !state->getRegistry()->isModified("my.frequency")
			 || !state->getRegistry()->isModified("my.attempts")) {
			return false;
		}
		else {
			voidP frequencyPtr = state->getRegistry()->getEntry("my.frequency");
			voidP budgetPtr = state->getRegistry()->getEntry("my.budget");
			voidP attemptsPtr = state->getRegistry()->getEntry("my.attempts");
			frequency = *((uint *)frequencyPtr.get());
			budget = *((uint *)budgetPtr.get());
			attempts = *((uint*)attemptsPtr.get());
			attemptsCopy = attempts;
			return true;
		}
	}

	bool operate(StateP state) {
		if (state->getGenerationNo() % frequency == 0) {
			DemeP deme = state->getPopulation()->getLocalDeme();
			IndividualP ind =deme->hof_->getBest()[0];  // ako cemo uzimati najbolju iz hall of fame
			int indIndex = rand() % deme->size();
		//	IndividualP ind = deme->at(indIndex);
			GenotypeP gen = ind->getGenotype();
			FitnessP fitness;
			localSearchEvalOp evalOp;
			int iteration = 0;
			double fitness1;
			double fitness2;

			if (gen->getName() == "individualMap") {
				individualMapMutationOp mapMutationOperator;
				IndividualP indBeforeMutation;

				while (iteration < budget) {
					indBeforeMutation = (IndividualP) ind->copy();
					fitness = state->getEvalOp()->evaluate(ind);
					fitness1 = evalOp.evaluate(ind).get()->getValue();
					mapMutationOperator.mutate(ind->getGenotype());
					fitness2 = evalOp.evaluate(ind).get()->getValue();
					iteration++;
					if (fitness1 < fitness2) {
						if (attemptsCopy == 0) {
							attemptsCopy = attempts;
							state->getPopulation()->getLocalDeme()->replace(indIndex, indBeforeMutation);
							break;
						}
						else {
							attemptsCopy--;
							ind = indBeforeMutation;
						}
					}
				}
			}
			else if (gen->getName() == "individualList") {
				individualListMutationOp listMutationOperator;
				IndividualP indBeforeMutation;

				while (iteration < budget) {
					indBeforeMutation = (IndividualP) ind->copy();
					fitness = state->getEvalOp()->evaluate(ind);
					fitness1 = evalOp.evaluate(ind).get()->getValue();
					listMutationOperator.mutate(ind->getGenotype());
					fitness2 = evalOp.evaluate(ind).get()->getValue();
					iteration++;
					if (fitness1 < fitness2) {
						if (attemptsCopy == 0) {
							attemptsCopy = attempts;
							state->getPopulation()->getLocalDeme()->replace(indIndex, indBeforeMutation);
							break;
						}
						else {
							attemptsCopy--;
							ind = indBeforeMutation;
						}
					}
				}

			}
			else if (gen->getName() == "individualPermutation") {
				individualPermutationMutationOp permutationMutationOperator;
				IndividualP indBeforeMutation;

				while (iteration < budget) {
					localSearchEvalOp evalOp;
					indBeforeMutation = (IndividualP) ind->copy();
					fitness = state->getEvalOp()->evaluate(ind);
					fitness1 = evalOp.evaluate(ind).get()->getValue();
					permutationMutationOperator.mutate(ind->getGenotype());
					fitness2 = evalOp.evaluate(ind).get()->getValue();
					iteration++;
					if (fitness1 < fitness2) {
						if (attemptsCopy == 0) {
							attemptsCopy = attempts;
							state->getPopulation()->getLocalDeme()->replace(indIndex, indBeforeMutation);
							break;
						}
						else {
							attemptsCopy--;
							ind = indBeforeMutation;
						}
					}
				}
			}

			state->getPopulation()->updateDemeStats();

			return true;
		}
		return true;
	}
};

typedef boost::shared_ptr<localSearchOperator> localSearchOperatorP;

#endif //ZAVRSNI_LOCALSEARCHOPERATOR_H
