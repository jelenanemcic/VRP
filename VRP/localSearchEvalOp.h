#ifndef LOCALSEARCHEVALOP_H
#define LOCALSEARCHEVALOP_H

#include "../../ECF/ECF.h"
#include "individualMap.h"
#include "individualList.h"
#include "individualPermutation.h"
#include "individual.h"
#include "problem.h"


class localSearchEvalOp : public EvaluateOp {

public:
	void registerParameters(StateP state) {
		state->getRegistry()->registerEntry("vrp.infile", (voidP)(new std::string), ECF::STRING, "file to parse");
		state->getContext()->environment = NULL;
	}

	bool initialize(StateP state) {
		if (!state->getRegistry()->isModified("vrp.infile")) {
			return false;
		}
		return true;
	}

	FitnessP evaluate(IndividualP individual) override {
		GenotypeP gen = individual->getGenotype();
		FitnessP fitness (new FitnessMin);
		if (gen->getName() == "individualMap") {
			auto *map = (individualMap *)(gen.get());
			fitness.get()->setValue(map->inBetweenstep()->fitnessFunction());
		}
		else if (gen->getName() == "individualList") {
			auto *list = (individualList *)(gen.get());
			fitness.get()->setValue(list->inBetweenStep()->fitnessFunction());
		}
		else if (gen->getName() == "individualPermutation") {
			auto *permutation = (individualPermutation *)(gen.get());
			fitness.get()->setValue(permutation->inBetweenStep()->fitnessFunction());
		}
		return fitness;
	}
};
typedef boost::shared_ptr<localSearchEvalOp> localSearchEvalOpP;

#endif
