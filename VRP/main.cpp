#include "../../ECF/ECF.h"
#include <iostream>
#include <time.h> 
#include "problem.h"
#include "individual.h"
#include "individualList.h"
#include "localSearchOperator.h"
#include "localMergeOperator.h"

using namespace std;

int main(int argc, char *argv[]) {

	srand((unsigned)time(NULL));

	StateP state(new State);
	
	individualPermutationP ind(new individualPermutation);
	localSearchOperatorP localSearch(new localSearchOperator);
	localMergeOperatorP localMerge(new localMergeOperator);
	
	state->addGenotype(ind);
	state->setEvalOp(new localSearchEvalOp);
	state->addOperator(localMerge);
	state->initialize(argc, argv);
	state->run();
	
	return 0;
}