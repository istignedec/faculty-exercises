#pragma once

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include "matrica.h"
#include "gnugraph/GnuGraph.h"

// trapezni postupak
void trapezni(Matrica A, Matrica B, Matrica x, double T, double tmax, int n) {
	Matrica U(A.getBrRedaka(), A.getBrStupaca(), 0);
	for (unsigned i = 0; i < U.getBrRedaka(); i++) {
		U(i, i) = 1;
	}

	Matrica inv = !(U - A*(T / 2));
	Matrica R = inv*(U + A*(T / 2));
	Matrica S = inv*B*(T / 2);

	Matrica xk = x;
	double t = 0;
	vector<double> tplot;
	vector<double> x1plot;
	vector<double> x2plot;

	printf("\n>t<");
	for (unsigned i = 0; i < xk.getBrRedaka(); i++)	printf("\t\t>x%d<", i + 1);

	int brIter = 0;
	while (t <= tmax) {
		// ispis na zaslon
		if (brIter % n == 0) {
			printf("\n%f", t);
			for (unsigned i = 0; i < xk.getBrRedaka(); i++)	printf("\t%f", xk(i, 0));
		}
		
		// priprema za plot
		tplot.push_back(t);
		x1plot.push_back(xk(0, 0));
		x2plot.push_back(xk(1, 0));

		// trapezni postupak
		xk = R*xk + S;
		t += T;
		brIter++;
	}

	// plot na ekran
	GnuGraph graph1("C:/Program Files/gnuplot/bin/gnuplot.exe");
	GnuGraph graph2("C:/Program Files/gnuplot/bin/gnuplot.exe");
	const string output1 = graph1.plot(tplot, x1plot, "x1");
	const string output2 = graph2.plot(tplot, x2plot, "x2");
	system("PAUSE");
}

// postupak Runge-Kutta 4. reda
void RK(Matrica A, Matrica B, Matrica x, double T, double tmax, int n) {
	Matrica xk = x;
	double t = 0;
	vector<double> tplot;
	vector<double> x1plot;
	vector<double> x2plot;

	printf("\n>t<");
	for (unsigned i = 0; i < xk.getBrRedaka(); i++)	printf("\t\t>x%d<", i + 1);

	int brIter = 0;
	while (t <= tmax) {
		// ispis na zaslon
		if (brIter % n == 0) {
			printf("\n%f", t);
			for (unsigned i = 0; i < xk.getBrRedaka(); i++)	printf("\t%f", xk(i, 0));
		}

		// priprema za plot
		tplot.push_back(t);
		x1plot.push_back(xk(0, 0));
		x2plot.push_back(xk(1, 0));

		// Runge-Kutta postupak 4. reda
		Matrica m1 = A*xk + B;
		Matrica m2 = A*(xk + (T / 2)*m1) + B;
		Matrica m3 = A*(xk + (T / 2)*m2) + B;
		Matrica m4 = A*(xk + T*m3) + B;
		xk = xk + (T / 6)*(m1 + 2 * m2 + 2 * m3 + m4);
		t += T;
		brIter++;
	}

	// plot na ekran
	GnuGraph graph1("C:/Program Files/gnuplot/bin/gnuplot.exe");
	GnuGraph graph2("C:/Program Files/gnuplot/bin/gnuplot.exe");
	const string output1 = graph1.plot(tplot, x1plot, "x1");
	const string output2 = graph2.plot(tplot, x2plot, "x2");
	system("PAUSE");
}