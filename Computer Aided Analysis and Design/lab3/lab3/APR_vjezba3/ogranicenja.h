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

class ogranicenjaAps { // apstraktni razred za funkcije ogranicenja
public:
	virtual double g(vector<double> x) = 0;
};

class ogranicenje1 :public ogranicenjaAps {
public:
	double g(vector<double> x) {
		return x[1] - x[0];
	}
};

class ogranicenje2 :public ogranicenjaAps {
public:
	double g(vector<double> x) {
		return 2 - x[0];
	}
};

class ogranicenje3 :public ogranicenjaAps {
public:
	double g(vector<double> x) {
		return 3 - x[0] - x[1];
	}
};

class ogranicenje4 :public ogranicenjaAps {
public:
	double g(vector<double> x) {
		return 3 + 1.5*x[0] - x[1];
	}
};

class ogranicenje5 :public ogranicenjaAps {
public:
	double g(vector<double> x) {
		return x[1] - 1;
	}
};
