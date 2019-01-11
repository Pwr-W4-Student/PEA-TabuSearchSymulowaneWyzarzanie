#include "TabuSearch.h"
#include <iostream>
#include <windows.h>

void TabuSearch::StartCounter() // licznik 
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
		cout << "QueryPerformanceFrequency failed!\n";

	PCFreq = double(li.QuadPart);// / 1000.0;

	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
}

double TabuSearch::GetCounter() // zakonczenie liczenia
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart - CounterStart) / PCFreq;
}

TabuSearch::TabuSearch()
{
	this->endCondition = 60;
	this->bestSolution = pow(2, 54); // najlepsze rozwiazanie 
	this->endThroughRefuse = 10; // 10 razy bedzie zmiana pod rzad permutacji poczatkowej i nie bedzie znajdywana lepsza od obecnie najleszego rozwiazania to znaczy ze mamy calkiem niezle to rozwiazanie i mozna zakonczyc 
	this->changePermutation = 0; // liczba zmian permutacji 
	this->worstSolution = 0; // najgorsze rozwiazanie
	this->averageTime = 0; // sredni czas 
	this->averageSolution = 0;

}

TabuSearch::~TabuSearch()
{
	for (int i = 0; i < amount; i++)
	{
		delete[] distances[i];
		delete[] tabuList[i];
	}
	delete[] distances;
	delete[] tabuList;
	delete[] bestPermutation;
}

void TabuSearch::loadCounter(int counter)
{
	this->counter = counter;
}

void TabuSearch::loadFromFile(string name)      // wczytywanie pliku
{
	string line, value, description;
	int help;
	ifstream file(name + ".atsp");
	if (!file)
	{
		cout << "Nie udalo sie otworzyc pliku!" << endl;
		system("pause");
		exit(0);
	}
	else
	{
		cout << "Wczytano plik ";
		do
		{
			getline(file, line);
			help = line.find(':');
			cout << line << endl;
			if (help > 0)
			{
				description = line.substr(0, help);
				value = line.substr(help + 1);
				if (!description.compare("DIMENSION"))
					this->amount = atoi(value.c_str());
			}
		} while (line.compare("EDGE_WEIGHT_SECTION"));

		this->distances = new int *[amount];
		for (int i = 0; i < amount; i++)
		{
			this->distances[i] = new int[amount];
			for (int j = 0; j < amount; j++)
				file >> distances[i][j];
		}
		this->bestPermutation = new int[amount];
		this->tabuList = new int*[amount];
		for (int i = 0; i < amount; i++)
			this->tabuList[i] = new int[amount];
		this->cadency = amount;
		cout << "Wczytano wszystkie dane z pliku" << endl << endl;
		file.close();
	}
}

void TabuSearch::generatePermutation(int *& permutation) // generowanie permutacji 
{
	int * temporary = new int[amount];
	int q;
	for (int i = 0; i < amount; i++) // indeksy miast 
		temporary[i] = i;
	for (int i = amount; i>0; i--)
	{
		q = rand() % i;
		permutation[i - 1] = temporary[q]; // losowanie indeksu zeby przypisac do permutacji 
		temporary[q] = temporary[i - 1];
	}
	delete[] temporary;
}

int TabuSearch::Path(int * permutation) // wyliczanie sciezki (koszt przejscia)
{
	int distance = 0;
	for (int i = 0; i < amount - 1; i++)
		distance += this->distances[permutation[i]][permutation[i + 1]];  // szukam jakie sa koszty miedzy i a i+1
	distance += this->distances[permutation[amount - 1]][permutation[0]]; // zamkniecie obiegu 
	return distance;
}

void TabuSearch::updateTabuList()  // zmienjszanie kadencji o 1 
{
	for (int i = 0; i < amount; i++)
	{
		for (int j = 0; j < amount; j++)
		{
			if (tabuList[i][j]>0)
				tabuList[i][j]--;
		}
	}
}

void TabuSearch::mainFunction(int stopTime) // funkcja glowna
{
	
	for (int c = 0; c < 10; c++) // ilosc wielkosci
	{
		for (int l = 0; l < this->counter; l++) // liczba iteracji algorytmu 
		{
			int firstCity, secondCity, firstTabu, secondTabu;
			int isEnd = 0; // zmienna sprawdzajaca czy zakonczyc 
			for (int i = 0; i < amount; i++)
				for (int j = 0; j < amount; j++)
					this->tabuList[i][j] = 0; // zape³niam tabu list zerami na poczatku 
			int * firstPermutation = new int[amount];
			int * secondPermutation = new int[amount];
			int * currSolution = new int[amount];
			StartCounter(); // start t 
			this->generatePermutation(firstPermutation); // wygenerowanie 1 permutacji 
			for (int i = 0; i < amount; i++)
				this->bestPermutation[i] = firstPermutation[i]; // best
			while (isEnd < this->endCondition && GetCounter() <= stopTime)				// iteruje sie dopoki nie jest spelniony warunek zakonczenia(endCondition 60, 120...)
			{
				bool needToChange = false; // czy trzeba zmieniac permutacje
				int mval = 0; // minimalna wartosc odleglosci
				for (int i = 0; i < amount; i++)
					secondPermutation[i] = firstPermutation[i]; // przypisanie do 2 permutacji

				for (int i = 0; i < amount - 1; i++)						//przeszukiwanie sasiedztwa
				{
					for (int j = i + 1; j < amount; j++) // sprawdzanie wszystkich mozliwosci zamiany
					{
						firstCity = i; secondCity = j;
						secondPermutation[firstCity] = firstPermutation[secondCity]; // zamienia miasta 
						secondPermutation[secondCity] = firstPermutation[firstCity];
						int sub = this->Path(firstPermutation) - this->Path(secondPermutation); // liczy sie roznice odleglosci
						if (sub>mval)		//jezeli znaleziono lepsze mval
						{
							if (tabuList[i][j] == 0 || this->Path(secondPermutation) < this->Path(bestPermutation)*0.95)		//sprawdzenie tabuList oraz kryterium aspiracji,
							{
								mval = sub;
								for (int k = 0; k < amount; k++)
									currSolution[k] = secondPermutation[k]; // przypisujemy 
								firstTabu = firstCity; // wpisanie indeksow miast ktore zamieniamy, zeby dodac do tabu list
								secondTabu = secondCity;
								needToChange = true; // zmiana obecnego rozwiazania, bo znalezlismy lepsze 
							}
						}
						secondPermutation[firstCity] = firstPermutation[firstCity]; // wracamy do poprzedniego stanu
						secondPermutation[secondCity] = firstPermutation[secondCity];
					}
				}

				if (needToChange == true) // skoro znalezlismy najlepsze to teraz robimy dla niego, czyli przepisujemy do 1 permutacji curroSolutions(najlepsze rozwiazanie dla petli zamiany miast(sasiedztwa))
				{
					for (int i = 0; i < amount; i++)						//x0<-x
						firstPermutation[i] = currSolution[i]; // przepisujemy
					this->tabuList[firstTabu][secondTabu] = this->cadency; // dodajemy do tablicy kadencje ktora jest sztywno ustalona (10)
					this->tabuList[secondTabu][firstTabu] = this->cadency;
					isEnd++; // ziwksza sie bo wykonany ruch
					this->changePermutation = 0; // zerujemy bo ona oznacza czy musimy zmienic obecna permutacje, nie trzeba bo znalezlismy juz najlepsze 
				}
				else	// jesli nie znalezlismy najlepszej permutacji to generujemy nowa bo tam wyzej nic nie polepszy³o		
				{
					this->generatePermutation(firstPermutation);				//ewentualna zmiana permutacji
					this->changePermutation++; // zwiekszamy licznik 
					if (this->changePermutation > this->endThroughRefuse)			//zakonczenie w przypadku nie znalezienia lepszego, tyle razy zmienialismy permutacje a nie znajdujemy rozwiazania, to nie bedzie lepiej
																					// i tu badamy czy 10 razy byla zmiana i nie bylo porawy i od razu warunek koncowy i wychodzi
						isEnd = this->endCondition;
				}

				if (this->Path(firstPermutation) < this->Path(bestPermutation))			//	bada czy obecna badana permutacja jest lepsza od obecnie najlepszej
				{
					for (int i = 0; i < amount; i++)
						this->bestPermutation[i] = firstPermutation[i]; // no i jesli jest to nadpisujemy 
				}

				this->updateTabuList(); // zmniejsza kadencje jest funkcja wyzej 
			}
			this->Time = GetCounter(); // zwraca czas
			delete[] firstPermutation;
			delete[] secondPermutation;
			delete[] currSolution;
			int x = this->Path(bestPermutation); // podsumowanie, wybiera z 10 razy najlepsze, najgorsze, liczy srednia
			if (x < this->bestSolution)
				this->bestSolution = x;
			if (x > this->worstSolution)
				this->worstSolution = x;
			averageSolution += x;
			averageTime += Time;
		}
		averageTime /= counter; // liczy srednie przez to 10
		averageSolution /= counter;
		cout << endl << "Ilosc krokow algorytmu: " << this->endCondition << endl;
		cout << "Sredni czas wykonania " << averageTime << " sekund" << endl;
		cout << "Najlepsze znalezione rozwiazanie : " << this->bestSolution << endl;
		cout << "Najgorsze znalezione rozwiazanie : " << this->worstSolution << endl;
		cout << "Srednie rozwiazanie : " << (int)this->averageSolution << endl;
		this->bestSolution = pow(2, 54);
		this->worstSolution = 0;
		this->averageSolution = 0;
		this->averageTime = 0;
		this->endCondition *= 2; // zwiekszenie liczby iteracji dotyczacych wielkosci 60*2... 
	}
}