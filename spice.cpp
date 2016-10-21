/*
 * spice.cpp
 *
 *  Created on: Jun 10, 2013
 *      Author: Filipe Barretto
 *              Caio Taulois
 */

#include <math.h>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <complex>

#include "spice.h"
/*
*/	


// Global Variables 
// Time Variables
double t = 0;
unsigned numSeries = 5;
double dti;
bool uic;

// Constants
const double pi = 4.0 * atan(1.0);
const complex<double> j(0.0, 1.0);

// Element Counters
unsigned numResistors = 0;
unsigned numCapacitors = 0;
unsigned numInductors = 0;

unsigned numDependentSourceVV = 0;
unsigned numDependentSourceII = 0;
unsigned numDependentSourceVI = 0;
unsigned numDependentSourceIV = 0;

unsigned numOpAmp = 0;

unsigned numDiode = 0;
unsigned numTransistor = 0;

unsigned numVoltageSource = 0;
unsigned numCurrentSource = 0;

// Auxiliary Global Variables
unsigned sizeAux = 0;


// Parent Class Element
Element::Element() {}

void Element::setType(string elemType) {
	type = elemType;
}

string Element::getType() {
	return type;
}

void Element::setName(string elemName) {
	this->name = elemName;
}

string Element::getName() {
	return this->name;
}

void Element::setNodes(unsigned nodeA, unsigned nodeB) {
	nodes.push_back(nodeA);
	nodes.push_back(nodeB);

}

void Element::setNodes(unsigned nodeA, unsigned nodeB, unsigned nodeC, unsigned nodeD) {
	nodes.push_back(nodeA);
	nodes.push_back(nodeB);
	nodes.push_back(nodeC);
	nodes.push_back(nodeD);
}

unsigned Element::getNodeA() {
	return this->nodes[0];
}

unsigned Element::getNodeB() {
	return this->nodes[1];
}

unsigned Element::getNodeC() {
	return this->nodes[2];
}

unsigned Element::getNodeD() {
	return this->nodes[3];
}


Source::Source() {}

void Source::setFrequency(double frequency) {
	this->w = frequency * 2 * pi;
}

double Source::getFrequency() {
	return this->w;
}


// Resistors
Resistor::Resistor(string resName, unsigned resNodeA, unsigned resNodeB, double resResistance) {
	this->type = "Resistor";
	setName(resName);
	setNodes(resNodeA, resNodeB);
	setResistance(resResistance);
	numResistors++;
}

void Resistor::setResistance(double resResistance) {
	this->resistance = resResistance;
}

double Resistor::getResistance() {
	return this->resistance;
}

void Resistor::stamp (vector< vector<complex<double> > > &a) {
	a[nodes[0] - 1] [nodes[0] - 1] += 1 / resistance;
	a[nodes[1] - 1] [nodes[1] - 1] += 1 / resistance;
	a[nodes[0] - 1] [nodes[1] - 1] += - (1 / resistance);
	a[nodes[1] - 1] [nodes[0]- 1] += - (1 / resistance);
}


// Capacitors
Capacitor::Capacitor(string capName, unsigned capNodeA, unsigned capNodeB, double capCapacitance) {
	this->type = "Capacitor";
	setName(capName);
	setNodes(capNodeA, capNodeB);
	setCapacitance(capCapacitance);
	numCapacitors++;
}

void Capacitor::setCapacitance(double capCapacitance) {
	this->capacitance = capCapacitance;
}

double Capacitor::getCapacitance() {
	return this->capacitance;
}

void Capacitor::stamp (vector< vector<complex<double> > > &a, double w) {
	a[nodes[0] - 1] [nodes[0] - 1] += j * w * capacitance;
	a[nodes[0] - 1] [nodes[1] - 1] += - j * w * capacitance;
	a[nodes[1] - 1] [nodes[0] - 1] += - j * w * capacitance;
	a[nodes[1] - 1] [nodes[1] - 1] += j * w * capacitance;
}


// Inductors
Inductor::Inductor(string indName, unsigned indNodeA, unsigned indNodeB, double indInductance) {
	this->type = "Inductor";
	setName(indName);
	setNodes(indNodeA, indNodeB);
	setInductance(indInductance);
	numInductors++;
	sizeAux++;
	setX(sizeAux);
}

void Inductor::setInductance(double indInductance) {
	this->inductance = indInductance;
}

double Inductor::getInductance() {
	return this->inductance;
}

void Inductor::setX(unsigned x) {
	this->x = x;
}

unsigned Inductor::getX() {
	return this->x;
}

void Inductor::stamp (vector< vector<complex<double> > > &a, double w) {
	a[x - 1] [nodes[0] - 1] += - 1;
	a[x - 1] [nodes[1] - 1] += 1;
	a[nodes[0] - 1] [x - 1] += 1;
	a[nodes[1] - 1] [x - 1] += - 1;
	a[x - 1] [x - 1] += j * w * inductance;
}

// Transformer
Transformer::Transformer(double l1, unsigned x, unsigned nodeA, unsigned nodeB, double l2, unsigned y, unsigned nodeC, unsigned nodeD, double k) {
	this->type = "Transformer";
	setL1(l1);
	setL2(l2);
	setK(k);
	setNodes(nodeA, nodeB, nodeC, nodeD);
	this->x = x;
	this->y = y;
}

void Transformer::setK(double k) {
	this->k = k;
}

double Transformer::getK() {
	return this->k;
}

void Transformer::setL1(double l1) {
	this->l1 = l1;
}

double Transformer::getL1() {
	return this->l1;
}

void Transformer::setL2(double l2) {
	this->l2 = l2;
}

double Transformer::getL2() {
	return this->l2;
}

void Transformer::stamp (vector< vector<complex<double> > > &a, double w) {
	double m = k * sqrt(l1 * l2);

	a[x - 1] [y - 1] += j * w * m;
	a[y - 1] [x - 1] += j * w * m;

}

// Voltage Controlled Voltage Source
DependentSourceVV::DependentSourceVV(string srcVVName, unsigned srcVVNodeA, unsigned srcVVNodeB, unsigned srcVVNodeC, unsigned srcVVNodeD, double srcVVAv) {
	setType("Voltage Controlled Voltage Source");
	setName(srcVVName);
	setNodes(srcVVNodeA, srcVVNodeB, srcVVNodeC, srcVVNodeD);
	setAv(srcVVAv);
	numDependentSourceVV++;
	sizeAux++;
	x = sizeAux;
}

void DependentSourceVV::setAv(double srcVVAv) {
	this->av = srcVVAv;
}

double DependentSourceVV::getAv() {
	return this->av;
}

void DependentSourceVV::stamp (vector< vector<complex<double> > > &a) {
	a[nodes[0] - 1] [x - 1] += 1;
	a[nodes[1] - 1] [x - 1] += - 1;
	a[x - 1] [nodes[0] - 1] += - 1;
	a[x - 1] [nodes[1] - 1] += 1;
	a[x - 1] [nodes[2] - 1] += av;
	a[x - 1] [nodes[3] - 1] += - av;

}


// Current Controlled Current Source
DependentSourceII::DependentSourceII(string srcIIName, unsigned srcIINodeA, unsigned srcIINodeB, unsigned srcIINodeC, unsigned srcIINodeD, double srcIIAi) {
	setType("Current Controlled Current Source");
	setName(srcIIName);
	setNodes(srcIINodeA, srcIINodeB, srcIINodeC, srcIINodeD);
	setAi(srcIIAi);
	numDependentSourceII++;
	sizeAux++;
	x = sizeAux;
}

void DependentSourceII::setAi(double srcIIAi) {
	this->ai = srcIIAi;
}

double DependentSourceII::getAi() {
	return this->ai;
}

void DependentSourceII::stamp (vector< vector<complex<double> > > &a) {
	a[nodes[0] - 1] [x - 1] += ai;
	a[nodes[1] - 1] [x - 1] += - ai;
	a[nodes[2] - 1] [x - 1] += 1;
	a[nodes[3] - 1] [x - 1] += - 1;
	a[x - 1] [nodes[2] - 1] += - 1;
	a[x - 1] [nodes[3] - 1] += 1;
}


// Voltage Controlled Current Source
DependentSourceVI::DependentSourceVI(string srcIVName, unsigned srcIVNodeA, unsigned srcIVNodeB, unsigned srcIVNodeC, unsigned srcIVNodeD, double srcIVGm) {
	setType("Voltage Controlled Current Source");
	setName(srcIVName);
	setNodes(srcIVNodeA, srcIVNodeB, srcIVNodeC, srcIVNodeD);
	setGm(srcIVGm);
	numDependentSourceVI++;
}

void DependentSourceVI::setGm(double srcIVGm) {
	this->gm = srcIVGm;
}

double DependentSourceVI::getGm() {
	return this->gm;
}

void DependentSourceVI::stamp (vector< vector<complex<double> > > &a) {
	a[nodes[0] - 1] [nodes[2] - 1] += gm;
	a[nodes[1] - 1] [nodes[2] - 1] += - gm;
	a[nodes[0] - 1] [nodes[3] - 1] += - gm;
	a[nodes[1] - 1] [nodes[3] - 1] += gm;
}


// Current Controlled Voltage Source
DependentSourceIV::DependentSourceIV(string srcVIName, unsigned srcVINodeA, unsigned srcVINodeB, unsigned srcVINodeC, unsigned srcVINodeD, double srcVIRm) {
	setType("Current Controlled Voltage Source");
	setName(srcVIName);
	setNodes(srcVINodeA, srcVINodeB, srcVINodeC, srcVINodeD);
	setRm(srcVIRm);
	numDependentSourceIV++;
	sizeAux++;
	x = sizeAux;
	sizeAux++;
	y = sizeAux;
}

void DependentSourceIV::setRm(double srcVIRm) {
	this->rm = srcVIRm;
}

double DependentSourceIV::getRm() {
	return this->rm;

}

void DependentSourceIV::stamp (vector< vector<complex<double> > > &a) {
	a[nodes[0] - 1] [y - 1] += 1;
	a[nodes[1] - 1] [y - 1] += - 1;
	a[nodes[2] - 1] [x - 1] += 1;
	a[nodes[3] - 1] [x - 1] += - 1;
	a[x - 1] [nodes[2] - 1] += - 1;
	a[x - 1] [nodes[3] - 1] += 1;
	a[y - 1] [nodes[0] - 1] += - 1;
	a[y - 1] [nodes[1] - 1] += 1;
	a[y - 1][x - 1] += rm;
}

// Operational Amplifier
OpAmp::OpAmp(string opAmpName, unsigned opAmpNodeA, unsigned opAmpNodeB, unsigned opAmpNodeC, unsigned opAmpNodeD) {
	this->type = "OpAmp";
	setName(opAmpName);
	setNodes(opAmpNodeA, opAmpNodeB, opAmpNodeC, opAmpNodeD);
	numOpAmp++;
	sizeAux++;
	x = sizeAux;
}

void OpAmp::stamp (vector< vector<complex<double> > > &a) {
	a[nodes[0] - 1] [x - 1] += 1;
	a[nodes[1] - 1] [x - 1] += - 1;
	a[x - 1] [nodes[2] - 1] += 1;
	a[x - 1] [nodes[3] - 1] += - 1;
}


// DC voltage Source
DCvoltageSource::DCvoltageSource(string sourceName, unsigned sourceNodeA, unsigned sourceNodeB, double sourceValue) {
	setType("DC Voltage Source");
	setName(sourceName);
	setNodes(sourceNodeA, sourceNodeB);
	setValue(sourceValue);
	setFrequency(0);
	numVoltageSource++;
	sizeAux++;
	x = sizeAux;
}

void DCvoltageSource::setValue(double sourceValue) {
	value = sourceValue;
}

double DCvoltageSource::getValue() {
	return value;
}

void DCvoltageSource::stamp(vector< vector<complex<double> > > &a, vector<complex<double> > &b, bool main) {
	a[nodes[0] - 1][x - 1] += 1;
	a[nodes[1] - 1][x - 1] += - 1;
	a[x - 1][nodes[0] - 1] += - 1;
	a[x - 1][nodes[1] - 1] += 1;

	if (main) {
		b[x - 1] += -value;
	}
}

void DCvoltageSource::setAuxDC(vector <complex<double> > auxDC){
	this->auxDC = auxDC;
}

vector <complex<double> > DCvoltageSource::getAuxDc() {return this->auxDC;}

// SIN voltage Source
SINvoltageSource::SINvoltageSource(string sourceName, unsigned sourceNodeA, unsigned sourceNodeB, double sourceDcLevel, double sourceAmplitude, double sourceFrequency, double sourceDelay, double sourceAttenuation, double sourceAngle, double sourceNumCycles) {
	setType("SIN Voltage Source");
	setName(sourceName);
	setNodes(sourceNodeA, sourceNodeB);
	setDcLevel(sourceDcLevel);
	setAmplitude(sourceAmplitude);
	setFrequency(sourceFrequency);
	setDelay(sourceDelay);
	setAttenuation(sourceAttenuation);
	setAngle(sourceAngle);
	setNumCycles(sourceNumCycles);
	numVoltageSource++;
	sizeAux++;
	x = sizeAux;
}

void SINvoltageSource::setDcLevel(double sourceDcLevel) {
	dcLevel = sourceDcLevel;
}

double SINvoltageSource::getDcLevel() {
	return dcLevel;
}

void SINvoltageSource::setAmplitude(double sourceAmplitude) {
	amplitude = sourceAmplitude;
}

double SINvoltageSource::getAmplitude() {
	return amplitude;
}

void SINvoltageSource::setDelay(double sourceDelay) {
	delay = sourceDelay;
}

double SINvoltageSource::getDelay() {
	return delay;
}

void SINvoltageSource::setAttenuation(double sourceAttenuation) {
	attenuation = sourceAttenuation;
}

double SINvoltageSource::getAttenuation() {
	return attenuation;
}

void SINvoltageSource::setAngle(double sourceAngle) {
	angle = sourceAngle;
}

double SINvoltageSource::getAngle() {
	return angle;
}

void SINvoltageSource::setNumCycles(double sourceNumCycles) {
	numCycles = sourceNumCycles;
}

double SINvoltageSource::getNumCycles() {
	return numCycles;
}

void SINvoltageSource::stamp(vector< vector<complex<double> > > &a, vector<complex<double> > &b, bool main, bool secondTime) {
	a[nodes[0] - 1][x - 1] += 1;
	a[nodes[1] - 1][x - 1] += - 1;
	a[x - 1][nodes[0] - 1] += - 1;
	a[x - 1][nodes[1] - 1] += 1;

	if (main) {
		if (!secondTime)
			b[x - 1] -= complex<double>(amplitude * cos(angle * pi / 180), amplitude * sin(angle * pi / 180));
		else
			b[x - 1] -= dcLevel;
	}
}

void SINvoltageSource::setAuxSin(vector <complex<double> > auxSin){
	this->auxSin = auxSin;
}

vector <complex<double> > SINvoltageSource::getAuxSin() {return this->auxSin;}

void SINvoltageSource::setAuxSinDC(vector <complex<double> > auxSinDC){
	this->auxSinDC = auxSinDC;
}

vector <complex<double> > SINvoltageSource::getAuxSinDC() {return this->auxSinDC;}

void SINvoltageSource::setBoolDC(bool DC){
	this->DC = DC;
}
	
bool SINvoltageSource::getBoolDC(){return this->DC;}

// PULSE voltage Source
PULSEvoltageSource::PULSEvoltageSource(string sourceName, unsigned sourceNodeA, unsigned sourceNodeB, double sourceAmplitude1, double sourceAmplitude2, double sourceDelay, double sourceRiseTime, double sourceFallTime, double sourceOnTime, double sourcePeriod, double sourceNumCycles, unsigned nMax) {
	setType("PULSE Voltage Source");
	setName(sourceName);
	setNodes(sourceNodeA, sourceNodeB);
	setAmplitude1(sourceAmplitude1);
	setAmplitude2(sourceAmplitude2);
	setDelay(sourceDelay);
	setRiseTime(sourceRiseTime);
	setFallTime(sourceFallTime);
	setOnTime(sourceOnTime);
	setPeriod(sourcePeriod);
	setNumCycles(sourceNumCycles);
	setFrequency(1/sourcePeriod);
	numVoltageSource++;
	sizeAux++;
	x = sizeAux;
}

void PULSEvoltageSource::setAmplitude1(double sourceAmplitude1) {
	amplitude1 = sourceAmplitude1;
}

double PULSEvoltageSource::getAmplitude1() {
	return amplitude1;
}

void PULSEvoltageSource::setAmplitude2(double sourceAmplitude2) {
	amplitude2 = sourceAmplitude2;
}

double PULSEvoltageSource::getAmplitude2() {
	return amplitude2;
}

void PULSEvoltageSource::setDelay(double sourceDelay) {
	delay = sourceDelay;
}

double PULSEvoltageSource::getDelay() {
	return delay;
}

void PULSEvoltageSource::setRiseTime(double sourceRiseTime) {
	riseTime = sourceRiseTime;
}

double PULSEvoltageSource::getRiseTime() {
	return riseTime;
}

void PULSEvoltageSource::setFallTime(double sourceFallTime) {
	fallTime = sourceFallTime;
}

double PULSEvoltageSource::getFallTime() {
	return fallTime;
}

void PULSEvoltageSource::setOnTime(double sourceOnTime) {
	onTime = sourceOnTime;
}

double PULSEvoltageSource::getOnTime() {
	return onTime;
}

void PULSEvoltageSource::setPeriod(double sourcePeriod) {
	period = sourcePeriod;
}

double PULSEvoltageSource::getPeriod() {
	return period;
}

void PULSEvoltageSource::setNumCycles(double sourceNumCycles) {
	numCycles = sourceNumCycles;
}

double PULSEvoltageSource::getNumCycles() {
	return numCycles;
}

void PULSEvoltageSource::setFourierCoefficients(unsigned nMax) {

	double toff = (this->period - this->onTime - this->riseTime - this->fallTime);
	double c1 = (this->riseTime) ? (this->amplitude2 - this->amplitude1) / this->riseTime : 0;
	double c2 = (this->fallTime) ? (this->amplitude1 - this->amplitude2) / this->fallTime : 0;
	double k1 = (this->riseTime) ? this->amplitude1 - (this->amplitude2 - this->amplitude1) * toff / this->riseTime : this->amplitude1;
	double k2 = (this->fallTime) ? this->amplitude2 - (this->amplitude1 - this->amplitude2) * (this->period - this->fallTime) / this->fallTime : this->amplitude2;

	an.push_back(0);
	bn.push_back(0);

	for (unsigned n = 0; n <= nMax; n++) { 
		if (n == 0)
			this->a0 = 1 / this->period *
					(this->amplitude1 * toff +
					(this->amplitude2 - this->amplitude1) * (this->riseTime + 2 * toff) / 2 +
					this->amplitude1 * this->riseTime - toff * (this->amplitude2 - this->amplitude1) +
					this->amplitude2 * this->onTime +
					(this->amplitude1 + this->amplitude2) * this->fallTime / 2);
		else {
			//cout << "a0: " << a0 << endl;

			this->an.push_back (this->amplitude1 / (pi * n) * sin(2 * pi * n * toff / this->period) +
						(c1 / (pi * n) * (this->period / (2 * pi * n) * (cos(2 * pi * n *(toff + this->riseTime) / this->period) - cos(2 * pi * n * toff / this->period)) +
						(toff + this->riseTime) * sin(2 * pi * n * (toff + this->riseTime) / this->period) - toff * sin(2 * pi * n * toff / this->period)) +
						k1 / (pi * n) * (sin(2 * pi * n	* (toff + this->riseTime) / this->period) - sin(2 * pi * n * toff / this->period))) +
						this->amplitude2 / (pi * n) * (sin(2 * pi * n * (this->period - this->fallTime) / this->period) - sin(2 * pi * n * (toff + this->riseTime) / this->period)) +
						(c2 / (pi * n) * (this->period / (2 * pi * n) * (cos(2 * pi * n) - cos(2 * pi * n * (this->period - this->fallTime) / this->period)) +
						this->period * sin(2 * pi * n) - (this->period - this->fallTime) * sin(2 * pi * n * (this->period - this->fallTime) / this->period)) +
						k2 / (pi * n) * (sin(2 * pi * n) - sin(2 * pi * n * (this->period - this->fallTime) / this->period))));

			this->bn.push_back (this->amplitude1 / (pi * n) * (1 - cos(2 * pi * n * toff / this->period)) +
								(c1 / (pi * n) * (this->period / (2 * pi * n) * (sin(2 * pi * n * (toff + this->riseTime) / this->period) - sin(2 * pi * n * toff / this->period)) +
								toff * cos(2 * pi * n * toff / this->period) - (toff + this->riseTime) * cos(2 * pi * n * (toff + this->riseTime) / this->period)) +
								k1 / (pi * n) * (cos(2 * pi * n	* toff/ this->period) - cos(2 * pi * n * (toff + this->riseTime) / this->period))) +
								this->amplitude2 / (pi * n) * (cos(2 * pi * n * (toff + this->riseTime) / this->period) - cos(2 * pi * n * (this->period - this->fallTime) / this->period)) +
								(c2 / (pi * n) * (this->period / (2 * pi * n) * (sin(2 * pi * n) - sin(2 * pi * n * (this->period - this->fallTime) / this->period)) +
								(this->period - this->fallTime) * cos(2 * pi * n * (this->period - this->fallTime) / this->period) - this->period * cos(2 * pi * n)) +
								k2 / (pi * n) * (cos(2 * pi * n * (this->period - this->fallTime) / this->period) - cos(2 * pi * n))));

			//cout << "n : " << n << endl << "an: " << an.at(n) << endl << "bn: " << bn.at(n) << endl;
			//cin.get();
			}
	}
}

void PULSEvoltageSource::stamp(vector< vector<complex<double> > > &a, vector<complex<double> > &b, bool main, bool secondTime, unsigned n) {
	a[nodes[0] - 1][x - 1] += 1;
	a[nodes[1] - 1][x - 1] += - 1;
	a[x - 1][nodes[0] - 1] += - 1;
	a[x - 1][nodes[1] - 1] += 1;

	if (main) {
		if (secondTime)
			b[x - 1] -= complex<double>(this->an.at(n),-this->bn.at(n));
		else
			b[x - 1] -= complex<double>(this->a0, 0);
	}
}

void PULSEvoltageSource::setAuxPulse(vector <complex<double> > aux){
	this->auxPulse.push_back(aux);
}
vector<complex<double> > PULSEvoltageSource::getAuxPulse(unsigned n){
	return this->auxPulse[n];
}

// DC Current Source
DCcurrentSource::DCcurrentSource(string sourceName, unsigned sourceNodeA, unsigned sourceNodeB, double sourceValue) {
	setType("DC Current Source");
	setFrequency(0);
	setName(sourceName);
	setNodes(sourceNodeA, sourceNodeB);
	setValue(sourceValue);
}

void DCcurrentSource::setValue(double sourceValue) {
	value = sourceValue;
}

double DCcurrentSource::getValue() {
	return value;
}

void DCcurrentSource::stamp(vector< vector<complex<double> > > &a, vector<complex<double> > &b, bool main) {
	if (main) {
	b[nodes[0] - 1] += - value;
	b[nodes[1] - 1] += value;
	}
}

void DCcurrentSource::setAuxDC(vector <complex<double> > auxDC){
	this->auxDC = auxDC;
}

vector <complex<double> > DCcurrentSource::getAuxDc() {return this->auxDC;}

// SIN Current Source
SINcurrentSource::SINcurrentSource(string sourceName, unsigned sourceNodeA, unsigned sourceNodeB, double sourceDcLevel, double sourceAmplitude, double sourceFrequency, double sourceDelay, double sourceAttenuation, double sourceAngle, double sourceNumCycles) {
	setType("SIN Current Source");
	setName(sourceName);
	setNodes(sourceNodeA, sourceNodeB);
	setDcLevel(sourceDcLevel);
	setAmplitude(sourceAmplitude);
	setDelay(sourceDelay);
	setAttenuation(sourceAttenuation);
	setAngle(sourceAngle);
	setNumCycles(sourceNumCycles);
	setFrequency(sourceFrequency );
}

void SINcurrentSource::setDcLevel(double sourceDcLevel) {
	dcLevel = sourceDcLevel;
}

double SINcurrentSource::getDcLevel() {
	return dcLevel;
}

void SINcurrentSource::setAmplitude(double sourceAmplitude) {
	amplitude = sourceAmplitude;
}

double SINcurrentSource::getAmplitude() {
	return amplitude;
}


void SINcurrentSource::setDelay(double sourceDelay) {
	delay = sourceDelay;
}

double SINcurrentSource::getDelay() {
	return delay;
}

void SINcurrentSource::setAttenuation(double sourceAttenuation) {
	attenuation = sourceAttenuation;
}

double SINcurrentSource::getAttenuation() {
	return attenuation;
}

void SINcurrentSource::setAngle(double sourceAngle) {
	angle = sourceAngle;
}

double SINcurrentSource::getAngle() {
	return angle;
}

void SINcurrentSource::setNumCycles(double sourceNumCycles) {
	numCycles = sourceNumCycles;
}

double SINcurrentSource::getNumCycles() {
	return numCycles;
}

void SINcurrentSource::stamp(vector< vector<complex<double> > > &a, vector<complex<double> > &b, bool main, bool secondTime) {
	if(main) {
		if (!secondTime){
			b[nodes[0] - 1] -= complex<double>(amplitude * cos(angle * pi / 180), amplitude * sin(angle * pi / 180));
			b[nodes[1] - 1] += complex<double>(amplitude * cos(angle * pi / 180), amplitude * sin(angle * pi / 180));
		}
		else {
			b[nodes[0] - 1] -= dcLevel;
			b[nodes[1] - 1] += dcLevel;
		}
	}
}

void SINcurrentSource::setAuxSin(vector <complex<double> > auxSin){
	this->auxSin = auxSin;
}

vector <complex<double> > SINcurrentSource::getAuxSin() {return this->auxSin;}

void SINcurrentSource::setAuxSinDC(vector <complex<double> > auxSinDC){
	this->auxSinDC = auxSinDC;
}

vector <complex<double> > SINcurrentSource::getAuxSinDC() {return this->auxSinDC;}

void SINcurrentSource::setBoolDC(bool DC){
	this->DC = DC;
}
	
bool SINcurrentSource::getBoolDC(){return this->DC;}

// PULSE Current Source
PULSEcurrentSource::PULSEcurrentSource(string sourceName, unsigned sourceNodeA, unsigned sourceNodeB, double sourceAmplitude1, double sourceAmplitude2, double sourceDelay, double sourceRiseTime, double sourceFallTime, double sourceOnTime, double sourcePeriod, double sourceNumCycles, unsigned nMax) {
	setType("PULSE Current Source");
	setName(sourceName);
	setNodes(sourceNodeA, sourceNodeB);
	setAmplitude1(sourceAmplitude1);
	setAmplitude2(sourceAmplitude2);
	setDelay(sourceDelay);
	setRiseTime(sourceRiseTime);
	setFallTime(sourceFallTime);
	setOnTime(sourceOnTime);
	setNumCycles(sourceNumCycles);
	setPeriod(sourcePeriod);
	setFrequency(1/sourcePeriod);
}

void PULSEcurrentSource::setAmplitude1(double sourceAmplitude1) {
	amplitude1 = sourceAmplitude1;
}

double PULSEcurrentSource::getAmplitude1() {
	return amplitude1;
}

void PULSEcurrentSource::setAmplitude2(double sourceAmplitude2) {
	amplitude2 = sourceAmplitude2;
}

double PULSEcurrentSource::getAmplitude2() {
	return amplitude2;
}

void PULSEcurrentSource::setDelay(double sourceDelay) {
	delay = sourceDelay;
}

double PULSEcurrentSource::getDelay() {
	return delay;
}

void PULSEcurrentSource::setRiseTime(double sourceRiseTime) {
	riseTime = sourceRiseTime;
}

double PULSEcurrentSource::getRiseTime() {
	return riseTime;
}

void PULSEcurrentSource::setFallTime(double sourceFallTime) {
	fallTime = sourceFallTime;
}

double PULSEcurrentSource::getFallTime() {
	return fallTime;
}

void PULSEcurrentSource::setOnTime(double sourceOnTime) {
	onTime = sourceOnTime;
}

double PULSEcurrentSource::getOnTime() {
	return onTime;
}

void PULSEcurrentSource::setPeriod(double sourcePeriod) {
	period = sourcePeriod;
}

double PULSEcurrentSource::getPeriod() {
	return period;
}

void PULSEcurrentSource::setNumCycles(double sourceNumCycles) {
	numCycles = sourceNumCycles;
}

double PULSEcurrentSource::getNumCycles() {
	return numCycles;
}

void PULSEcurrentSource::setFourierCoefficients(unsigned nMax) {

	double toff = (this->period - this->onTime - this->riseTime - this->fallTime);
	double c1 = (this->riseTime) ? (this->amplitude2 - this->amplitude1) / this->riseTime : 0;
	double c2 = (this->fallTime) ? (this->amplitude1 - this->amplitude2) / this->fallTime : 0;
	double k1 = (this->riseTime) ? this->amplitude1 - (this->amplitude2 - this->amplitude1) * toff / this->riseTime : this->amplitude1;
	double k2 = (this->fallTime) ? this->amplitude2 - (this->amplitude1 - this->amplitude2) * (this->period - this->fallTime) / this->fallTime : this->amplitude2;
	cout << nMax;
	an.push_back(0);
	bn.push_back(0);

	for (unsigned n = 0; n <= nMax; n++) { 
		if (n == 0)
			this->a0 = 1 / this->period *
					(this->amplitude1 * toff +
					(this->amplitude2 - this->amplitude1) * (this->riseTime + 2 * toff) / 2 +
					this->amplitude1 * this->riseTime - toff * (this->amplitude2 - this->amplitude1) +
					this->amplitude2 * this->onTime +
					(this->amplitude1 + this->amplitude2) * this->fallTime / 2);
		else {
			cout << "a0: " << a0 << endl;

			this->an.push_back (this->amplitude1 / (pi * n) * sin(2 * pi * n * toff / this->period) +
						(c1 / (pi * n) * (this->period / (2 * pi * n) * (cos(2 * pi * n *(toff + this->riseTime) / this->period) - cos(2 * pi * n * toff / this->period)) +
						(toff + this->riseTime) * sin(2 * pi * n * (toff + this->riseTime) / this->period) - toff * sin(2 * pi * n * toff / this->period)) +
						k1 / (pi * n) * (sin(2 * pi * n	* (toff + this->riseTime) / this->period) - sin(2 * pi * n * toff / this->period))) +
						this->amplitude2 / (pi * n) * (sin(2 * pi * n * (this->period - this->fallTime) / this->period) - sin(2 * pi * n * (toff + this->riseTime) / this->period)) +
						(c2 / (pi * n) * (this->period / (2 * pi * n) * (cos(2 * pi * n) - cos(2 * pi * n * (this->period - this->fallTime) / this->period)) +
						this->period * sin(2 * pi * n) - (this->period - this->fallTime) * sin(2 * pi * n * (this->period - this->fallTime) / this->period)) +
						k2 / (pi * n) * (sin(2 * pi * n) - sin(2 * pi * n * (this->period - this->fallTime) / this->period))));

			this->bn.push_back (this->amplitude1 / (pi * n) * (1 - cos(2 * pi * n * toff / this->period)) +
								(c1 / (pi * n) * (this->period / (2 * pi * n) * (sin(2 * pi * n * (toff + this->riseTime) / this->period) - sin(2 * pi * n * toff / this->period)) +
								toff * cos(2 * pi * n * toff / this->period) - (toff + this->riseTime) * cos(2 * pi * n * (toff + this->riseTime) / this->period)) +
								k1 / (pi * n) * (cos(2 * pi * n	* toff/ this->period) - cos(2 * pi * n * (toff + this->riseTime) / this->period))) +
								this->amplitude2 / (pi * n) * (cos(2 * pi * n * (toff + this->riseTime) / this->period) - cos(2 * pi * n * (this->period - this->fallTime) / this->period)) +
								(c2 / (pi * n) * (this->period / (2 * pi * n) * (sin(2 * pi * n) - sin(2 * pi * n * (this->period - this->fallTime) / this->period)) +
								(this->period - this->fallTime) * cos(2 * pi * n * (this->period - this->fallTime) / this->period) - this->period * cos(2 * pi * n)) +
								k2 / (pi * n) * (cos(2 * pi * n * (this->period - this->fallTime) / this->period) - cos(2 * pi * n))));

			cout << "n : " << n << endl << "an: " << an.at(n) << endl << "bn: " << bn.at(n) << endl;
			//cin.get();
			}
	}
}

void PULSEcurrentSource::stamp(vector< vector<complex<double> > > &a, vector<complex<double> > &b, bool main, bool secondTime, unsigned n) {
	if (main) {
		if (secondTime){
			b[nodes[0] - 1]  -= complex<double>(this->an.at(n),-this->bn.at(n));
			b[nodes[1] - 1]  -= complex<double>(this->an.at(n),-this->bn.at(n));		
		}
		else{
			b[nodes[0] - 1] -= complex<double>(this->a0,0);
			b[nodes[1] - 1] += complex<double>(this->a0,0);
		}
	}
}

void PULSEcurrentSource::setAuxPulse(vector <complex<double> > aux){
	this->auxPulse.push_back(aux);
}
vector<complex<double> > PULSEcurrentSource::getAuxPulse(unsigned n){
	return this->auxPulse[n];
}


// Functions

// Solves a Ax = B system
void gaussianElimination (vector< vector<complex<double> > > matrixA, vector<complex<double> > vectorB, vector<complex<double> > &vectorX) {

	vector< vector<complex<double> > > a(matrixA.size() - 1, vector<complex<double> >(matrixA.size() - 1));
	vector<complex<double> > b(vectorB.size() - 1);
	vector<complex<double> > x(vectorX.size() - 1);
	vector<complex<double> > xIndex(vectorX.size() - 1);

	complex<double> c;


// Add elements to a and b
	for (unsigned column = 1; column < matrixA.size(); column++) {
		b[column - 1] = vectorB[column];
		xIndex[column - 1] = column - 1;
		for (unsigned line = 1; line < matrixA.size(); line++) {
			a[line - 1][column - 1] = matrixA[line][column];
		}
	}


// Makes sure no elements of a main diagonal are 0
	for (unsigned line = 0; line < a.size(); line++) {
		unsigned column = line;

		if (a[line][column] == complex<double>(0, 0)) {
			for (unsigned aux = 0; aux < a.size(); aux++) {
				if (a[aux][column] != complex<double>(0, 0)) {
					for (column = 0; column < a.size(); column++) {
						a[line][column] += a[aux][column];
					}
					b[line] += b[aux];
					break;

				}
			}
		}
	}


// Transforms a into a triangular matrix
	for (unsigned aux = 0; aux < a.size(); aux++) {

		if (a[aux][aux] == complex<double>(0, 0)) {
			complex<double> y;
			complex<double> i;

			for (unsigned line2 = aux + 1; line2 < a.size(); line2++) {
				if (a[line2][aux] != complex<double>(0, 0)) {
					for (unsigned column2 = 0; column2 < a.size(); column2++) {
						y = a[line2][column2];
						a[line2][column2] = a[aux][column2];
						a[aux][column2] = y;
					}

					i = xIndex[line2];
					xIndex[line2] = xIndex[aux];
					xIndex[aux] = i;


					y = b[line2];
					b[line2] = b[aux];
					b[aux] = y;

					break;
				}
			}
		}

		for (unsigned line = aux + 1; line < a.size(); line++) {
			c = a[line][aux] / a[aux][aux];
			b[line] = b[line] - b[aux] * c;
			for (unsigned column = 0; column < a.size(); column++) {
				a[line][column] = a[line][column] - a[aux][column] * c;
			}
		}
	}




// Solves for X
	for (int line = a.size() - 1; line >=  0 ; line--) {
		x[line] = b[line];
		for (int column = a.size() - 1; column > line; column--) {
			x[line] -= a[line][column] * x[column];
		}
		x[line] = x[line] / a[line][line];
	}

	vectorX[0] = 0;
	for (unsigned column = 1; column <= x.size(); column++) {
		vectorX[column] = x[column - 1];
	}

}




/*
void gaussianElimination (vector< vector<double> > matrixA, vector<double> vectorB, vector<double> &vectorX) {

	vector< vector<double> > y(matrixA.size() - 1, vector<double>(matrixA.size()));


// Add elements to y
	for (unsigned line = 1; line < matrixA.size(); line++) {
		for (unsigned column = 1; column < matrixA.size(); column++) {
			y[line - 1][column - 1] = matrixA[line][column];
		}
		y[line - 1][y.size()] = vectorB[line];
	}

	int i,j,l, a;
	double t, p;

	for (i = 0; i < (int) y.size(); i++) { // 0 - 2
		t = 0.0;
		a = i;

		for (l = i; l < (int) y.size(); l++) { // i - 2
			if (fabs(y[l][i]) > fabs(t)) {
				a = l;
				t = y[l][i];
			}
		}

		if (i != a) {
			for (l = 0; l < (int) y.size() + 1; l++) { // 0 - 3
				p = y[i][l];
				y[i][l] = y[a][l];
				y[a][l] = p;
			}
		}

		if (fabs(t) < 1e-9) {
			exit(1);
		}
		for (j = y.size(); j >= 0; j--) {  // Ponha j>0 em vez de j>i para melhor visualizacao // 3 - 0
			y[i][j] /= t;
			p = y[i][j];
			for (l = 0; l < (int) y.size(); l++) {
				if (l != i) {
					y[l][j] -=y [l][i] * p;
				}
			}
		}
	}

	for (unsigned line = 0; line < y.size(); line++) {
		vectorX[line + 1] = y[line][y[line].size() - 1]; // 35
	}

	vectorX[0] = 0;

}

*/


// Checks if the difference between all values of at1 and at0 are smaller than e
bool checkConvergence (vector<double> at1, vector<double> at0) {
	double e = 0.001;

	for (unsigned count = 0; count < at1.size(); count++) {
		if (fabs(at1[count] - at0[count]) > e) {
			return false;
		}
	}
	return true;
}


void timeAnalysis (vector<Element*> elements, vector< vector<complex<double> > > &a, vector<complex<double> > &b, vector<complex<double> > &x, double w, unsigned n) {

//	cout << "\nENTROU EM TIME ANALYSYS\n";

	for (unsigned count = 0; count < elements.size(); count++) {
//		cout << "\n\nTime Analysis " << count;
		//showAbx(a, b, x);
		if (elements[count]->getType() == "Resistor") {
			Resistor* resistor = (Resistor*) elements[count];
			resistor->stamp(a);
		}
		else if (elements[count]->getType() == "Capacitor") {
			Capacitor* capacitor = (Capacitor*) elements[count];
			capacitor->stamp(a, n *w);
		}
		else if (elements[count]->getType() == "Inductor") {
			Inductor* inductor = (Inductor*) elements[count];
			inductor->stamp(a, n * w);
		}
		else if (elements[count]->getType() == "Transformer") {
			Transformer* transformer = (Transformer*) elements[count];
			transformer->stamp(a, n * w);
		}
		else if (elements[count]->getType() == "OpAmp") {
			OpAmp* opamp = (OpAmp*) elements[count];
			opamp->stamp(a);
		}
		else if (elements[count]->getType() == "Voltage Controlled Voltage Source") {
			DependentSourceVV* dependentSourceVV = (DependentSourceVV*) elements[count];
			dependentSourceVV->stamp(a);
		}
		else if (elements[count]->getType() == "Current Controlled Current Source") {
			DependentSourceII* dependentSourceII = (DependentSourceII*) elements[count];
			dependentSourceII->stamp(a);
		}
		else if (elements[count]->getType() == "Voltage Controlled Current Source") {
			DependentSourceVI* dependentSourceVI = (DependentSourceVI*) elements[count];
			dependentSourceVI->stamp(a);
		}
		else if (elements[count]->getType() == "Current Controlled Voltage Source") {
			DependentSourceIV* dependentSourceIV = (DependentSourceIV*) elements[count];
			dependentSourceIV->stamp(a);
		}
	}

	gaussianElimination(a, b, x);
}

// Displays Netlist info on screen
void showInfo (vector<Element*> elements) {

	for(unsigned count = 0; count < elements.size(); count++) {
		if (elements[count]->getType() == "Resistor") {
			Resistor* resistor = (Resistor*) elements[count];
			cout << "\n\nName: " << resistor->getName() <<
					"\nNode A: " << resistor->getNodeA() <<
					"\nNode B: " << resistor->getNodeB() <<
					"\nValue: " << resistor->getResistance() <<
					"\nType: " << resistor->getType();
		}
		else if (elements[count]->getType() == "Capacitor") {
			Capacitor* capacitor = (Capacitor*) elements[count];
			cout << "\n\nName: " << capacitor->getName() <<
					"\nNode A: " << capacitor->getNodeA() <<
					"\nNode B: " << capacitor->getNodeB() <<
					"\nValue: " << capacitor->getCapacitance() <<
					"\nType: " << capacitor->getType();
		}
		else if (elements[count]->getType() == "Inductor") {
			Inductor* inductor = (Inductor*) elements[count];
			cout << "\n\nName: " << inductor->getName() <<
					"\nNode A: " << inductor->getNodeA() <<
					"\nNode B: " << inductor->getNodeB() <<
					"\nValue: " << inductor->getInductance() <<
					"\nType: " << inductor->getType();
		}
		else if (elements[count]->getType() == "Transformer") {
			Transformer* transformer = (Transformer*) elements[count];
			cout << "\n\nName: " << transformer->getName() <<
					"\nK: " << transformer->getK() <<
					"\nNode A: " << transformer->getNodeA() <<
					"\nNode B: " << transformer->getNodeB() <<
					"\nNode C: " << transformer->getNodeC() <<
					"\nNode D: " << transformer->getNodeD() <<
					"\nType: " << transformer->getType();
		}
		else if (elements[count]->getType() == "Voltage Controlled Voltage Source") {
			DependentSourceVV* dependentSourceVV = (DependentSourceVV*) elements[count];
			cout << "\n\nName: " << dependentSourceVV->getName() <<
					"\nNode A: " << dependentSourceVV->getNodeA() <<
					"\nNode B: " << dependentSourceVV->getNodeB() <<
					"\nNode C: " << dependentSourceVV->getNodeC() <<
					"\nNode D: " << dependentSourceVV->getNodeD() <<
					"\nType: " << dependentSourceVV->getAv();
		}
		else if (elements[count]->getType() == "Current Controlled Current Source") {
			DependentSourceII* dependentSourceII = (DependentSourceII*) elements[count];
			cout << "\n\nName: " << dependentSourceII->getName() <<
					"\nNode A: " << dependentSourceII->getNodeA() <<
					"\nNode B: " << dependentSourceII->getNodeB() <<
					"\nNode C: " << dependentSourceII->getNodeC() <<
					"\nNode D: " << dependentSourceII->getNodeD() <<
					"\nType: " << dependentSourceII->getAi();		}
		else if (elements[count]->getType() == "Voltage Controlled Current Source") {
			DependentSourceVI* dependentSourceVI = (DependentSourceVI*) elements[count];
			cout << "\n\nName: " << dependentSourceVI->getName() <<
					"\nNode A: " << dependentSourceVI->getNodeA() <<
					"\nNode B: " << dependentSourceVI->getNodeB() <<
					"\nNode C: " << dependentSourceVI->getNodeC() <<
					"\nNode D: " << dependentSourceVI->getNodeD() <<
					"\nType: " << dependentSourceVI->getGm();		}
		else if (elements[count]->getType() == "Current Controlled Voltage Source") {
			DependentSourceIV* dependentSourceIV = (DependentSourceIV*) elements[count];
			cout << "\n\nName: " << dependentSourceIV->getName() <<
					"\nNode A: " << dependentSourceIV->getNodeA() <<
					"\nNode B: " << dependentSourceIV->getNodeB() <<
					"\nNode C: " << dependentSourceIV->getNodeC() <<
					"\nNode D: " << dependentSourceIV->getNodeD() <<
					"\nType: " << dependentSourceIV->getRm();
		}
		//else if (elements[count]->getType() == "DC Voltage Source") {
		//	DCvoltageSource* dcVoltageSource = (DCvoltageSource*) elements[count];
		//	cout << "\n\nName: " << dcVoltageSource->getName() <<
		//			"\nNode A: " << dcVoltageSource->getNodeA() <<
		//			"\nNode B: " << dcVoltageSource->getNodeB() <<
		//			"\nV: " << dcVoltageSource->getValue();
		//}
		//else if (elements[count]->getType() == "SIN Voltage Source") {
		//	SINvoltageSource* sinVoltageSource = (SINvoltageSource*) elements[count];
		//	cout << "\n\nName: " << sinVoltageSource->getName() <<
		//			"\nNode A: " << sinVoltageSource->getNodeA() <<
		//			"\nNode B: " << sinVoltageSource->getNodeB() <<
		//			"\nV: " << sinVoltageSource->getDcLevel() <<
		//			"\nAmplitude: " << sinVoltageSource->getAmplitude() <<
		//			"\nFrequency: " << sinVoltageSource->getFrequency() <<
		//			"\nDelay: " << sinVoltageSource->getDelay() <<
		//			"\nAttenuation: " << sinVoltageSource->getAttenuation() <<
		//			"\nAngle: " << sinVoltageSource->getAngle() <<
		//			"\nNum Cycles: " << sinVoltageSource->getNumCycles();
		//}
		//else if (elements[count]->getType() == "PULSE Voltage Source") {
		//	PULSEvoltageSource* pulseVoltageSource = (PULSEvoltageSource*) elements[count];
		//	cout << "\n\nName: " << pulseVoltageSource->getName() <<
		//			"\nNode A: " << pulseVoltageSource->getNodeA() <<
		//			"\nNode B: " << pulseVoltageSource->getNodeB() <<
		//			"\nA1: " << pulseVoltageSource->getAmplitude1() <<
		//			"\nA2: " << pulseVoltageSource->getAmplitude2() <<
		//			"\nPeriod: " << pulseVoltageSource->getPeriod() <<
		//			"\nRise Time: " << pulseVoltageSource->getRiseTime() <<
		//			"\nOn Time: " << pulseVoltageSource->getOnTime() <<
		//			"\nFall Time: " << pulseVoltageSource->getFallTime() <<
		//			"\nNum Cycles: " << pulseVoltageSource->getNumCycles();
		//}
		//else if (elements[count]->getType() == "PULSE Current Source") {
		//	PULSEcurrentSource* pulseCurrentSource = (PULSEcurrentSource*) elements[count];
		//	cout << "\n\nName: " << pulseCurrentSource->getName() <<
		//			"\nNode A: " << pulseCurrentSource->getNodeA() <<
		//			"\nNode B: " << pulseCurrentSource->getNodeB() <<
		//			"\nA1: " << pulseCurrentSource->getAmplitude1() <<
		//			"\nA2: " << pulseCurrentSource->getAmplitude2() <<
		//			"\nPeriod: " << pulseCurrentSource->getPeriod() <<
		//			"\nRise Time: " << pulseCurrentSource->getRiseTime() <<
		//			"\nOn Time: " << pulseCurrentSource->getOnTime() <<
		//			"\nFall Time: " << pulseCurrentSource->getFallTime() <<
		//			"\nNum Cycles: " << pulseCurrentSource->getNumCycles();
		//}


	}

}


// Displays Netlist info on screen
void showInfo (vector<Source*> sources) {
	for(unsigned count = 0; count < sources.size(); count++) {
		if (sources[count]->getType() == "DC Voltage Source") {
			DCvoltageSource* dcVoltageSource = (DCvoltageSource*) sources[count];
			cout << "\n\nName: " << dcVoltageSource->getName() <<
					"\nNode A: " << dcVoltageSource->getNodeA() <<
					"\nNode B: " << dcVoltageSource->getNodeB() <<
					"\nV: " << dcVoltageSource->getValue();
		}
		else if (sources[count]->getType() == "SIN Voltage Source") {
			SINvoltageSource* sinVoltageSource = (SINvoltageSource*) sources[count];
			cout << "\n\nName: " << sinVoltageSource->getName() <<
					"\nNode A: " << sinVoltageSource->getNodeA() <<
					"\nNode B: " << sinVoltageSource->getNodeB() <<
					"\nV: " << sinVoltageSource->getDcLevel() <<
					"\nAmplitude: " << sinVoltageSource->getAmplitude() <<
					"\nFrequency: " << sinVoltageSource->getFrequency() <<
					"\nDelay: " << sinVoltageSource->getDelay() <<
					"\nAttenuation: " << sinVoltageSource->getAttenuation() <<
					"\nAngle: " << sinVoltageSource->getAngle() <<
					"\nNum Cycles: " << sinVoltageSource->getNumCycles();
		}
		else if (sources[count]->getType() == "PULSE Voltage Source") {
			PULSEvoltageSource* pulseVoltageSource = (PULSEvoltageSource*) sources[count];
			cout << "\n\nName: " << pulseVoltageSource->getName() <<
					"\nNode A: " << pulseVoltageSource->getNodeA() <<
					"\nNode B: " << pulseVoltageSource->getNodeB() <<
					"\nA1: " << pulseVoltageSource->getAmplitude1() <<
					"\nA2: " << pulseVoltageSource->getAmplitude2() <<
					"\nPeriod: " << pulseVoltageSource->getPeriod() <<
					"\nRise Time: " << pulseVoltageSource->getRiseTime() <<
					"\nFall Time: " << pulseVoltageSource->getFallTime() <<
					"\nNum Cycles: " << pulseVoltageSource->getNumCycles();
		}
		else if (sources[count]->getType() == "DC Current Source") {
			DCcurrentSource* dcCurrentSource = (DCcurrentSource*) sources[count];
			cout << "\n\nName: " << dcCurrentSource->getName() <<
					"\nNode A: " << dcCurrentSource->getNodeA() <<
					"\nNode B: " << dcCurrentSource->getNodeB() <<
					"\nV: " << dcCurrentSource->getValue();
		}
		else if (sources[count]->getType() == "SIN Current Source") {
			SINcurrentSource* sinCurrentSource = (SINcurrentSource*) sources[count];
			cout << "\n\nName: " << sinCurrentSource->getName() <<
					"\nNode A: " << sinCurrentSource->getNodeA() <<
					"\nNode B: " << sinCurrentSource->getNodeB() <<
					"\nV: " << sinCurrentSource->getDcLevel() <<
					"\nAmplitude: " << sinCurrentSource->getAmplitude() <<
					"\nFrequency: " << sinCurrentSource->getFrequency() <<
					"\nDelay: " << sinCurrentSource->getDelay() <<
					"\nAttenuation: " << sinCurrentSource->getAttenuation() <<
					"\nAngle: " << sinCurrentSource->getAngle() <<
					"\nNum Cycles: " << sinCurrentSource->getNumCycles();
		}
		else if (sources[count]->getType() == "PULSE Current Source") {
			PULSEcurrentSource* pulseCurrentSource = (PULSEcurrentSource*) sources[count];
			cout << "\n\nName: " << pulseCurrentSource->getName() <<
					"\nNode A: " << pulseCurrentSource->getNodeA() <<
					"\nNode B: " << pulseCurrentSource->getNodeB() <<
					"\nA1: " << pulseCurrentSource->getAmplitude1() <<
					"\nA2: " << pulseCurrentSource->getAmplitude2() <<
					"\nPeriod: " << pulseCurrentSource->getPeriod() <<
					"\nRise Time: " << pulseCurrentSource->getRiseTime() <<
					"\nOn Time: " << pulseCurrentSource->getOnTime() <<
					"\nFall Time: " << pulseCurrentSource->getFallTime() <<
					"\nNum Cycles: " << pulseCurrentSource->getNumCycles();
		}
	}

}


void showAbx (vector< vector<complex<double> > > a, vector<complex<double> > b, vector<complex<double> > x) {
	cout << "\nA\n";
	showMatrix(a);

	cout << "\nB\n";
	showVector(b);

	cout << "\nX\n";
	showVector(x);
}

void showVector(vector<complex<double> > &vec) {
	for (int columns = 0; columns < (int) vec.size(); columns++) {
		cout << "\t" << vec[columns];
	}
}

void showMatrix(vector< vector<complex<double> > > a) {
	for (int line = 0; line < (int) a.size(); line++) {
		showVector(a[line]);
		cout << "\n";
	}

}

void clearVector (vector<complex<double> > &vec) {
	for (unsigned count = 0; count < vec.size(); count++) {
		vec[count] = 0;
	}
}

void clearMatrix (vector< vector<complex<double> > > &mat) {
	for (unsigned count = 0; count < mat.size(); count++) {
		clearVector(mat[count]);
	}
}

void superposition(vector<complex<double> > &a, vector<complex<double> > b, float w, float t, bool pulse, bool coseno, unsigned n, double toff) {
//	cout << "\n\n\nVector B1:";
//	showVector(b);
//	cout << "\n\n\nVector A1:";
//	showVector(a);
	for (unsigned count = 0; count < a.size(); count++) {
//		cout << "\n\n\nb = " << b[count];
//		cout << "\nw = " << w;
//		cout << "\nt = " << t;
//		cout << "\nModule = " << abs(b[count]);
//		cout << "\nFase 1 = " << arg(b[count]);
//		cout << "\nFase 2 = " << arg(b[count]) * 180 / pi;
//		cout << "\nSin = " << sin(w * t + arg(b[count]));
//		cout << "\nSin = " << sin(w * t + arg(b[count]));
//		cout << "\nw * t = " << w * t;
		if (pulse) {
				a[count] += abs(b[count]) * cos(n * w * (t+toff) + arg(b[count]));
				//cout << endl << "ABS: " << abs(b[count]) << endl;
				//cout << "ARG: " << arg(b[count]) << endl;
				//cout << "real: " << real(b[count]) << endl;
				//cout << "imag: " << imag(b[count]) << endl;
				//cout << "abs * seno(arg): " << abs(abs(b[count]))*sin(abs(b[count])) << endl;
				//cout << "abs * cos(arg): " << abs(abs(b[count]))*cos(abs(b[count])) << endl;
				//cout << "real * seno(imag): " << abs(real(b[count]))*sin(imag(b[count])) << endl;
				//cout << "real * cos(imag): " << abs(real(b[count]))*cos(imag(b[count])) << endl;
		}
		else {
			if (w != 0) {
				a[count] += abs(b[count]) * sin(w * t + arg(b[count]));
			}
			else {
				a[count] += real(b[count]);
			}
		}
	}
//	cout << "\n\n\nVector B2:";
//	showVector(b);
//	cout << "\n\n\nVector A2:";
//	showVector(a);
}

vector<complex<double> > multiplyVector (vector<complex<double> > vec, complex<double> k) {
	for (unsigned count = 0; count < vec.size(); count++) {
		vec[count] = vec[count] * k;
	}
	return vec;
}
