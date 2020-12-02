#include "../../ECF/ECF.h"
#include <iostream>
#include <string.h>
#include <random>
#include "individual.h"
#include "individualMap.h"

individualMap::individualMap() {
	name_ = "individualMap";
}

bool individualMap::initialize(StateP state) {
	if (state->getContext()->environment == NULL) {
		voidP filenamePtr = state->getRegistry()->getEntry("vrp.infile"); // tu se cita
		string filename = *((string*)filenamePtr.get());
		problem* inputProblem = new problem(filename.c_str());
		state->getContext()->environment = inputProblem;
	}

	inputProblem = (problem*) state->getContext()->environment;
	int vehicleNumber = inputProblem->getNumVehicles();
	int customerNumber = inputProblem->getNumCusto();
	std::vector<int> vehicles(vehicleNumber);

	for (int i = customerNumber + 1; i <= customerNumber + vehicleNumber; i++) {
		genome[std::to_string(i)] = {};
	}

	for (int i = 1; i <= customerNumber; i++) {
		auto it = genome.begin();
		std::advance(it, rand() % genome.size());
		genome.at(it->first).push_back(i);
	}

	if (inputProblem->isPDVRP == 1) {
		checkPPD();
	}
	return true;
}

individualMap *individualMap::copy() {
	individualMap* map = new individualMap();
	map->setGenome(genome);
	map->setProblem(inputProblem);
	return map;
}

void individualMap::write(XMLNode &xMyGenotype) {
	xMyGenotype = XMLNode::createXMLTopNode("individualMap");
	stringstream sValue;
	sValue << this->genome.size();
	xMyGenotype.addAttribute("size", sValue.str().c_str());

	sValue.str("");

	// write genome to sValue
	sValue << "Genome: ";

	std::map<std::string, std::vector<int>>::iterator it;
	for (it = genome.begin(); it != genome.end(); it++) {
		sValue << it->first << ':';
		for (int i = 0; i < it->second.size(); i++) {
			sValue << it->second[i] << ' ';
		}
		sValue << "\n";
	}

	xMyGenotype.addText(sValue.str().c_str());
}

void individualMap::read(XMLNode &xMyGenotype) {

}

std::vector<MutationOpP> individualMap::getMutationOp() {
	std::vector<MutationOpP> mut;
	mut.push_back(static_cast<MutationOpP> (new individualMapMutationOp));
	return mut;
}
std::vector<CrossoverOpP> individualMap::getCrossoverOp() {
	std::vector<CrossoverOpP> crx;
	crx.push_back(static_cast<CrossoverOpP> (new individualMapCrossoverOp));
	return crx;
}

problem* individualMap::getProblem() {
	return inputProblem;
}

void individualMap::setProblem(problem* problem) {
	inputProblem = problem;
}

map<string, vector<int>> individualMap::getGenome() {
	return genome;
}

void individualMap::setGenome(map<string, vector<int>> gen) {
	genome = gen;
}

individualP individualMap::inBetweenstep() {
	int numberOfCustomers = inputProblem->getNumCusto();
	int numberOfVehicles = inputProblem->getNumVehicles();

	std::vector<int> genomeToReturn;
	std::map<std::string, std::vector<int>>::iterator iterator;

	for (iterator = genome.begin(); iterator != genome.end(); iterator++) {
		genomeToReturn.push_back(stoi(iterator->first));
		for (int i = 0; i < genome.at(iterator->first).size(); i++) {
			genomeToReturn.push_back(genome.at(iterator->first).at(i));
		}
	}
	
	return individualP(new individual (genomeToReturn, inputProblem));
}

void individualMap::registerParameters(StateP state) {
	registerParameter(state, "size", (voidP)(new uint(1)), ECF::UINT, "size");
}

void individualMap::checkPPD() {

	customer *currentCustomer;
	int current;
	int pair;
	int indexCurrent = 0;
	int indexPair = 0;
	map<string, vector<int>> genomeCopy = genome;
	vector<int> doNotCheck;
	vector<int> currentCustomers;
	string vehicleNameCurrent;
	string vehicleNameOfPair;
	vector<int> customersUnderVehiclePair;
	bool makeSwitch = false;
	int index = 0;


	std::map<std::string, std::vector<int>>::iterator iterator;
	//iteriranje po mapi ({"V1", <1,5,7,3,21>}, {"V2", <4,9,33,12>}, ...)
	for (iterator = genomeCopy.begin(); iterator != genomeCopy.end(); iterator++) {
		//kljuc, tj. vozilo npr. "V1"
		vehicleNameCurrent = iterator->first;
		//trenutni customeri pod tim kljucem
		currentCustomers = genome.at(vehicleNameCurrent);
		for (indexCurrent = 0; indexCurrent < currentCustomers.size(); indexCurrent++) {
			//dohvaca preko indeksa sve customere redom
			current = currentCustomers[indexCurrent];
			//ako vec nije provjeren nadji konkretnog customera
			if (find(doNotCheck.begin(), doNotCheck.end(), current) == doNotCheck.end()) {
				currentCustomer = &(inputProblem->allCustomer[current - 1]);
				if (currentCustomer->pickup != 0) {
					pair = currentCustomer->pickup;
				}
				else {
					pair = currentCustomer->delivery;
				}
				//sad znamo customera i koji mu je par!
				//pokusava naci svoj par pod istim kljucem
				auto it1 = find(genome.at(vehicleNameCurrent).begin(), genome.at(vehicleNameCurrent).end(), current);
				index = distance(genome.at(vehicleNameCurrent).begin(), it1);

				auto it2 = find(currentCustomers.begin(), currentCustomers.end(), pair);
				//ako je dosao do kraja, tj. nema svog para pod kljucem
				//trazi pod kojim kljucem mu se nalazi par i nalazi kljuc
				if (it2 == currentCustomers.end()) {
					std::map<std::string, std::vector<int>>::iterator iterator2;
					for (iterator2 = genome.begin(); iterator2 != genome.end(); iterator2++) {
						customersUnderVehiclePair = genome.at(iterator2->first);
						for (int j = 0; j < customersUnderVehiclePair.size(); j++) {
							if (customersUnderVehiclePair[j] == pair) {
								vehicleNameOfPair = iterator2->first;
								indexPair = j;
								break;
							}
						}
					}


					//ako je to onaj koji ceka - ide pod vozilo od para a nestaje iz originalnog
					if (currentCustomer->pickup != 0) {
						genome.at(vehicleNameCurrent).erase(genome.at(vehicleNameCurrent).begin() + index);
						genome.at(vehicleNameOfPair).push_back(current);
					}
					else { //ako je onaj koji daje par dolazi pod vozilo davatelja i nestaje iz svog
						genome.at(vehicleNameOfPair).erase(genome.at(vehicleNameOfPair).begin() + indexPair);
						genome.at(vehicleNameCurrent).push_back(pair);
					}
					//tu je sve rijeseno ovim jer ih se gura na kraj liste customera
					//ako current ceka ide na kraj liste a ovaj koji daje je sigurno prije
					//isto tako ako current daje primatelj ide na kraj liste i current je sigurno prije
				}
				else { // ako su pak pod istim vozilom treba provjeriti koji je prije koji poslije
					indexPair = distance(currentCustomers.begin(), it2);
					if (currentCustomer->pickup != 0) {
						if (indexCurrent > indexPair) {
							makeSwitch = true;
						}
					}
					else {
						if (indexCurrent < indexPair) {
							makeSwitch = true;
						}
					}
				}
				doNotCheck.push_back(pair);
				doNotCheck.push_back(current);

				if (makeSwitch) {
					int tmp = currentCustomers.at(indexCurrent);
					currentCustomers.at(indexCurrent) = currentCustomers.at(indexPair);
					currentCustomers.at(indexPair) = tmp;
					makeSwitch = false;
				}
			}
		}
	}
}
