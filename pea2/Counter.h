#pragma once
#include <Windows.h>
#ifndef _COUNTER_H_
#define _COUNTER_H_
#include <iostream>

using namespace std;

double PCFreq = 0.0;
__int64 CounterStart = 0;

void StartCounter() // licznik 
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
		cout << "QueryPerformanceFrequency failed!\n";

	PCFreq = double(li.QuadPart);// / 1000.0;

	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
}

double GetCounter() // zakonczenie liczenia
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart - CounterStart) / PCFreq;
}

#endif