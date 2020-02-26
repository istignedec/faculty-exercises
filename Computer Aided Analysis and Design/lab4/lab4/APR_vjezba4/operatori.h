#pragma once

#include <stdio.h>
#include <math.h>
#include <vector>
#include <ctime>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <random>
#include "funkcija.h"
#include "kromosom.h"

using namespace std;

// vraca populaciju kromosoma
static vector<kromosom*> inicijalizirajPopulaciju(int velicinaPopulacije, bool binarni, int p, int N) {
	vector<kromosom*> populacija(velicinaPopulacije);
	for (int i = 0; i < velicinaPopulacije; i++) {
		if (binarni) {
			kromosomBinarni *kr = new kromosomBinarni(N, p);
			populacija[i] = kr;
		}
		else {
			kromosomDouble *kr = new kromosomDouble(N);
			populacija[i] = kr;
		}
	}
	return populacija;
}

// vraca najbolji kromosom u populaciji (najmanja fja dobrote)
static kromosom * nadjiNajbolji(vector<kromosom*> populacija) {
	double najmanjiError = -1;
	kromosom *najbolji = nullptr;

	for (int i = 0; i < populacija.size(); i++) {
		if (populacija[i]->dohvatiDobrotu() < najmanjiError || najmanjiError == -1) {
			najmanjiError = populacija[i]->dohvatiDobrotu();
			najbolji = populacija[i];
		}
	}
	return najbolji;
}

// odaberi slucajno k-jedinki i vrati indekse u populaciji
static vector<int> turnirskiOdabir(vector<kromosom*> populacija, int k) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, populacija.size() - 1);

	vector<int> odabir;

	while (odabir.size() != k) {
		int x = dis(gen);
		if (find(odabir.begin(), odabir.end(), x) == odabir.end()) { // ako odabir ne sadrzi x
			odabir.push_back(x);
		}
	}
	return odabir;
}

static kromosom * aritmetickoKrizanje(kromosom *roditelj1, kromosom *roditelj2) {
	kromosom *dijete = new kromosomDouble(roditelj1->dohvatiVelicinu());

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0, 1);
	for (int i = 0; i < roditelj1->dohvatiVelicinu(); i++) {
		double x1 = roditelj1->dohvatiVrijednost(i);
		double x2 = roditelj2->dohvatiVrijednost(i);
		double R = dis(gen);

		dijete->postaviVrijednost(i, R*x1 + (1 - R)*x2);
	}
	return dijete;
}

static kromosom * heuristickoKrizanje(kromosom *roditelj1, kromosom *roditelj2) {
	kromosom *dijete = new kromosomDouble(roditelj1->dohvatiVelicinu());

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0, 1);

	dekoder dec = dekoder();
	for (int i = 0; i < roditelj1->dohvatiVelicinu(); i++) {
		double x1 = roditelj1->dohvatiVrijednost(i);
		double x2 = roditelj2->dohvatiVrijednost(i);
		double R = dis(gen);

		if (roditelj2->dohvatiDobrotu() < roditelj1->dohvatiDobrotu()) {
			double vrijednost = R*(x2 - x1) + x2;
			if (vrijednost > dec.getHigh()) vrijednost = dec.getHigh();
			if (vrijednost < dec.getLow()) vrijednost = dec.getLow();
			dijete->postaviVrijednost(i, R*(x2 - x1) + x2);
		}
		else {
			double vrijednost = R*(x1 - x2) + x1;
			if (vrijednost > dec.getHigh()) vrijednost = dec.getHigh();
			if (vrijednost < dec.getLow()) vrijednost = dec.getLow();
			dijete->postaviVrijednost(i, R*(x1 - x2) + x1);
		}
	}
	return dijete;
}

static kromosom * uniformnoKrizanje(kromosom *roditelj1, kromosom *roditelj2, int p) {
	kromosom *dijete = new kromosomBinarni(roditelj1->dohvatiVelicinu(), p);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0, 1);

	for (int i = 0; i < roditelj1->dohvatiVelicinu(); i++) {
		vector<int> r1 = roditelj1->dohvatiBinarniOblik(i); // dohvaca jednu tocku (imamo x1 i x2, a dohvaca x1)
		vector<int> r2 = roditelj2->dohvatiBinarniOblik(i);
		vector<int> d(r1.size());

		for (int i = 0; i < r1.size(); i++)
			d[i] = dis(gen) < 0.5 ? r1[i] : r2[i];

		// iz vektora bitova u int, pa u double (jer postaviVrijednost prima double vrijednost)

		ostringstream oss;
		if (! d.empty()) {
			copy(d.begin(), d.end(), ostream_iterator<int>(oss, ""));
		}
		
		int x = stoi(oss.str().c_str(), nullptr, 2); // pretvara iz bin u dec
		dekoder dec = dekoder();
		double vrijednost = dec.uDouble(x, d.size());
		if (vrijednost > dec.getHigh()) vrijednost = dec.getHigh();
		if (vrijednost < dec.getLow()) vrijednost = dec.getLow();
		dijete->postaviVrijednost(i, vrijednost);
	}
	return dijete;
}

static void mutiraj(kromosom *dijete, double pm) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::normal_distribution<> disGauss(0, 1);
	std::uniform_real_distribution<> dis(0, 1);


	dekoder dec = dekoder();
	for (int i = 0; i < dijete->dohvatiVelicinu(); i++) {
		double R = dis(gen);
		if (R <= pm) {
			double vrijednost = dijete->dohvatiVrijednost(i) + disGauss(gen);
			if (vrijednost > dec.getHigh()) vrijednost = dec.getHigh();
			if (vrijednost < dec.getLow()) vrijednost = dec.getLow();
			dijete->postaviVrijednost(i, vrijednost);
		}
	}
}

static void mutirajBinarni(kromosom *dijete, double pm) { // pm je vjerojatnost mutacije jednog bita
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0, 1);

	dekoder dec = dekoder();
	for (int i = 0; i < dijete->dohvatiVelicinu(); i++) {
		double R = dis(gen);
		if (R <= pm) {
			vector<int> d = dijete->dohvatiBinarniOblik(i);
			for (int j = 0; j < d.size(); j++) {
				d[j] = 1 - d[j];
			}

			// iz vektora bitova u int, pa u double (jer postaviVrijednost prima double vrijednost)

			ostringstream oss;
			if (!d.empty()) {
				copy(d.begin(), d.end(), ostream_iterator<int>(oss, ""));
			}

			int x = stoi(oss.str().c_str(), nullptr, 2);
			dekoder dec = dekoder();
			double vrijednost = dec.uDouble(x, d.size());
			if (vrijednost > dec.getHigh()) vrijednost = dec.getHigh();
			if (vrijednost < dec.getLow()) vrijednost = dec.getLow();
			dijete->postaviVrijednost(i, vrijednost);
		}
	}
}