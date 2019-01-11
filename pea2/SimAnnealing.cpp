#include "SimAnnealing.h"
#include <iostream>
#include <windows.h>

void SimAnnealing::StartCounter() // licznik 
{
	LARGE_INTEGER li;
	if (!QueryPerformanceFrequency(&li))
		cout << "QueryPerformanceFrequency failed!\n";

	PCFreq = double(li.QuadPart);// / 1000.0;

	QueryPerformanceCounter(&li);
	CounterStart = li.QuadPart;
}

double SimAnnealing::GetCounter() // zakonczenie liczenia
{
	LARGE_INTEGER li;
	QueryPerformanceCounter(&li);
	return double(li.QuadPart - CounterStart) / PCFreq;
}

SimAnnealing::SimAnnealing()
{
	this->averageResult = 0.0;
	this->averageTime = 0.0;
	this->bestResult = pow(2, 54);
	this->worstResult = 0.0;
	this->constants = new double[10];
	this->constants[0] = 0.75;
	this->constants[1] = 0.8;
	this->constants[2] = 0.85;
	this->constants[3] = 0.9;
	this->constants[4] = 0.95;
	this->constants[5] = 0.975;
	this->constants[6] = 0.999;
	this->constants[7] = 0.9999;
	this->constants[8] = 0.99999;
	this->constants[9] = 0.999999;
}

SimAnnealing::~SimAnnealing()
{
	for (int i = 0; i < amount; i++)
		delete[] distances[i];
	delete[] distances;
	delete[] constants;
}

void SimAnnealing::loadFromFile(string name)
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
		cout << "Wczytano wszystkie dane do pliku" << endl << endl;
		file.close();
	}
}

int SimAnnealing::Path(int * permutation) // wyliczanie sciezki ( koszt przejscia)
{
	int distance = 0;
	for (int i = 0; i < amount - 1; i++)
		distance += this->distances[permutation[i]][permutation[i + 1]]; // szukam jakie sa koszty miedzy i a i+1
	distance += this->distances[permutation[amount - 1]][permutation[0]]; //zamkniecie obiegu
	return distance;
}

void SimAnnealing::generatePermutation(int *& permutation)
{
	int * temporary = new int[amount];
	int q;
	for (int i = 0; i < amount; i++) // indeky do miast
		temporary[i] = i;
	for (int i = amount; i>0; i--)
	{
		q = rand() % i;
		permutation[i - 1] = temporary[q]; // losowanie indeksu zeby przypisac do permutacji
		temporary[q] = temporary[i - 1];
	}
	delete[] temporary;
}

int SimAnnealing::Probability(int a, int b)  // prawdopodobienatwo przyjecia gorszej sciezki
{
	double q = pow(2.71828182845904523536, ((-1 * (b - a)) / this->T));	//wg wzoru
	double r = (double)rand() / RAND_MAX;
	if (r < q)
		return 1; // gorsza sciezka
	else
		return 0;
}

void SimAnnealing::mainFunction(int counter, string name, int stopTime)
{
	double minT = 0.01; // min. temp

						// iteracja po zmiennych
	for (int l = 0; l < 10; l++)
	{
		for (int p = 0; p < counter; p++) // liczba iteracji algorytmu
		{
			int Solution = INT_MAX;
			StartCounter(); // start t
			int * firstPermutation = new int[amount]; // permutacje miast
			int * secondPermutation = new int[amount];
			int firstCity, secondCity, a, b;
			double temporaryDifference, difference = 0;
			for (int i = 0; i < amount; i++) // tyle ile miast, wyznaczenie temperatury
			{
				this->generatePermutation(firstPermutation); // generowanie permutacji
				this->generatePermutation(secondPermutation);
				temporaryDifference = abs(this->Path(firstPermutation) - this->Path(secondPermutation)); // obliczanie roznicy odleglosci
				if (temporaryDifference>difference) // szuka najwiekszej i nadpisuje
					difference = temporaryDifference;
				delete[] firstPermutation;
				delete[] secondPermutation;
				firstPermutation = new int[amount];
				secondPermutation = new int[amount];
			}
			this->T = difference; // wyliczona temp
			generatePermutation(firstPermutation); // genereuje losowa permutacje
			a = this->Path(firstPermutation); // liczymy sciezke dla permutacji
			for (int i = 0; i < amount; i++)
				secondPermutation[i] = firstPermutation[i]; // przepisuje zeby 

			while (this->T>minT && GetCounter() <= stopTime) // dopoki temp bedzie wieksza od minimalnej
			{
				do {
					firstCity = rand() % amount; // losowanie miast do zamiany 
					secondCity = rand() % amount;
				} while (firstCity == secondCity);

				secondPermutation[secondCity] = firstPermutation[firstCity]; // zamiana miast
				secondPermutation[firstCity] = firstPermutation[secondCity];
				b = this->Path(secondPermutation); // liczymy 2 sciezke 

				if (b <= a || this->Probability(a, b) == 1)  // sprawdzam czy b >= a lub obliczam prawdopodobienstwo
				{ // lcize ktora sciezka jest lepsza 
					a = b;
					if (a <= Solution) // sprawdzam czy sciezka jest lepsza niz obecne rozwiazanie
						Solution = a;
					firstPermutation[firstCity] = secondPermutation[firstCity];
					firstPermutation[secondCity] = secondPermutation[secondCity]; // przyjmujemy 2 permutacje jako kolejna wyjsciowa
				}
				else
				{
					secondPermutation[firstCity] = firstPermutation[firstCity]; // lub wracamy do stanu poczatkowego
					secondPermutation[secondCity] = firstPermutation[secondCity]; // brak lepszej sciezki 
				}
				this->T *= this->constants[l]; // mnozenie razy wyznaczona stala
			}

			delete[] firstPermutation;
			delete[] secondPermutation;
			this->result = GetCounter(); // zwraca czas

			//cout << "Obliczona odleglosc: " << Solution << ' ';
			//cout << "Czas: " << this->result << endl;
			this->averageTime += this->result; // obecna iteracja
			this->averageResult += Solution; // dodawanie czasow

			if (Solution > worstResult) // sprawdzam czy slabsze od najgorsego 
				worstResult = Solution;
			if (Solution < bestResult) // lepsze od najlepszego
				bestResult = Solution;
		}
		averageTime = averageTime / counter;
		averageResult = averageResult / counter;
		cout << "Stala: ";
		cout << this->constants[l] << endl;
		cout << "Sredni czas: ";
		cout << averageTime << endl;
		cout << "Najkrotsza odleglosc: ";
		cout << bestResult << endl;
		cout << "Najgorsza odleglosc: ";
		cout << worstResult <<  endl;
		cout << "Srednia odleglosc: ";
		cout << (int)averageResult << endl;
		cout << "Koncowa temperatura: " ;
		cout << this->T / this->constants[l] << endl << endl << endl;
		this->averageResult = 0.0;
		this->averageTime = 0.0;
		this->bestResult = pow(2, 54);
		this->worstResult = 0.0;
	}
}