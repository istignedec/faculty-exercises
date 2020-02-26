#pragma once

#include <stdio.h>
#include <math.h>
#include <vector>
#include <ctime>
#include <sstream>
#include <iterator>
#include <random>
#include "funkcija.h"
#include "kromosom.h"
#include "operatori.h"

using namespace std;

class genetskiAlgoritam {

public:
	int velicinaPopulacije;
	double pm; // vjerojatnost mutacije jednog bita
	bool binarni; // prikaz rjesenja koji ce se koristiti
	int p, k, N; // p - broj znamenki (preciznost) za binarni prikaz, k - za k-turnirski odabir, N - dimenzija (broj x-eva, ulaza funkcije)
	int limitEvaluacija;

	funkcijaAps *fja;
	const double epsilon = 0.000001;

	// konstruktor
	genetskiAlgoritam(funkcijaAps &fja, int velicinaPopulacije, bool binarni, double pm, int limitEvaluacija, int p, int k, int N) {
		this->fja = &fja;
		this->velicinaPopulacije = velicinaPopulacije;
		this->binarni = binarni;
		this->pm = pm;
		this->limitEvaluacija = limitEvaluacija;
		this->p = p;
		this->k = k;
		this->N = N;
	}

	kromosom * eliminacijskiGA() {

		// stvori populaciju
		vector<kromosom*> populacija = inicijalizirajPopulaciju(velicinaPopulacije, binarni, p, N);

		// evaluiraj populaciju (svakoj jedinki pridruzi dobrotu)
		for (int i = 0; i < populacija.size(); i++) {
			populacija[i]->postaviDobrotu(fja->f(populacija[i]->dohvatiTocke())); // dobrota je f
		}

		do {
			vector<kromosom*> novaPopulacija(velicinaPopulacije);
			kromosom *najbolji = nadjiNajbolji(populacija);
			if (najbolji->dohvatiDobrotu() <= epsilon) { // ako je dovoljno dobar, kraj
				break;
			}

			if (fja->getBr() % 1000 == 0) {
				printf("\nBr. poziva : %d, Dobrota najbolje jedinke : %f", fja->getBr(), najbolji->dohvatiDobrotu());
			}

			// k-turnirski eliminacijski odabir, vraca indekse roditelja i najgore jedinke
			vector<int> odabir = turnirskiOdabir(populacija, k);

			// nadji koji indeks pokazuje na najgoru jedinku u populaciji (najveca dobrota)
			int maxDobrota = odabir[0];
			for (int i = 0; i < k; i++) {
				if (populacija[odabir[i]]->dohvatiDobrotu() > populacija[maxDobrota]->dohvatiDobrotu())
					maxDobrota = odabir[i];
			}

			// nadji indekse roditelja
			int roditelj1 = -1;
			int roditelj2 = -1;
			for (int i = 0; i < k; i++) {
				if (odabir[i] != maxDobrota) {
					roditelj1 = odabir[i];
					break;
				}
			}	
			for (int j = 0; j < k; j++) {
				if (odabir[j] != maxDobrota && roditelj1 != odabir[j]) {
					roditelj2 = odabir[j];
					break;
				}	
			}

			// krizanje i mutacija
			kromosom* dijete;
			if (binarni) {
				dijete = uniformnoKrizanje(populacija[roditelj1], populacija[roditelj2], p);
				mutirajBinarni(dijete, pm);
			}
			else {
				dijete = aritmetickoKrizanje(populacija[roditelj1], populacija[roditelj2]);
				mutiraj(dijete, pm);
			}

			// postavi dobrotu djeteta (evaulacija nove jedinke)
			dijete->postaviDobrotu(fja->f(dijete->dohvatiTocke()));

			// stavi sve osim najgore u populaciju, umjesto najgore stavi novo dijete
			for (int i = 0; i < populacija.size(); i++) {
				if (i == maxDobrota) novaPopulacija[i] = dijete;
				else novaPopulacija[i] = populacija[i];
			}
			populacija = novaPopulacija;

		} while (fja->getBr() < limitEvaluacija);

		// nadjen minimum (ili ako je <= eps, ili je dosao limit evaluacija)
		printf("\nKRAJ");
		printf("\nBroj evaluacija funkcije cilja : %d", fja->getBr());

		return nadjiNajbolji(populacija);
	}
};