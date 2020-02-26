#pragma once

#include <stdio.h>
#include <math.h>
#include <vector>

using namespace std;

class funkcijaAps { // apstraktni razred za ciljne funkcije, prati broj poziva svake funkcije
	int brPoziva = 0;

public:
	int getBr() { return brPoziva; }
	void setBr() { brPoziva++; }

	double fd(double x) {
		vector<double> pom;
		pom.push_back(x);
		return f(pom);
	}
	double fd2(double x1, double x2) {
		vector<double> pom;
		pom.push_back(x1);
		pom.push_back(x2);
		return f(pom);
	}
	virtual double f(vector<double> x) = 0; // to je i dobrota ujedno
};

// razredi pojedine ciljne funkcije
class funkcija1 :public funkcijaAps {
public:
	double f(vector<double> x) {
		setBr();
		return 100 * pow((x[1] - x[0] * x[0]), 2) + pow((1 - x[0]), 2);
	}
};

class funkcija3 :public funkcijaAps {
public:
	double f(vector<double> x) {
		setBr();
		int N = x.size();
		double sum = 0;

		for (int i = 0; i < N; i++) {
			sum += pow((x[i] - i), 2);
		}
		return sum;
	}
};

class funkcija6 :public funkcijaAps {
public:
	double f(vector<double> x) {
		setBr();
		int N = x.size();
		double sum = 0;

		for (int i = 0; i < N; i++) {
			sum += pow(x[i], 2);
		}

		return 0.5 + (pow(sin(sqrt(sum)), 2) - 0.5) / (pow((1 + 0.001*sum), 2));
	}
};

class funkcija7 :public funkcijaAps {
public:
	double f(vector<double> x) {
		setBr();
		int N = x.size();
		double sum = 0;

		for (int i = 0; i < N; i++) {
			sum += pow(x[i], 2);
		}

		return pow(sum, 0.25)*(1 + pow(sin(50 * pow(sum, 0.1)), 2));
	}
};