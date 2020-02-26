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
	virtual double f(vector<double> x) = 0;
};

// razredi pojedine ciljne funkcije
class funkcija1 :public funkcijaAps {
public:
	double f(vector<double> x) {
		setBr();
		return 100 * pow((x[1] - x[0] * x[0]), 2) + pow((1 - x[0]), 2);
	}
};

class funkcija2 :public funkcijaAps {
public:
	double f(vector<double> x) {
		setBr();
		return pow((x[0] - 4), 2) + 4 * pow((x[1] - 2), 2);
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

class funkcija4 :public funkcijaAps {
public:
	double f(vector<double> x) {
		setBr();
		return fabs((x[0] - x[1])*(x[0] + x[1])) + sqrt(x[0] * x[0] + x[1] * x[1]);
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

class funkcijaOmot :public funkcijaAps {
public:
	funkcijaAps *fjaO;
	vector<double> xO;
	int iO;
	funkcijaOmot(funkcijaAps &fja, vector<double> x, int i) {
		fjaO = &fja;
		xO = x;
		iO = i;
	};

	double f(vector<double> x) {
		xO[iO] = x[0];
		return fjaO->f(xO);
	}
};

class funkcijaTest :public funkcijaAps {
public:
	double f(vector<double> x) {
		setBr();
		return pow((x[0] - 4), 2);
	}
};

class funkcijaTestHJ :public funkcijaAps {
public:
	double f(vector<double> x) {
		setBr();
		return pow(x[0], 2) + 4 * pow(x[1], 2);
	}
};

class funkcijaTestSimplex :public funkcijaAps {
public:
	double f(vector<double> x) {
		setBr();
		return pow((x[0] - 1), 2) + pow((x[1] - 1), 2); // min ce bit u (1,1)
	}
};

class funkcijaZad1 :public funkcijaAps { // funkcija sa minimumom u toèki 3
public:
	double f(vector<double> x) {
		setBr();
		return pow((x[0] - 3), 2);
	}
};