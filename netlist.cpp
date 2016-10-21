/*
 * netlist.cpp
 *
 *  Created on: Jun 10, 2013
 *      Author: Filipe Barretto
 *              Caio Taulois
 */

#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <windows.h>
#include <sstream>
#include <time.h>
#include <math.h>

#include "spice.h"

#define		ERROR_OPENFILE		1
#define		ERROR_NOT_CONV		2
#define		ERROR_RESISTOR		3

#define		NUM_ELEM_RES		4
#define		NUM_ELEM_CAP		4
#define		NUM_ELEM_IND		4
#define		NUM_ELEM_DEPSRC		6
#define		NUM_ELEM_OPAMP		5
#define		NUM_MIN_SRC			4
#define		NUM_MAX_DC			5
#define		NUM_MAX_SIN			11
#define		NUM_MAX_PULSE		12
#define		NUM_MAX_ELEM		12
#define		NUM_MAX_INFO		6

using namespace std;

int main () {

	string fileName;
	string numNodesStr;
	string line;
	string parameters[NUM_MAX_ELEM];

	unsigned size;
	unsigned numNodes;

	double dt = 0.01;
	double maxT = 1;
	unsigned nMax = 20;

	bool secondTime;

	vector<Element*> elements;
	vector<Source*> sources;
	vector<Inductor*> inductors;
	vector<string> jx;
	vector<string> info;

	cout << "Digite o nome do arquivo de netlist: ";
	getline(cin, fileName);

	clock_t start = clock();

	ifstream netlist;
	netlist.open((fileName + ".net").c_str());


	if (!(netlist)) {
		cout << "\nErro ao abrir o arquivo.\n";
		cout << "\nPressione ENTER para finalizar o programa.\n";
		cin.get();
		exit(ERROR_OPENFILE);
	}

	getline(netlist, numNodesStr);
	numNodes = 1 + atoi(numNodesStr.c_str());
	sizeAux = numNodes;

	/*
	 * Resistores
	 * Capacitores
	 * Indutores
	 * Tranformadores
	 * Fontes contínuas, pulsadas e senoidais
	 * Fontes Controladas
	 * AmpOps Ideais
	 *
	 */

	//if (netlist.is_open()) {
	//	while(netlist.good()) {
	//		getline(netlist, line);
	//		switch (line[0]) {
	//			case '.':
	//			{
	//				for (unsigned count = 0; count < NUM_MAX_INFO; count++) {
	//					unsigned position = line.find(" ");
	//					info.push_back(line.substr(0, position));
	//					if (line == line.substr(position + 1)) {
	//						break;
	//					}
	//					line = line.substr(position + 1);
	//				}

	//				maxT = atof(info[1].c_str());
	//				dt = atof(info[2].c_str());
	//				nMax = (info.size() == 4) ? atof(info[3].c_str()) : 0;
	//			}
	//		}
	//	}
	//}

	//cout << "nMax: " << nMax;

	//netlist.seekg (0, 0);

	cout << "\nLendo netlist\n";
	// Reading netlist file and inserting elements to a list
	if (netlist.is_open()) {
		while(netlist.good()) {
			getline(netlist, line);

			for (unsigned count = 0; count < NUM_MAX_ELEM; count++) {
				parameters[count] = "0";
			}


			switch (line[0]) {
			case '.':
			{
				for (unsigned count = 0; count < NUM_MAX_INFO; count++) {
					unsigned position = line.find(" ");
					info.push_back(line.substr(0, position));
					if (line == line.substr(position + 1)) {
						break;
					}
					line = line.substr(position + 1);
				}

				maxT = atof(info[1].c_str());
				dt = atof(info[2].c_str());
				nMax = (info.size() == 4) ? atof(info[3].c_str()) : 20;
				break;
			}

			// Resistors - <Name> <Node A> <Node B> <Resistance>
			case 'R':
			{
				for (unsigned count = 0; count < NUM_ELEM_RES; count++) {
					unsigned position = line.find(" ");
					parameters[count] = line.substr(0, position);
					line = line.substr(position + 1);
				}

				if (atof(parameters[3].c_str())) {
					Resistor* resistor = new Resistor(parameters[0], 1 + atoi(parameters[1].c_str()), 1 + atoi(parameters[2].c_str()), atof(parameters[3].c_str()));
					elements.push_back(resistor);
				}

				else {
					cout << "\nResistor nao pode ser nulo\n\nPressione ENTER para finalizar o programa.";
					cin.get();
					exit(ERROR_RESISTOR);
				}

				break;
			}

			// Capacitors - <Name> <Node A> <Node B> <Capacitance> (Initial Voltage)
			case 'C':
			{
				for (unsigned count = 0; count < NUM_ELEM_CAP; count++) {
					unsigned position = line.find(" ");
					parameters[count] = line.substr(0, position);
					line = line.substr(position + 1);
				}

				Capacitor* capacitor = new Capacitor(parameters[0], 1 + atoi(parameters[1].c_str()), 1 + atoi(parameters[2].c_str()), atof(parameters[3].c_str()));
				elements.push_back(capacitor);

				break;
			}
			// Inductance - <Name> <Node A> <Node B> <Inductance> (Initial Current)
			case 'L':
			{
				for (unsigned count = 0; count < NUM_ELEM_IND; count++) {
					unsigned position = line.find(" ");
					parameters[count] = line.substr(0, position);
					if (line == line.substr(position + 1)) {
						break;
					}

					line = line.substr(position + 1);
				}

					Inductor* inductor = new Inductor(parameters[0], 1 + atoi(parameters[1].c_str()), 1 + atoi(parameters[2].c_str()), atof(parameters[3].c_str()));
					elements.push_back(inductor);
					inductors.push_back(inductor);
					jx.push_back(parameters[0]);


				break;
			}

			case 'K':
			{
				for (unsigned count = 0; count < NUM_ELEM_IND; count++) {
					unsigned position = line.find(" ");
					parameters[count] = line.substr(0, position);
					if (line == line.substr(position + 1)) {
						break;
					}

					line = line.substr(position + 1);
				}

				unsigned aux1;
				unsigned aux2;


				for (unsigned count = 0; count < inductors.size(); count++) {
					if (inductors[count]->getName() == parameters[1].c_str()) {
						aux1 = count;
					}
				}

				for (unsigned count = 0; count < inductors.size(); count++) {
					if (inductors[count]->getName() == parameters[2].c_str()) {
						aux2 = count;
					}
				}

				//cout << "\n La: " << inductors[aux1]->getInductance();
				//cout << "\n Na: " << inductors[aux1]->getNodeA();
				//cout << "\n Nb: " << inductors[aux1]->getNodeB();

				//cout << "\n Lb: " << inductors[aux2]->getInductance();
				//cout << "\n Na: " << inductors[aux2]->getNodeA();
				//cout << "\n Nb: " << inductors[aux2]->getNodeB();

				Transformer* transformer = new Transformer(inductors[aux1]->getInductance(), inductors[aux1]->getX(), inductors[aux1]->getNodeA(), inductors[aux1]->getNodeB(), inductors[aux2]->getInductance(), inductors[aux2]->getX(), inductors[aux2]->getNodeA(), inductors[aux2]->getNodeB(), atof(parameters[3].c_str()));
				elements.push_back(transformer);

			break;
			}
			// Voltage Controlled Voltage Source - <Name> <Node A> <Node B> <Node C> <Node D> <Av>
			case 'E':
			{
				for (unsigned count = 0; count < NUM_ELEM_DEPSRC; count++) {
					unsigned position = line.find(" ");
					parameters[count] = line.substr(0, position);
					line = line.substr(position + 1);
				}


				DependentSourceVV* dependentSourceVV = new DependentSourceVV(parameters[0], 1 + atoi(parameters[1].c_str()), 1 + atoi(parameters[2].c_str()), 1 + atoi(parameters[3].c_str()), 1 + atoi(parameters[4].c_str()), atof(parameters[5].c_str()));
				elements.push_back(dependentSourceVV);
				jx.push_back(parameters[0]);

				break;
			}

			// Current Controlled Current Source - <Name> <Node A> <Node B> <Node C> <Node D> <Ai>
			case 'F':
			{
				for (unsigned count = 0; count < NUM_ELEM_DEPSRC; count++) {
					unsigned position = line.find(" ");
					parameters[count] = line.substr(0, position);
					line = line.substr(position + 1);
				}

				DependentSourceII* dependentSourceII = new DependentSourceII(parameters[0], 1 + atoi(parameters[1].c_str()), 1 + atoi(parameters[2].c_str()), 1 + atoi(parameters[3].c_str()), 1 + atoi(parameters[4].c_str()), atof(parameters[5].c_str()));
				elements.push_back(dependentSourceII);
				jx.push_back(parameters[0]);

				break;
			}

			// Voltage Controlled Current Source - <Name> <Node A> <Node B> <Node C> <Node D> <Gm>
			case 'G':
			{
				for (unsigned count = 0; count < NUM_ELEM_DEPSRC; count++) {
					unsigned position = line.find(" ");
					parameters[count] = line.substr(0, position);
					line = line.substr(position + 1);
				}

				DependentSourceVI* dependentSourceVI = new DependentSourceVI(parameters[0], 1 + atoi(parameters[1].c_str()), 1 + atoi(parameters[2].c_str()), 1 + atoi(parameters[3].c_str()), 1 + atoi(parameters[4].c_str()), atof(parameters[5].c_str()));
				elements.push_back(dependentSourceVI);

				break;
			}

			// Current Controlled Voltage Source - <Name> <Node A> <Node B> <Node C> <Node D> <Rm>
			case 'H':
			{
				for (unsigned count = 0; count < NUM_ELEM_DEPSRC; count++) {
					unsigned position = line.find(" ");
					parameters[count] = line.substr(0, position);
					line = line.substr(position + 1);
				}

				DependentSourceIV* dependentSourceIV = new DependentSourceIV(parameters[0], 1 + atoi(parameters[1].c_str()), 1 + atoi(parameters[2].c_str()), 1 + atoi(parameters[3].c_str()), 1 + atoi(parameters[4].c_str()), atof(parameters[5].c_str()));
				elements.push_back(dependentSourceIV);
				jx.push_back(parameters[0] + "(1)");
				jx.push_back(parameters[0] + "(2)");

				break;
			}

			// Operational Amplifier - <Name> <Node A> <Node B> <Node C> <Node D>
			case 'O':
			{
				for (unsigned count = 0; count < NUM_ELEM_OPAMP; count++) {
					unsigned position = line.find(" ");
					parameters[count] = line.substr(0, position);
					line = line.substr(position + 1);
				}

				OpAmp* opAmp = new OpAmp(parameters[0], 1 + atoi(parameters[1].c_str()), 1 + atoi(parameters[2].c_str()), 1 + atoi(parameters[3].c_str()), 1 + atoi(parameters[4].c_str()));
				elements.push_back(opAmp);
				jx.push_back(parameters[0]);

				break;
			}
			// Voltage Sources
			case 'V':
			{
				for (unsigned count = 0; count < NUM_MIN_SRC; count++) {
					unsigned position = line.find(" ");
					parameters[count] = line.substr(0, position);
					line = line.substr(position + 1);
				}

				switch (parameters[3][0]) {

				// DC Voltage Source
				case 'D':
				{
					for (unsigned count = NUM_MIN_SRC; count < NUM_MAX_DC; count++) {
						unsigned position = line.find(" ");
						parameters[count] = line.substr(0, position);
						if (line == line.substr(position + 1)) {
							break;
						}
						line = line.substr(position + 1);
					}

					DCvoltageSource* dcVoltageSource = new DCvoltageSource(parameters[0], 1 + atoi(parameters[1].c_str()), 1 + atoi(parameters[2].c_str()), atof(parameters[4].c_str()));
					//elements.push_back(dcVoltageSource);
					sources.push_back(dcVoltageSource);
					jx.push_back(parameters[0]);

					break;
				}

				// SIN Voltage Source
				case 'S':
				{
					for (unsigned count = NUM_MIN_SRC; count < NUM_MAX_SIN; count++) {
						unsigned position = line.find(" ");
						parameters[count] = line.substr(0, position);
						if (line == line.substr(position + 1)) {
							break;
						}
						line = line.substr(position + 1);
					}

					SINvoltageSource* sinVoltageSource = new SINvoltageSource(parameters[0], 1 + atoi(parameters[1].c_str()), 1 + atoi(parameters[2].c_str()), atof(parameters[4].c_str()),atof(parameters[5].c_str()), atof(parameters[6].c_str()), atof(parameters[7].c_str()), atof(parameters[8].c_str()), atof(parameters[9].c_str()), atof(parameters[10].c_str()));
					//elements.push_back(sinVoltageSource);
					sources.push_back(sinVoltageSource);
					jx.push_back(parameters[0]);

					break;
				}

				// PULSE Voltage Source
				case 'P':
				{
					for (unsigned count = NUM_MIN_SRC; count < NUM_MAX_PULSE; count++) {
						unsigned position = line.find(" ");
						parameters[count] = line.substr(0, position);
						if (line == line.substr(position + 1)) {
							break;
						}
						line = line.substr(position + 1);
					}
					
					PULSEvoltageSource* pulseVoltageSource = new PULSEvoltageSource(parameters[0], 1 + atoi(parameters[1].c_str()), 1 + atoi(parameters[2].c_str()), atof(parameters[4].c_str()),atof(parameters[5].c_str()), atof(parameters[6].c_str()), atof(parameters[7].c_str()), atof(parameters[8].c_str()), atof(parameters[9].c_str()), atof(parameters[10].c_str()), atof(parameters[11].c_str()), nMax);
					//elements.push_back(pulseVoltageSource);
					sources.push_back(pulseVoltageSource);
					jx.push_back(parameters[0]);
					
					break;
				}

				}

				break;
			}

			case 'I':
			{
				for (unsigned count = 0; count < NUM_MIN_SRC; count++) {
					unsigned position = line.find(" ");
					parameters[count] = line.substr(0, position);
					if (line == line.substr(position + 1)) {
						break;
					}
					line = line.substr(position + 1);
				}

				switch (parameters[3][0]) {

				// DC Current Source
				case 'D':
				{
					for (unsigned count = NUM_MIN_SRC; count < NUM_MAX_DC; count++) {
						unsigned position = line.find(" ");
						parameters[count] = line.substr(0, position);
						if (line == line.substr(position + 1)) {
							break;
						}
						line = line.substr(position + 1);
					}

					DCcurrentSource* dcCurrentSource = new DCcurrentSource(parameters[0], 1 + atoi(parameters[1].c_str()), 1 + atoi(parameters[2].c_str()), atof(parameters[4].c_str()));
					sources.push_back(dcCurrentSource);

					break;
				}

				// SIN Current Source
				case 'S':
				{
					for (unsigned count = NUM_MIN_SRC; count < NUM_MAX_SIN; count++) {
						unsigned position = line.find(" ");
						parameters[count] = line.substr(0, position);
						if (line == line.substr(position + 1)) {
							break;
						}
						line = line.substr(position + 1);
					}

					SINcurrentSource* sinCurrentSource = new SINcurrentSource(parameters[0], 1 + atoi(parameters[1].c_str()), 1 + atoi(parameters[2].c_str()), atof(parameters[4].c_str()),atof(parameters[5].c_str()), atof(parameters[6].c_str()), atof(parameters[7].c_str()), atof(parameters[8].c_str()), atof(parameters[9].c_str()), atof(parameters[10].c_str()));
					sources.push_back(sinCurrentSource);

					break;
				}

				// PULSE Current Source
				case 'P':
				{
					for (unsigned count = NUM_MIN_SRC; count < NUM_MAX_PULSE; count++) {
						unsigned position = line.find(" ");
						parameters[count] = line.substr(0, position);
						if (line == line.substr(position + 1)) {
							break;
						}
						line = line.substr(position + 1);
					}

					PULSEcurrentSource* pulseCurrentSource = new PULSEcurrentSource(parameters[0], 1 + atoi(parameters[1].c_str()), 1 + atoi(parameters[2].c_str()), atof(parameters[4].c_str()),atof(parameters[5].c_str()), atof(parameters[6].c_str()), atof(parameters[7].c_str()), atof(parameters[8].c_str()), atof(parameters[9].c_str()), atof(parameters[10].c_str()), atof(parameters[11].c_str()), nMax);
					sources.push_back(pulseCurrentSource);

					break;
				}

				}

				break;
			}
			}
		}
		netlist.close();
	}

	cout << "\nLeitura da netlist finalizada\n";
	cout << "\nSize Aux = " << sizeAux;


	cout << "\n\nElements:\n";
	showInfo(elements);
	cout << "\n\nSources:\n";
	showInfo(sources);
	cout << "\n\n";

	cin.get();

	// Finished Netlist Info -> Build Matrix
	size =  numNodes 					+
			numOpAmp					+
			numInductors				+
			numDependentSourceVV 		+
			numDependentSourceII 		+
			2 * numDependentSourceIV	+
			numVoltageSource			;


	// a * x = b
	vector< vector<complex<double> > > a(size, vector<complex<double> >(size));


	vector<complex<double> > b(size);
	for (unsigned count = 0; count < size; count++) {
		b[count] = 0;
	}

	vector<complex<double> > aux(size);
	for (unsigned count = 0; count < size; count++) {
		aux[count] = 0;
	}

	vector<complex<double> > x(size);
	for (unsigned count = 0; count < size; count++) {
		x[count] = 0;
	}

//	showAbx(a, b, x);

	ofstream graphs;
	graphs.open((fileName + ".txt").c_str());

	graphs << "t";

	for (unsigned count = 1; count < numNodes; count++) {
		graphs << "\tV" << count;
	}

	for (unsigned count = 0; count < (x.size() - numNodes); count++) {
		graphs << "\tJ" << jx[count];
	}
	graphs << "\n";

	//if (nMax == 0){
	//	for (unsigned i = 0; i < sources.size(); i ++) {
	//		if ( (sources[i]->getType() == "PULSE Current Source") || (sources[i]->getType() == "PULSE Voltage Source") ) {
	//			if (nMax != 0){
	//				if ( (1/(2*dt))/sources[i]->getFrequency() < nMax)
	//					nMax = (1/(2*dt))/sources[i]->getFrequency();
	//			}
	//			else
	//				nMax = (1/(2*dt))/sources[i]->getFrequency();
	//		}
	//	}
	//}

	for (unsigned i = 0; i <sources.size(); i++) {
		if (sources[i]->getType() == "PULSE Current Source") {
			PULSEcurrentSource* pulseCurrentSource = (PULSEcurrentSource*) sources[i];
			pulseCurrentSource->setFourierCoefficients(nMax);
			for (unsigned n = 0; n <= nMax; n++) {
					clearMatrix(a);
					clearVector(b);
					clearVector(aux);

					for (unsigned j = 0; j < sources.size(); j++) {
						if (sources[i] != sources[j]) {
							if (sources[j]->getType() == "DC Voltage Source") {
								DCvoltageSource* dcVoltageSource = (DCvoltageSource*) sources[j];
								dcVoltageSource->stamp(a, b, false);
							}
							if (sources[j]->getType() == "SIN Voltage Source") {
								SINvoltageSource* sinVoltageSource = (SINvoltageSource*) sources[j];
								sinVoltageSource->stamp(a, b, false,false);
							}
							if (sources[j]->getType() == "PULSE Voltage Source") {
								PULSEvoltageSource* pulseVoltageSource = (PULSEvoltageSource*) sources[j];
								pulseVoltageSource->stamp(a, b, false, false, 0);
							}
							if (sources[i]->getType() == "DC Current Source") {
								DCcurrentSource* dcCurrentSource = (DCcurrentSource*) sources[i];
								dcCurrentSource->stamp(a, b, false);
							}
							if (sources[i]->getType() == "SIN Current Source") {
								SINcurrentSource* sinCurrentSource = (SINcurrentSource*) sources[i];
								sinCurrentSource->stamp(a, b, false, false);
							}
							if (sources[i]->getType() == "PULSE Current Source") {
								PULSEcurrentSource* pulseCurrentSource = (PULSEcurrentSource*) sources[i];
								pulseCurrentSource->stamp(a, b, false, false, 0);
							}
						}
					}

					if (n == 0){
						pulseCurrentSource->stamp(a, b, true, false, n);
						timeAnalysis(elements, a, b, aux, 0, n);
					}
					else {
						pulseCurrentSource->stamp(a, b, true, true, n);
						timeAnalysis(elements, a, b, aux, sources[i]->getFrequency(), n);
					}
					pulseCurrentSource->setAuxPulse(aux);

					//cout << "\nn = " << n << "\n\n";
					//showAbx(a, b, aux);
					//cin.get();
			}
		}
		if (sources[i]->getType() == "PULSE Voltage Source") {			
			PULSEvoltageSource* pulseVoltageSource = (PULSEvoltageSource*) sources[i];
			pulseVoltageSource->setFourierCoefficients(nMax);
			for (unsigned n = 0; n <= nMax; n++) {
				clearMatrix(a);
				clearVector(b);
				clearVector(aux);

				for (unsigned j = 0; j < sources.size(); j++) {
					if (sources[i] != sources[j]) {
						if (sources[j]->getType() == "DC Voltage Source") {
							DCvoltageSource* dcVoltageSource = (DCvoltageSource*) sources[j];
							dcVoltageSource->stamp(a, b, false);
						}
						if (sources[j]->getType() == "SIN Voltage Source") {
							SINvoltageSource* sinVoltageSource = (SINvoltageSource*) sources[j];
							sinVoltageSource->stamp(a, b, false,false);
						}
						if (sources[j]->getType() == "PULSE Voltage Source") {
							PULSEvoltageSource* pulseVoltageSource = (PULSEvoltageSource*) sources[j];
							pulseVoltageSource->stamp(a, b, false, false, 0);
						}
						if (sources[i]->getType() == "DC Current Source") {
							DCcurrentSource* dcCurrentSource = (DCcurrentSource*) sources[i];
							dcCurrentSource->stamp(a, b, false);
						}
						if (sources[i]->getType() == "SIN Current Source") {
							SINcurrentSource* sinCurrentSource = (SINcurrentSource*) sources[i];
							sinCurrentSource->stamp(a, b, false, false);
						}
						if (sources[i]->getType() == "PULSE Current Source") {
							PULSEcurrentSource* pulseCurrentSource = (PULSEcurrentSource*) sources[i];
							pulseCurrentSource->stamp(a, b, false, false, 0);
						}
					}
				}

				if (n == 0){
					pulseVoltageSource->stamp(a, b, true, false, n);
					timeAnalysis(elements, a, b, aux, 0, n);
				}
				else {
					pulseVoltageSource->stamp(a, b, true, true, n);
					timeAnalysis(elements, a, b, aux, sources[i]->getFrequency(), n);
				}
				pulseVoltageSource->setAuxPulse(aux);

				//cout << "\nn = " << n << "\n\n";
				//showAbx(a, b, aux);
				//cin.get();
			}
		}
		if (sources[i]->getType() == "DC Voltage Source") {
			clearMatrix(a);
			clearVector(b);
			clearVector(aux);

			DCvoltageSource* dcVoltageSource = (DCvoltageSource*) sources[i];
			dcVoltageSource->stamp(a, b, true);
			for (unsigned j = 0; j < sources.size(); j++) {
				if (sources[i] != sources[j]) {
					if (sources[j]->getType() == "DC Voltage Source") {
						DCvoltageSource* dcVoltageSource = (DCvoltageSource*) sources[j];
						dcVoltageSource->stamp(a, b, false);
					}
					if (sources[j]->getType() == "SIN Voltage Source") {
						SINvoltageSource* sinVoltageSource = (SINvoltageSource*) sources[j];
						sinVoltageSource->stamp(a, b, false,false);
					}
					if (sources[j]->getType() == "PULSE Voltage Source") {
						PULSEvoltageSource* pulseVoltageSource = (PULSEvoltageSource*) sources[j];
						pulseVoltageSource->stamp(a, b, false, false, 0);
					}
					if (sources[i]->getType() == "DC Current Source") {
						DCcurrentSource* dcCurrentSource = (DCcurrentSource*) sources[i];
						dcCurrentSource->stamp(a, b, false);
					}
					if (sources[i]->getType() == "SIN Current Source") {
						SINcurrentSource* sinCurrentSource = (SINcurrentSource*) sources[i];
						sinCurrentSource->stamp(a, b, false, false);
					}
					if (sources[i]->getType() == "PULSE Current Source") {
						PULSEcurrentSource* pulseCurrentSource = (PULSEcurrentSource*) sources[i];
						pulseCurrentSource->stamp(a, b, false, false, 0);
					}
				}
			}
			timeAnalysis(elements, a, b, aux, sources[i]->getFrequency(), 1);
			dcVoltageSource->setAuxDC(aux);
		}
		if (sources[i]->getType() == "DC Current Source") {
			clearMatrix(a);
			clearVector(b);
			clearVector(aux);

			DCcurrentSource* dcCurrentSource = (DCcurrentSource*) sources[i];
			dcCurrentSource->stamp(a, b, true);
			for (unsigned j = 0; j < sources.size(); j++) {
				if (sources[i] != sources[j]) {
					if (sources[j]->getType() == "DC Voltage Source") {
						DCvoltageSource* dcVoltageSource = (DCvoltageSource*) sources[j];
						dcVoltageSource->stamp(a, b, false);
					}
					if (sources[j]->getType() == "SIN Voltage Source") {
						SINvoltageSource* sinVoltageSource = (SINvoltageSource*) sources[j];
						sinVoltageSource->stamp(a, b, false,false);
					}
					if (sources[j]->getType() == "PULSE Voltage Source") {
						PULSEvoltageSource* pulseVoltageSource = (PULSEvoltageSource*) sources[j];
						pulseVoltageSource->stamp(a, b, false, false, 0);
					}
					if (sources[i]->getType() == "DC Current Source") {
						DCcurrentSource* dcCurrentSource = (DCcurrentSource*) sources[i];
						dcCurrentSource->stamp(a, b, false);
					}
					if (sources[i]->getType() == "SIN Current Source") {
						SINcurrentSource* sinCurrentSource = (SINcurrentSource*) sources[i];
						sinCurrentSource->stamp(a, b, false, false);
					}
					if (sources[i]->getType() == "PULSE Current Source") {
						PULSEcurrentSource* pulseCurrentSource = (PULSEcurrentSource*) sources[i];
						pulseCurrentSource->stamp(a, b, false, false, 0);
					}
				}
			}
			timeAnalysis(elements, a, b, aux, sources[i]->getFrequency(), 1);
			dcCurrentSource->setAuxDC(aux);
		}
		if (sources[i]->getType() == "SIN Voltage Source") {
			clearMatrix(a);
			clearVector(b);
			clearVector(aux);

			secondTime = false;
			SINvoltageSource* sinVoltageSource = (SINvoltageSource*) sources[i];
			sinVoltageSource->stamp(a, b, true, false);

			if (sinVoltageSource->getDcLevel() != 0) {
				secondTime = true;
			}

			sinVoltageSource->setBoolDC(secondTime);

			for (unsigned j = 0; j < sources.size(); j++) {
				if (sources[i] != sources[j]) {
					if (sources[j]->getType() == "DC Voltage Source") {
						DCvoltageSource* dcVoltageSource = (DCvoltageSource*) sources[j];
						dcVoltageSource->stamp(a, b, false);
					}
					if (sources[j]->getType() == "SIN Voltage Source") {
						SINvoltageSource* sinVoltageSource = (SINvoltageSource*) sources[j];
						sinVoltageSource->stamp(a, b, false,false);
					}
					if (sources[j]->getType() == "PULSE Voltage Source") {
						PULSEvoltageSource* pulseVoltageSource = (PULSEvoltageSource*) sources[j];
						pulseVoltageSource->stamp(a, b, false, false, 0);
					}
					if (sources[i]->getType() == "DC Current Source") {
						DCcurrentSource* dcCurrentSource = (DCcurrentSource*) sources[i];
						dcCurrentSource->stamp(a, b, false);
					}
					if (sources[i]->getType() == "SIN Current Source") {
						SINcurrentSource* sinCurrentSource = (SINcurrentSource*) sources[i];
						sinCurrentSource->stamp(a, b, false, false);
					}
					if (sources[i]->getType() == "PULSE Current Source") {
						PULSEcurrentSource* pulseCurrentSource = (PULSEcurrentSource*) sources[i];
						pulseCurrentSource->stamp(a, b, false, false, 0);
					}
				}
			}
			timeAnalysis(elements, a, b, aux, sources[i]->getFrequency(), 1);
			sinVoltageSource->setAuxSin(aux);

			if (secondTime) {
				secondTime = false;

				clearMatrix(a);
				clearVector(b);
				clearVector(aux);

				SINvoltageSource* sinVoltageSource = (SINvoltageSource*) sources[i];
				sinVoltageSource->stamp(a, b, true, true);

				for (unsigned j = 0; j < sources.size(); j++) {
					if (sources[i] != sources[j]) {
						if (sources[j]->getType() == "DC Voltage Source") {
							DCvoltageSource* dcVoltageSource = (DCvoltageSource*) sources[j];
							dcVoltageSource->stamp(a, b, false);
						}
						if (sources[j]->getType() == "SIN Voltage Source") {
							SINvoltageSource* sinVoltageSource = (SINvoltageSource*) sources[j];
							sinVoltageSource->stamp(a, b, false,false);
						}
						if (sources[j]->getType() == "PULSE Voltage Source") {
							PULSEvoltageSource* pulseVoltageSource = (PULSEvoltageSource*) sources[j];
							pulseVoltageSource->stamp(a, b, false, false, 0);
						}
						if (sources[i]->getType() == "DC Current Source") {
							DCcurrentSource* dcCurrentSource = (DCcurrentSource*) sources[i];
							dcCurrentSource->stamp(a, b, false);
						}
						if (sources[i]->getType() == "SIN Current Source") {
							SINcurrentSource* sinCurrentSource = (SINcurrentSource*) sources[i];
							sinCurrentSource->stamp(a, b, false, false);
						}
						if (sources[i]->getType() == "PULSE Current Source") {
							PULSEcurrentSource* pulseCurrentSource = (PULSEcurrentSource*) sources[i];
							pulseCurrentSource->stamp(a, b, false, false, 0);
						}
					}
				}
				timeAnalysis(elements, a, b, aux, sources[i]->getFrequency(), 1);
				sinVoltageSource->setAuxSinDC(aux);
			}
		}
		if (sources[i]->getType() == "SIN Current Source") {
			clearMatrix(a);
			clearVector(b);
			clearVector(aux);

			secondTime = false;
			SINcurrentSource* sinCurrentSource = (SINcurrentSource*) sources[i];
			sinCurrentSource->stamp(a, b, true, false);

			if (sinCurrentSource->getDcLevel() != 0) {
				secondTime = true;
			}

			sinCurrentSource->setBoolDC(secondTime);

			for (unsigned j = 0; j < sources.size(); j++) {
				if (sources[i] != sources[j]) {
					if (sources[j]->getType() == "DC Voltage Source") {
						DCvoltageSource* dcVoltageSource = (DCvoltageSource*) sources[j];
						dcVoltageSource->stamp(a, b, false);
					}
					if (sources[j]->getType() == "SIN Voltage Source") {
						SINvoltageSource* sinVoltageSource = (SINvoltageSource*) sources[j];
						sinVoltageSource->stamp(a, b, false,false);
					}
					if (sources[j]->getType() == "PULSE Voltage Source") {
						PULSEvoltageSource* pulseVoltageSource = (PULSEvoltageSource*) sources[j];
						pulseVoltageSource->stamp(a, b, false, false, 0);
					}
					if (sources[i]->getType() == "DC Current Source") {
						DCcurrentSource* dcCurrentSource = (DCcurrentSource*) sources[i];
						dcCurrentSource->stamp(a, b, false);
					}
					if (sources[i]->getType() == "SIN Current Source") {
						SINcurrentSource* sinCurrentSource = (SINcurrentSource*) sources[i];
						sinCurrentSource->stamp(a, b, false, false);
					}
					if (sources[i]->getType() == "PULSE Current Source") {
						PULSEcurrentSource* pulseCurrentSource = (PULSEcurrentSource*) sources[i];
						pulseCurrentSource->stamp(a, b, false, false, 0);
					}
				}
			}
			timeAnalysis(elements, a, b, aux, sources[i]->getFrequency(), 1);
			sinCurrentSource->setAuxSin(aux);

			if (secondTime) {
				secondTime = false;

				clearMatrix(a);
				clearVector(b);
				clearVector(aux);

				SINcurrentSource* sinCurrentSource = (SINcurrentSource*) sources[i];
				sinCurrentSource->stamp(a, b, true, true);

				for (unsigned j = 0; j < sources.size(); j++) {
					if (sources[i] != sources[j]) {
						if (sources[j]->getType() == "DC Voltage Source") {
							DCvoltageSource* dcVoltageSource = (DCvoltageSource*) sources[j];
							dcVoltageSource->stamp(a, b, false);
						}
						if (sources[j]->getType() == "SIN Voltage Source") {
							SINvoltageSource* sinVoltageSource = (SINvoltageSource*) sources[j];
							sinVoltageSource->stamp(a, b, false,false);
						}
						if (sources[j]->getType() == "PULSE Voltage Source") {
							PULSEvoltageSource* pulseVoltageSource = (PULSEvoltageSource*) sources[j];
							pulseVoltageSource->stamp(a, b, false, false, 0);
						}
						if (sources[i]->getType() == "DC Current Source") {
							DCcurrentSource* dcCurrentSource = (DCcurrentSource*) sources[i];
							dcCurrentSource->stamp(a, b, false);
						}
						if (sources[i]->getType() == "SIN Current Source") {
							SINcurrentSource* sinCurrentSource = (SINcurrentSource*) sources[i];
							sinCurrentSource->stamp(a, b, false, false);
						}
						if (sources[i]->getType() == "PULSE Current Source") {
							PULSEcurrentSource* pulseCurrentSource = (PULSEcurrentSource*) sources[i];
							pulseCurrentSource->stamp(a, b, false, false, 0);
						}
					}
				}
				timeAnalysis(elements, a, b, aux, sources[i]->getFrequency(), 1);
				sinCurrentSource->setAuxSinDC(aux);
			}
		}
	}

	cout << "\nInicio da analise do circuito\n";
	for (t = 0; t < maxT; t += dt) {

		clearVector(x);

		// Superposition
		for (unsigned i = 0; i < sources.size(); i ++) {

			if (sources[i]->getType() == "PULSE Current Source") {

				PULSEcurrentSource* pulseCurrentSource = (PULSEcurrentSource*) sources[i];
				for (unsigned n = 0; n <= nMax; n++) {
					if (n==0) 
						superposition(x, pulseCurrentSource->getAuxPulse(n), 0, t, false, false, n, (pulseCurrentSource->getPeriod() - pulseCurrentSource->getRiseTime() - pulseCurrentSource->getFallTime() - pulseCurrentSource->getOnTime()));
					else 						
						superposition(x, pulseCurrentSource->getAuxPulse(n), sources[i]->getFrequency(), t, true, true, n, (pulseCurrentSource->getPeriod() - pulseCurrentSource->getRiseTime() - pulseCurrentSource->getFallTime() - pulseCurrentSource->getOnTime()));									
				}
			}
			else if (sources[i]->getType() == "PULSE Voltage Source") {

				PULSEvoltageSource* pulseVoltageSource = (PULSEvoltageSource*) sources[i];
				for (unsigned n = 0; n <= nMax; n++) {
					if (n==0) 
						superposition(x, pulseVoltageSource->getAuxPulse(n), 0, t, false, false, n, (pulseVoltageSource->getPeriod() - pulseVoltageSource->getRiseTime() - pulseVoltageSource->getFallTime() - pulseVoltageSource->getOnTime()));
					else 						
						superposition(x, pulseVoltageSource->getAuxPulse(n), sources[i]->getFrequency(), t, true, true, n, (pulseVoltageSource->getPeriod() - pulseVoltageSource->getRiseTime() - pulseVoltageSource->getFallTime() - pulseVoltageSource->getOnTime()));					
				}
			}
			else if (sources[i]->getType() == "DC Voltage Source") {
				DCvoltageSource* dcVoltageSource = (DCvoltageSource*) sources[i];
				superposition(x, dcVoltageSource->getAuxDc(), sources[i]->getFrequency(), t, false, false, 0, 0);
			}
			else if (sources[i]->getType() == "DC Current Source") {
				DCcurrentSource* dcCurrentSource = (DCcurrentSource*) sources[i];
				superposition(x, dcCurrentSource->getAuxDc(), sources[i]->getFrequency(), t, false, false, 0, 0);
			}
			else if (sources[i]->getType() == "SIN Voltage Source") {
				SINvoltageSource* sinVoltageSource = (SINvoltageSource*) sources[i];
				superposition(x, sinVoltageSource->getAuxSin(), sources[i]->getFrequency(), t, false, false, 0, 0);
				if (sinVoltageSource->getBoolDC())
					superposition(x, sinVoltageSource->getAuxSinDC(), 0, t, false, false, 0, 0);
			}
			else if (sources[i]->getType() == "SIN Current Source") {
				SINcurrentSource* sinCurrentSource = (SINcurrentSource*) sources[i];
				superposition(x, sinCurrentSource->getAuxSin(), sources[i]->getFrequency(), t, false, false, 0, 0);
				if (sinCurrentSource->getBoolDC())
					superposition(x, sinCurrentSource->getAuxSinDC(), 0, t, false, false, 0, 0);
			}
		}

		graphs << t;
		for (unsigned count = 1; count < numNodes; count++) {
			graphs << "\t" << real(x[count]);
		}
		for (unsigned count = numNodes; count < x.size(); count++) {
			graphs << "\t" << real(x[count]);
		}
		graphs << "\n";

//		cout << "\n\n\n\n\nSolucao:";
//		showVector(x);
//		cin.get();;

	}

//	timeAnalysis(elements, a, b, x, 1000);


//	gaussianElimination(a, b, x);

	clock_t end = clock();



	cout << "\n\nA simulação demorou" << (end - start) / 1000 << " segundos.";
	cout << "\n\nPressione ENTER para finalizar o programa\n";

	cin.get();
}




