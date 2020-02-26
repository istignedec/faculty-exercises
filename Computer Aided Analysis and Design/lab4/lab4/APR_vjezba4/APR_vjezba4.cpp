// APR_vjezba4.cpp : ELIMINACIJSKI GENETSKI ALGORITAM

#include "stdafx.h"

#include <iostream>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <random>
#include "kromosom.h"
#include "funkcija.h"
#include "operatori.h"
#include "genetskiAlgoritam.h"


using namespace std;

int main() {
	
	funkcija3 fja;
	int velicinaPopulacije = 100;
	bool binarni = false;
	double pm = 0.3;
	int limitEvaluacija = 1000000;
	int p = 4; // preciznost za binarni prikaz
	int k = 3;
	int N = 5; // broj dimenzija funkcije

	genetskiAlgoritam ga1(fja, velicinaPopulacije, binarni, pm, limitEvaluacija, p, k, N);
	kromosom *kr = ga1.eliminacijskiGA();
	double dobrota = kr->dohvatiDobrotu();
	printf("\nNajbolja jedinka : ");
	for (int i = 0; i < kr->dohvatiVelicinu(); i++) {
		printf("%f ", kr->dohvatiVrijednost(i));
	}
	printf("\nDobrota najbolje jedinke : %f\n", dobrota);

	return 0;
}
