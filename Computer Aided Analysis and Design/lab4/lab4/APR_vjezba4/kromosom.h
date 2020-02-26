#pragma once

#include <stdio.h>
#include <math.h>
#include <vector>
#include <random>

using namespace std;

class kromosom {

public:
	virtual double dohvatiDobrotu() = 0;
	virtual void postaviDobrotu(double dobrota) = 0;
	virtual int dohvatiVelicinu() = 0;
	virtual double dohvatiVrijednost(int i) = 0;
	virtual void postaviVrijednost(int i, double vrijednost) = 0;
	virtual vector<double> dohvatiTocke() = 0;
	virtual vector<int> dohvatiBinarniOblik(int i) = 0;
};

class dekoder {

	int low = -50;
	int high = 150;

public:

	dekoder() {	}

	dekoder(int low, int high) {
		this->low = low;
		this->high = high;
	}

	int uBinarni(double x, int n) {
		int pom = (int)((x - low) / (high - low) * pow(2, n - 1));
		return pom;
	}

	double uDouble(int b, int n) {
		return low + b * (high - low) / pow(2, n - 1);
	}

	int getLow() {
		return this->low;
	}

	int getHigh() {
		return this->high;
	}
};

class kromosomDouble :public kromosom {

	double dobrota;
	vector<double> *tocke;

public:
	kromosomDouble(int N) { // N dimenzija u kromosomu (N tocaka)
		this->dobrota = 0;
		this->tocke = new vector<double>(N);

		random_device rd; // random double broj izmedju 0 i 1
		mt19937 gen(rd());
		uniform_real_distribution<> dis(0, 1);

		for (int i = 0; i < N; i++) {
			double R = dis(gen);

			dekoder d = dekoder();
			(*tocke)[i] = R*(d.getHigh() - d.getLow()) + d.getLow(); // slucajno generirani
		}
	}

	double dohvatiDobrotu() {
		return dobrota;
	}

	void postaviDobrotu(double dobrota) {
		this->dobrota = dobrota;
	}

	int dohvatiVelicinu() {
		return tocke->size();
	}

	double dohvatiVrijednost(int i) {
		return (*tocke)[i];
	}

	void postaviVrijednost(int i, double vrijednost) {
		(*tocke)[i] = vrijednost;
	}

	vector<double> dohvatiTocke() {
		return *tocke;
	}

	vector<int> dohvatiBinarniOblik(int i) {
		return vector<int>(); // vrati prazan vektor
	}
};

class kromosomBinarni :public kromosom {

	double dobrota;
	vector<int> *tocke;
	int N; // broj dimenzije
	int n; // broj bitova

public:
	kromosomBinarni(int N, int p) { // N-dimenzijski kromosom, p je preciznost, broj znamenki iza dec. tocke
		this->dobrota = 0;
		this->N = N;

		this->tocke = new vector<int>(N);

		dekoder d = dekoder();
		this->n = (int)(log(floor(1 + (d.getHigh() - d.getLow())*pow(10, p))) / log(2)); // broj bitova

		random_device rd; // random int broj izmedju 0 i 2^(n-1)
		mt19937 gen(rd());
		uniform_int_distribution<> dis(0, pow(2, n - 1));

		for (int i = 0; i < N; i++) {
			int R = dis(gen);
			
			(*tocke)[i] = R;
		}
	}

	double dohvatiDobrotu() {
		return dobrota;
	}

	void postaviDobrotu(double d) {
		this->dobrota = d;
	}

	int dohvatiVelicinu() {
		return tocke->size();
	}

	double dohvatiVrijednost(int i) {
		dekoder d = dekoder();
		return d.uDouble((*tocke)[i], n); // pretvori iz binarnog u double
	}

	void postaviVrijednost(int i, double vrijednost) {
		dekoder d = dekoder();
		(*tocke)[i] = d.uBinarni(vrijednost, n); // iz double u binarni
	}

	vector<double> dohvatiTocke() {
		dekoder d = dekoder();
		vector<double> *p = new vector<double>(N);
		for (int i = 0; i < (*tocke).size(); i++) {
			(*p)[i] = d.uDouble((*tocke)[i], n); // iz binarnog u double
		}
		return *p;
	}

	// funkcija koja pretvara decimalni broj "tocke[index]" u binarni vektor sa n bitova
	vector<int> dohvatiBinarniOblik(int index) {
		vector<int> v;
		for (int i = n - 1; i >= 0; i--) {
			int k = (*tocke)[index] >> i;
			if (k & 1)
				v.push_back(1);
			else
				v.push_back(0);
		}
		return v;
	}
};