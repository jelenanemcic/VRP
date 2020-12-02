#ifndef INDIVIDUAL_H
#define INDIVIDUAL_H

#include "../../ECF/ECF.h"
#include "customer.h"
#include "problem.h"

class individual : public Genotype {
public:
	individual();
	bool initialize(StateP state);
	void write(XMLNode& xMyGenotype);
	void read(XMLNode& xMyGenotype);
	individual(std::vector<int> genome, problem* input);
	void setGenome(std::vector <int> genome);
	void setFitness(double fitness);
	std::vector<int> getGenome();
	individual* copy();
	double getFitness();
	double fitnessFunction();

private:
	double fitness;
	std::vector<int> genome;
	problem* input;
};
typedef boost::shared_ptr<individual> individualP;
#endif