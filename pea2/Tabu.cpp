#include "TabuSearch.h"
#include "SimAnnealing.h"
#include <ctime>


void smMain()
{
	srand(time(NULL));
	string name;
	int counter;
	int stopTime;
	SimAnnealing * SM = new SimAnnealing();
	cout << "Podaj nazwe pliku ATSP : "; cin >> name;
	cout << "Podaj ilosc powtorzen algorytmu : "; cin >> counter;
	cout << "Podaj czas stopu : "; cin >> stopTime;
	SM->loadFromFile(name);
	SM->mainFunction(counter, name, stopTime);
	cout << "Koniec dzialania programu" << endl;
	delete SM;
}

void tabuMain()
{
	srand(time(NULL));
	string name;
	int counter;
	int stopTime;
	TabuSearch * TB = new TabuSearch();
	cout << "Podaj nazwe pliku ATSP : "; cin >> name;
	cout << "Podaj ilosc powtorzen algorytmu : "; cin >> counter;
	cout << "Podaj czas stopu : "; cin >> stopTime;
	TB->loadCounter(counter); // laduje licznik 
	TB->loadFromFile(name); // laduje plik
	TB->mainFunction(stopTime); // przeskakujemy do funkcji 
	cout << "Koniec dzialania programu" << endl;
	delete TB;
}

int main() // glowna funkcja 
{

	int choose = 0;

	while (choose != 1 && choose != 2) {
		cout << "Wybierz algorytm : " << endl;
		cout << "1. Tabu" << endl;
		cout << "2. Wyzarzanie" << endl;
		cin >> choose;
	}

	if (choose == 1)
	{
		tabuMain();
	}
	else
	{
		smMain();
	}
	system("pause");
	return 0;
}