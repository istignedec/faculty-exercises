// APR_vjezba3.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "funkcija.h"
#include "matrica.h"
#include <stdio.h>
#include <math.h>

using namespace std;

// trazi unimodalni interval za zadanu pocetnu tocku pretrazivanja i pomak h, l i r su granice intervala
void unimodalni(double h, double tocka, double *l, double *r, funkcijaAps &fja) {

	//printf("\nUNIMODALNI INTERVAL - tocka = %f, pomak = %f : ", tocka, h);
	*l = tocka - h;
	*r = tocka + h;
	double m = tocka;
	double fl, fm, fr;
	unsigned step = 1;

	//funkcijaTest fja;
	fm = fja.fd(tocka);
	fl = fja.fd(*l);
	fr = fja.fd(*r);

	if (fm < fr && fm < fl) {
		return;
	}
	else if (fm > fr) {
		do {
			*l = m;
			m = *r;
			fm = fr;
			*r = tocka + h * (step *= 2);
			fr = fja.fd(*r);
		} while (fm > fr);
	}
	else {
		do {
			*r = m;
			m = *l;
			fm = fl;
			*l = tocka - h * (step *= 2);
			fl = fja.fd(*l);
		} while (fm > fl);
	}
	//printf("[%f, %f]\n", *l, *r);
}

// za zadani unimodalni interval, nadji interval dobiven postupkom zlatnog reza
double zlatni_rez_unim(double *a, double *b, double e, funkcijaAps &fja) {

	int brojac = 0;
	//printf("\nZLATNI REZ (e = %f) - INTERVAL [%f, %f] : ", e, *a, *b);
	//printf("\n\n\ta\t\tc\t\td\t\tb\t\tfc\t\tfd");

	double c = *b - 0.5*(sqrt(5) - 1)*(*b - *a);
	double d = *a + 0.5*(sqrt(5) - 1)*(*b - *a);

	//funkcijaTest fja;
	double fc = fja.fd(c);
	double fd = fja.fd(d);

	while ((*b - *a) > e) {
		//printf("\n%d\t%3f\t%3f\t%3f\t%3f\t%3f\t%3f", brojac, *a, c, d, *b, fc, fd);
		if (fc < fd) {
			*b = d;
			d = c;
			c = *b - 0.5*(sqrt(5) - 1)*(*b - *a);
			fd = fc;
			fc = fja.fd(c);
		}
		else {
			*a = c;
			c = d;
			d = *a + 0.5*(sqrt(5) - 1)*(*b - *a);
			fc = fd;
			fd = fja.fd(d);
		}
		brojac++;
	}
	//printf("\n%d\t%3f\t%3f\t%3f\t%3f\t%3f\t%3f\n", brojac, *a, c, d, *b, fc, fd);
	return (*a + *b) / 2;
}

// za zadanu pocetnu tocku i pomak h, nadji interval dobiven postupkom zlatnog reza
double zlatni_rez_tocka(double h, double tocka, double *l, double *r, double e, funkcijaAps &fja) {
	unimodalni(h, tocka, l, r, fja);
	return zlatni_rez_unim(l, r, e, fja);
}

vector<double> gradijentni_spust(vector<double> x0, double e, double h, funkcijaAps &fja, bool optimal) {
	vector<double> x = x0;
	vector<double> gradijent(x0.size());
	int iter = 0;
	double fjaCilja = fja.f(x);
	double fjaCiljaMin = fjaCilja;

	do {
		gradijent = fja.g(x);
		printf("\nx1=%f, x2=%f, f=%f, grad(x1)=%f grad(x2)=%f", x[0], x[1], fjaCilja, gradijent[0], gradijent[1]);
		double norma = 0;
		for (int i = 0; i < x0.size(); i++)
			norma += pow(gradijent[i], 2);
		norma = sqrt(norma);
		//for (int i = 0; i < x0.size(); i++)  // NORMIRANI GRADIJENT
		//	gradijent[i] /= norma;
		if (norma < e) goto izlaz; // ako je norma gradijenta <= epsilon, izlaz
		if (!optimal) {
			for (int i = 0; i < x0.size(); i++) {
				x[i] -= gradijent[i];
			}
		}
		else {
			double l, r;
			funkcijaOmot fomot(fja, x, gradijent); // x je coef
			double coef = zlatni_rez_tocka(h, 1, &l, &r, e, fomot);
			for (int i = 0; i < x0.size(); i++) {
				x[i] -= coef*gradijent[i];
			}
		}

		fjaCilja = fja.f(x);
		if (fjaCilja < fjaCiljaMin) {
			fjaCiljaMin = fjaCilja;
			iter = 0;
		}
		else iter++;
	} while (iter <= 100);
	printf("\nUpozorenje : DIVERGENCIJA");
izlaz:
	return x; // minimalni x
}

int main()
{
	// POSTUPAK NAJBRZEG SPUSTA

	vector<double> x0;
	double e;
	double h;
	int n;

	funkcija3 fja;

	vector<double> xmin;

	printf("\nUpisi (odvojeno razmakom) broj dimenzije prostora i pomak h :\n");
	scanf_s("%d %lf", &n, &h);

	printf("\nUpisi koordinate pocetne tocke :\n");
	for (int i = 0; i < n; i++) {
		double xi;
		scanf_s("%lf", &xi);
		x0.push_back(xi);
	}

	printf("\nUpisi vrijednost granicne preciznosti :\n");
	scanf_s("%lf", &e);

	xmin = gradijentni_spust(x0, e, h, fja, false); // true - optimalno, false - ne

	printf("\n\nKoordinate tocke minimuma : ");
	for (int i = 0; i < n; i++)
		printf("%f ", xmin[i]);
	printf("\nFunkcija cilja u tocki minimuma = %f", fja.f(xmin));
	printf("\nBroj poziva funkcije : %d\n", fja.getBrF());
	printf("\nBroj poziva racunanja gradijenta : %d\n", fja.getBrG());
    return 0;
}

