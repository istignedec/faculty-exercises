#include <stdio.h>
#include <math.h>
#include <vector>

using namespace std;

class funkcijaAps { // apstraktni razred za ciljne funkcije, prati broj poziva svake funkcije
	int brPozivaFje = 0;
	int brPozivaGrad = 0;

public:
	int getBrF() { return brPozivaFje; }
	void setBrF() { brPozivaFje++; }

	int getBrG() { return brPozivaGrad; }
	void setBrG() { brPozivaGrad++; }

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
	vector<double> g(vector<double> x) {
		vector<double> pom;

		for (int i = 0; i < xO.size(); i++) {
			pom.push_back(xO[i]);
			pom[i] -= x[0] * gradijentO[i];
		}
		return fjaO->g(pom);
	}
};

class funkcijaTestNediv :public funkcijaAps {
public:
	double f(vector<double> x) {
		setBrF();
		return 0.001*pow((x[0] - 4), 2) + 4 * pow((x[1] - 2), 2);
	}
	vector<double> g(vector<double> x) {
		setBrG();
		vector<double> gradijent(2);
		gradijent[0] = 0.001*(2 * x[0] - 8);
		gradijent[1] = 0.001*(8 * x[1] - 16);
		return gradijent;
	}
};