#include <stdio.h>
#include <math.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <cmath>
#include "matrica.h"
#pragma once

using namespace std;

class funkcijaAps { // apstraktni razred za ciljne funkcije, prati broj poziva svake funkcije
	int brPozivaFje = 0;
	int brPozivaGrad = 0;
	int brPozivaHes = 0;

public:
	int getBrF() { return brPozivaFje; }
	void setBrF() { brPozivaFje++; }

	int getBrG() { return brPozivaGrad; }
	void setBrG() { brPozivaGrad++; }

	int getBrH() { return brPozivaHes; }
	void setBrH() { brPozivaHes++; }

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
	virtual double f(vector<double> x) = 0;
	virtual vector<double> g(vector<double> x) = 0;
	virtual Matrica hess(vector<double> x) = 0;
	virtual double o1(vector<double> x) = 0;
	virtual double o2(vector<double> x) = 0;
	virtual double o3(vector<double> x) = 0;
	virtual double o4(vector<double> x) = 0;
	virtual double o5(vector<double> x) = 0;
};

// razredi pojedine ciljne funkcije
class funkcija1 :public funkcijaAps {
public:
	double f(vector<double> x) {
		setBrF();
		return 100 * pow((x[1] - x[0] * x[0]), 2) + pow((1 - x[0]), 2);
	}
	vector<double> g(vector<double> x) {
		setBrG();
		vector<double> gradijent(2);
		gradijent[0] = 2 * (-1 + x[0] + 200 * pow(x[0], 3) - 200 * x[0] * x[1]); // gradijent po x1
		gradijent[1] = -200 * (pow(x[0], 2) - x[1]); // gradijent po x2
		return gradijent;
	}
	Matrica hess(vector<double> x) {
		setBrH();

		vector<double> vrijednosti;
		vrijednosti.push_back(400 * (3 * pow(x[0], 2) - x[1]) + 2);
		vrijednosti.push_back(-400 * (x[0]));
		vrijednosti.push_back(-400 * (x[0]));
		vrijednosti.push_back(200);

		Matrica H = Matrica(2, 2, vrijednosti);
		return H;
	}

	// ogranicenja
	double o1(vector<double> x) {
		return x[1] - x[0];
	}
	double o2(vector<double> x) {
		return 2 - x[0];
	}
	double o3(vector<double> x) {
		return 3 - x[0] - x[1];
	}
	double o4(vector<double> x) {
		return 3 + 1.5*x[0] - x[1];
	}
	double o5(vector<double> x) {
		return x[1] - 1;
	}
};

class funkcija2 :public funkcijaAps {
public:
	double f(vector<double> x) {
		setBrF();
		return pow((x[0] - 4), 2) + 4 * pow((x[1] - 2), 2);
	}
	vector<double> g(vector<double> x) {
		setBrG();
		vector<double> gradijent(2);
		gradijent[0] = 2 * x[0] - 8;
		gradijent[1] = 8 * x[1] - 16;
		return gradijent;
	}
	Matrica hess(vector<double> x) {
		setBrH();

		vector<double> vrijednosti;
		vrijednosti.push_back(2);
		vrijednosti.push_back(0);
		vrijednosti.push_back(0);
		vrijednosti.push_back(8);

		Matrica H = Matrica(2, 2, vrijednosti);
		return H;
	}
	// ogranicenja
	double o1(vector<double> x) {
		return x[1] - x[0];
	}
	double o2(vector<double> x) {
		return 2 - x[0];
	}
	double o3(vector<double> x) {
		return 3 - x[0] - x[1];
	}
	double o4(vector<double> x) {
		return 3 + 1.5*x[0] - x[1];
	}
	double o5(vector<double> x) {
		return x[1] - 1;
	}
};

class funkcija3 :public funkcijaAps {
public:
	double f(vector<double> x) {
		setBrF();
		return pow((x[0] - 2), 2) + pow((x[1] + 3), 2);
	}
	vector<double> g(vector<double> x) {
		setBrG();
		vector<double> gradijent(2);
		gradijent[0] = 2 * x[0] - 4;
		gradijent[1] = 2 * x[1] + 6;
		return gradijent;
	}
	Matrica hess(vector<double> x) {
		setBrH();

		vector<double> vrijednosti;
		vrijednosti.push_back(2);
		vrijednosti.push_back(0);
		vrijednosti.push_back(0);
		vrijednosti.push_back(2);

		Matrica H = Matrica(2, 2, vrijednosti);
		return H;
	}
	// ogranicenja
	double o1(vector<double> x) {
		return x[1] - x[0];
	}
	double o2(vector<double> x) {
		return 2 - x[0];
	}
	double o3(vector<double> x) {
		return 3 - x[0] - x[1];
	}
	double o4(vector<double> x) {
		return 3 + 1.5*x[0] - x[1];
	}
	double o5(vector<double> x) {
		return x[1] - 1;
	}
};

class funkcija4 :public funkcijaAps {
public:
	double f(vector<double> x) {
		setBrF();
		return pow((x[0] - 3), 2) + pow(x[1], 2);
	}
	vector<double> g(vector<double> x) {
		setBrG();
		vector<double> gradijent(2);
		gradijent[0] = 2 * x[0] - 6;
		gradijent[1] = 2 * x[1];
		return gradijent;
	}
	Matrica hess(vector<double> x) {
		setBrH();

		vector<double> vrijednosti;
		vrijednosti.push_back(2);
		vrijednosti.push_back(0);
		vrijednosti.push_back(0);
		vrijednosti.push_back(2);

		Matrica H = Matrica(2, 2, vrijednosti);
		return H;
	}
	// ogranicenja
	double o1(vector<double> x) {
		return x[1] - x[0];
	}
	double o2(vector<double> x) {
		return 2 - x[0];
	}
	double o3(vector<double> x) {
		return 3 - x[0] - x[1];
	}
	double o4(vector<double> x) {
		return 3 + 1.5*x[0] - x[1];
	}
	double o5(vector<double> x) {
		return x[1] - 1;
	}
};

class funkcijaOmot :public funkcijaAps {
public:
	funkcijaAps *fjaO;
	vector<double> xO;
	vector<double> gradijentO;
	funkcijaOmot(funkcijaAps &fja, vector<double> x, vector<double> gradijent) {
		fjaO = &fja;
		xO = x;
		gradijentO = gradijent;
	};

	double f(vector<double> coef) {
		vector<double> pom;

		for (int i = 0; i < xO.size(); i++) {
			pom.push_back(xO[i]);
			pom[i] -= coef[0] * gradijentO[i];
		}
		return fjaO->f(pom); // f(x+coef*grad)
	}

	vector<double> g(vector<double> coef) {
		vector<double> pom;

		for (int i = 0; i < xO.size(); i++) {
			pom.push_back(xO[i]);
			pom[i] -= coef[0] * gradijentO[i];
		}
		return fjaO->g(pom);
	}

	Matrica hess(vector<double> coef) {
		vector<double> pom;

		for (int i = 0; i < xO.size(); i++) {
			pom.push_back(xO[i]);
			pom[i] -= coef[0] * gradijentO[i];
		}
		return fjaO->hess(pom);
	}
	// ogranicenja
	double o1(vector<double> x) {
		return x[1] - x[0];
	}
	double o2(vector<double> x) {
		return 2 - x[0];
	}
	double o3(vector<double> x) {
		return 3 - x[0] - x[1];
	}
	double o4(vector<double> x) {
		return 3 + 1.5*x[0] - x[1];
	}
	double o5(vector<double> x) {
		return x[1] - 1;
	}
};