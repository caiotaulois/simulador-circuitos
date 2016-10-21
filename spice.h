/*
 * spice.h
 *
 *  Created on: Jun 10, 2013
 *      Author: Filipe Barretto
 *              Caio Taulois
 */

#ifndef SPICE_H_
#define SPICE_H_


#include <string>
#include <vector>
#include <complex>

using namespace std;

// Global Variables
// Time Variables
extern double t;
extern double dt;
extern double dtbe;
extern double maxT;
extern unsigned nMax;
extern unsigned numSeries;
extern bool uic;

// Constants
extern const double pi;
extern const complex<double> j;

// Element Counters
extern unsigned numResistors;
extern unsigned numCapacitors;
extern unsigned numInductors;

extern unsigned numDependentSourceVV;
extern unsigned numDependentSourceII;
extern unsigned numDependentSourceVI;
extern unsigned numDependentSourceIV;

extern unsigned numOpAmp;

extern unsigned numDiode;
extern unsigned numTransistor;

extern unsigned numVoltageSource;
extern unsigned numCurrentSource;

// Auxiliary Global Variables
extern unsigned sizeAux;

// Elements
class Element {
public:
	Element();
	void setType(string);
	string getType();
	void setName(string);
	string getName();
	void setNodes(unsigned, unsigned);
	void setNodes(unsigned, unsigned, unsigned, unsigned);
	unsigned getNodeA();
	unsigned getNodeB();
	unsigned getNodeC();
	unsigned getNodeD();

protected:
	string name;
	string type;
	vector<unsigned> nodes;
};


class Source : public Element {
public:
	Source();
	void setFrequency(double);
	double getFrequency();
private:
	double w;
};

class Resistor : public Element {
public:
	Resistor(string, unsigned, unsigned, double);
	void setResistance(double);
	double getResistance();
	void stamp(vector< vector<complex<double> > > &);
private:
	double resistance;
};


class Capacitor : public Element {
public:
	Capacitor(string, unsigned, unsigned, double);
	void setCapacitance(double);
	double getCapacitance();
	void stamp(vector< vector<complex<double> > > &, double);
private:
	double capacitance;
};


class Inductor : public Element {
public:
	Inductor(string, unsigned, unsigned, double);
	void setInductance(double);
	double getInductance();
	void stamp(vector< vector<complex<double> > > &, double);
	void setX(unsigned);
	unsigned getX();
private:
	double inductance;
	unsigned x;
};


class Transformer : public Element {
public:
	Transformer(double, unsigned, unsigned, unsigned, double, unsigned, unsigned, unsigned, double);
	void setK(double);
	double getK();
	void setL1(double);
	double getL1();
	void setL2(double);
	double getL2();
	void stamp(vector< vector<complex<double> > > &, double);
private:
	double k;
	double l1;
	double l2;
	unsigned x;
	unsigned y;
};



class DependentSourceVV : public Element {
public:
	DependentSourceVV(string, unsigned, unsigned, unsigned, unsigned, double);
	void setAv(double);
	double getAv();
	void stamp(vector< vector<complex<double> > > &);
private:
	double av;
	unsigned x;
};


class DependentSourceII : public Element {
public:
	DependentSourceII(string, unsigned, unsigned, unsigned, unsigned, double);
	void setAi(double);
	double getAi();
	void stamp(vector< vector<complex<double> > > &);
private:
	double ai;
	unsigned x;
};


class DependentSourceVI : public Element {
public:
	DependentSourceVI(string, unsigned, unsigned, unsigned, unsigned, double);
	void setGm(double);
	double getGm();
	void stamp(vector< vector<complex<double> > > &);
private:
	double gm;
};


class DependentSourceIV : public Element {
public:
	DependentSourceIV(string, unsigned, unsigned, unsigned, unsigned, double);
	void setRm(double);
	double getRm();
	void stamp(vector< vector<complex<double> > > &);
private:
	double rm;
	unsigned x;
	unsigned y;
};


class DCvoltageSource : public Source {
public:
	DCvoltageSource(string, unsigned, unsigned, double);
	void setValue(double);
	double getValue();
	void setAuxDC(vector <complex<double> >);
	vector <complex<double> > getAuxDc();
	void stamp(vector< vector<complex<double> > > &, vector<complex<double> > &, bool);
private:
	double value;
	unsigned x;
	vector <complex<double> > auxDC;
};


class OpAmp : public Element {
public:
	OpAmp(string, unsigned, unsigned, unsigned, unsigned);
	void stamp(vector< vector<complex<double> > > &);
private:
	unsigned x;
};


class SINvoltageSource : public Source {
public:
	SINvoltageSource(string, unsigned, unsigned, double, double, double, double, double, double, double);
	void setDcLevel(double);
	double getDcLevel();
	void setAmplitude(double);
	double getAmplitude();
	void setDelay(double);
	double getDelay();
	void setAttenuation(double);
	double getAttenuation();
	void setAngle(double);
	double getAngle();
	void setNumCycles(double);
	double getNumCycles();
	void setAuxSin(vector <complex<double> >);
	vector <complex<double> > getAuxSin();
	void setAuxSinDC(vector <complex<double> >);
	vector <complex<double> > getAuxSinDC();
	void setBoolDC(bool);
	bool getBoolDC();

	void stamp(vector< vector<complex<double> > > &, vector<complex<double> > &, bool, bool);
private:
	double dcLevel;
	double amplitude;
	double delay;
	double attenuation;
	double angle;
	double numCycles;
	unsigned x;
	bool DC;
	vector <complex<double> > auxSin;
	vector <complex<double> > auxSinDC;
};


class PULSEvoltageSource : public Source {
public:
	PULSEvoltageSource(string, unsigned, unsigned, double, double, double, double, double, double, double, double, unsigned);
	void setAmplitude1(double);
	double getAmplitude1();
	void setAmplitude2(double);
	double getAmplitude2();
	void setDelay(double);
	double getDelay();
	void setRiseTime(double);
	double getRiseTime();
	void setFallTime(double);
	double getFallTime();
	void setOnTime(double);
	double getOnTime();
	void setPeriod(double);
	double getPeriod();
	void setNumCycles(double);
	double getNumCycles();
	void setFourierCoefficients(unsigned);
	double getFourierCoefficients();
	void setAuxPulse(vector <complex<double> >);
	vector<complex<double> > getAuxPulse(unsigned);

	void stamp(vector< vector<complex<double> > > &, vector<complex<double> > &, bool, bool, unsigned);
private:
	double amplitude1;
	double amplitude2;
	double delay;
	double riseTime;
	double fallTime;
	double onTime;
	double period;
	double numCycles;
	unsigned x;
	double a0;
	vector <double> an;
	vector <double> bn;
	vector< vector <complex<double> > > auxPulse;
};

class DCcurrentSource : public Source {
public:
	DCcurrentSource(string, unsigned, unsigned, double);
	void setValue(double);
	double getValue();
	void setAuxDC(vector <complex<double> >);
	vector <complex<double> > getAuxDc();
	void stamp(vector< vector<complex<double> > > &, vector<complex<double> > &, bool);
private:
	double value;
	vector <complex<double> > auxDC;
};


class SINcurrentSource : public Source {
public:
	SINcurrentSource(string, unsigned, unsigned, double, double, double, double, double, double, double);
	void setDcLevel(double);
	double getDcLevel();
	void setAmplitude(double);
	double getAmplitude();
	void setDelay(double);
	double getDelay();
	void setAttenuation(double);
	double getAttenuation();
	void setAngle(double);
	double getAngle();
	void setNumCycles(double);
	double getNumCycles();
	void setAuxSin(vector <complex<double> >);
	vector <complex<double> > getAuxSin();
	void setAuxSinDC(vector <complex<double> >);
	vector <complex<double> > getAuxSinDC();
	void setBoolDC(bool);
	bool getBoolDC();

	void stamp(vector< vector<complex<double> > > &, vector<complex<double> > &, bool, bool);
private:
	double dcLevel;
	double amplitude;
	double delay;
	double attenuation;
	double angle;
	double numCycles;
	bool DC;
	vector <complex<double> > auxSin;
	vector <complex<double> > auxSinDC;
};

class PULSEcurrentSource : public Source {
public:
	PULSEcurrentSource(string, unsigned, unsigned, double, double, double, double, double, double, double, double, unsigned);
	void setAmplitude1(double);
	double getAmplitude1();
	void setAmplitude2(double);
	double getAmplitude2();
	void setDelay(double);
	double getDelay();
	void setRiseTime(double);
	double getRiseTime();
	void setFallTime(double);
	double getFallTime();
	void setOnTime(double);
	double getOnTime();
	void setPeriod(double);
	double getPeriod();
	void setNumCycles(double);
	double getNumCycles();
	void setFourierCoefficients(unsigned);
	double getFourierCoefficients();
	void setAuxPulse(vector <complex<double> >);
	vector<complex<double> > getAuxPulse(unsigned);

	void stamp(vector< vector<complex<double> > > &, vector<complex<double> > &, bool, bool, unsigned);
private:
	double amplitude1;
	double amplitude2;
	double delay;
	double riseTime;
	double fallTime;
	double onTime;
	double period;
	double numCycles;
	double a0;
	vector <double> an;
	vector <double> bn;
	vector< vector <complex<double> > > auxPulse;
};




// Functions
// Solves a Ax = B system
void gaussianElimination (vector< vector<complex<double> > >, vector<complex<double> >, vector<complex<double> > &);

// Checks if the difference between all values of at1 and at0 are smaller than e
bool checkConvergence (vector<double>, vector<double>);

//
void timeAnalysis (vector<Element*>, vector< vector<complex<double> > > &, vector<complex<double> > &, vector<complex<double> > &, double, unsigned);

// Displays Netlist info on screen
void showInfo (vector<Element*>);

// Displays Netlist info on screen
void showInfo (vector<Source*>);

// Displays A, B and X on screen
void showAbx (vector< vector<complex<double> > >, vector<complex<double> >, vector<complex<double> >);

void showVector(vector<complex<double> > &);

void showMatrix(vector< vector<complex<double> > > a);

void clearVector(vector<complex<double> > &);

void clearMatrix (vector< vector<complex<double> > > &mat);

void superposition(vector<complex<double> > &, vector<complex<double> >, float, float, bool, bool, unsigned, double);

vector<complex<double> > multiplyVector (vector<complex<double> >, complex<double>);


#endif /* SPICE_H_ */
