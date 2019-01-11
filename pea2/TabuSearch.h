#pragma once
#ifndef _TABUSEARCH_H_
#define _TABUSEARCH_H_
#include <string>
#include <fstream>
#include <iostream>
using namespace std;

class TabuSearch
{
	double PCFreq = 0.0;
	__int64 CounterStart = 0;
	void StartCounter();
	double GetCounter();
	int ** distances;
	int ** tabuList;
	int amount, counter, endCondition, cadency, endThroughRefuse, changePermutation;
	double worstSolution, bestSolution, averageSolution, averageTime, Time;
	int * bestPermutation;
public:
	TabuSearch();
	~TabuSearch();
	void loadCounter(int counter);
	void loadFromFile(string name);
	void generatePermutation(int *& permutation);
	int Path(int * permutation);
	void updateTabuList();
	void mainFunction(int stopTime);
};

#endif