#pragma once
#ifndef _SIMANNEALING_H_
#define _SIMANNEALING_H_
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

class SimAnnealing
{
	int ** distances; // tablica kosztów odleglosci
	int amount; // l. miast
	int stopTime;
	double T, result, averageTime, averageResult, bestResult, worstResult;
	double * constants;
	double PCFreq = 0.0;
	__int64 CounterStart = 0;
	void StartCounter();
	double GetCounter();
public:
	SimAnnealing();
	~SimAnnealing();
	void loadFromFile(string name);
	void generatePermutation(int *& permutation);
	int Path(int * permutation);
	int Probability(int a, int b);
	void mainFunction(int counter, string name, int stopTime);
};

#endif