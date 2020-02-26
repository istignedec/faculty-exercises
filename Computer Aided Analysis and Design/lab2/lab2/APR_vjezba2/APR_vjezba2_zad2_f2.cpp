// APR_vjezba2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "funkcija.h"
#include <stdio.h>
#include <math.h>
#include <vector>

using namespace std;

// trazi unimodalni interval za zadanu pocetnu tocku pretrazivanja i pomak h, l i r su granice intervala
void unimodalni(double h, double tocka, double *l, double *r, funkcijaAps &fja) {

	printf("\nUNIMODALNI INTERVAL - tocka = %f, pomak = %f : ", tocka, h);
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
	printf("[%f, %f]\n", *l, *r);
}

// za zadani unimodalni interval, nadji interval dobiven postupkom zlatnog reza
double zlatni_rez_unim(double *a, double *b, double e, funkcijaAps &fja) {

	int brojac = 0;
	printf("\nZLATNI REZ (e = %f) - INTERVAL [%f, %f] : ", e, *a, *b);
	printf("\n\n\ta\t\tc\t\td\t\tb\t\tfc\t\tfd");

	double c = *b - 0.5*(sqrt(5) - 1)*(*b - *a);
	double d = *a + 0.5*(sqrt(5) - 1)*(*b - *a);

	//funkcijaTest fja;
	double fc = fja.fd(c);
	double fd = fja.fd(d);

	while ((*b - *a) > e) {
		printf("\n%d\t%3f\t%3f\t%3f\t%3f\t%3f\t%3f", brojac, *a, c, d, *b, fc, fd);
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
	printf("\n%d\t%3f\t%3f\t%3f\t%3f\t%3f\t%3f\n", brojac, *a, c, d, *b, fc, fd);
	return (*a + *b) / 2;
}

// za zadanu pocetnu tocku i pomak h, nadji interval dobiven postupkom zlatnog reza
double zlatni_rez_tocka(double h, double tocka, double *l, double *r, double e, funkcijaAps &fja) {
	unimodalni(h, tocka, l, r, fja);
	return zlatni_rez_unim(l, r, e, fja);
}

vector<double> koordinatno_trazenje(vector<double> x0, vector<double> e, double h, funkcijaAps &fja) {
	vector<double> x = x0;
	vector<double> xs(x0.size());

	while (1) {
		xs = x;
		for (int i = 0; i < x0.size(); i++) { // za svaku dimenziju
			double l, r;
			funkcijaOmot fomot(fja, x, i); // fja jedne varijable

			x[i] = zlatni_rez_tocka(h, x[i], &l, &r, e[i], fomot);
		}
		for (int i = 0; i < x0.size(); i++) {
			if (fabs(x[i] - xs[i]) <= e[i]) goto izlaz;
		}
	}
izlaz:
	return x; // minimalni x
}

vector<double> simplex(vector<double> x0, double e, double pomak, double alfa, double beta, double gamma, double sigma, funkcijaAps &fja) {
	int n = x0.size();
	vector<vector<double>> tocke((n + 1), vector<double>(n)); // vanjski je broj tocaka (n+1), unutarnji je dimenzija tocke (n)

	for (int i = 0; i < n; i++) { // postavi prvu tocku
		tocke[0][i] = x0[i];
	}

	// izracun tocaka simpleksa pomakom po svakoj dimenziji iz pocetne tocke x0
	for (int i = 1; i <= n; i++) { // za svaku tocku (tocaka ima n+1), n i pocetna
		for (int j = 0; j < n; j++) // za svaku dimenziju (n)
			tocke[i][j] = tocke[0][j];
		tocke[i][i - 1] = tocke[0][i - 1] + pomak;
	}

	double min, max;
	int h, l;
	double kriterij;

	printf("\n\tCentroid\t\tVrijednost funkcije cilja\n");
	do {
		// odredi indekse h, l, F(X[h])=max, F(X[l])=min
		min = fja.f(tocke[0]);
		max = fja.f(tocke[0]);
		h = l = 0;
		for (int i = 0; i <= n; i++) {
			if (fja.f(tocke[i]) < min) {
				min = fja.f(tocke[i]);
				l = i;
			}
			if (fja.f(tocke[i]) > max) {
				max = fja.f(tocke[i]);
				h = i;
			}
		}

		// odredi centroid Xc
		vector<double> Xc(n);
		for (int j = 0; j < n; j++) {
			for (int i = 0; i <= n; i++) {
				if (i == h) continue;
				Xc[j] += tocke[i][j];
			}
			Xc[j] = Xc[j] * (double)1 / n;
		}
		for (int i = 0; i < n; i++) printf("  %f ", Xc[i]);
		printf("\t\t%f\n", fja.f(Xc));

		// refleksija
		vector<double> Xr(n);
		for (int i = 0; i < n; i++)
			Xr[i] = (1 + alfa)*Xc[i] - alfa*tocke[h][i];
		if (fja.f(Xr) < fja.f(tocke[l])) {
			// ekspanzija
			vector<double> Xe(n);
			for (int i = 0; i < n; i++)
				Xe[i] = (1 - gamma)*Xc[i] + gamma*Xr[i];
			if (fja.f(Xe) < fja.f(tocke[l]))
				tocke[h] = Xe;
			else
				tocke[h] = Xr;
		}
		else {
			bool izlaz = false;
			for (int j = 0; j <= n; j++) {
				if (j == h) continue;
				if (fja.f(Xr) <= fja.f(tocke[j])) {
					izlaz = true;
					break;
				}
			}
			if (!izlaz) {
				if (fja.f(Xr) < fja.f(tocke[h])) tocke[h] = Xr;

				// kontrakcija
				vector<double> Xk(n);
				for (int i = 0; i < n; i++)
					Xk[i] = (1 - beta)*Xc[i] + beta*tocke[h][i];
				if (fja.f(Xk) < fja.f(tocke[h])) tocke[h] = Xk;
				else {
					// pomakni sve tocke prema X[l] (min)
					for (int i = 0; i <= n; i++) {
						for (int j = 0; j < n; j++) {
							if (i == l) continue;
							tocke[i][j] += tocke[l][j];
							tocke[i][j] *= sigma;
						}
					}
				}
			}
			else tocke[h] = Xr;
		}

		// lijeva strana kriterija zaustavljanja
		double sum = 0;
		for (int j = 0; j <= n; j++) {
			sum += pow((fja.f(tocke[j]) - fja.f(Xc)), 2);
		}
		sum /= n;
		kriterij = sqrt(sum);
	} while (kriterij > e);
	return tocke[l]; // vraca tocku najblizu minimumu
}

vector<double> istrazi(vector<double> xP, vector<double> Dx, funkcijaAps &fja) {
	vector<double> x = xP;
	int n = xP.size();
	for (int i = 0; i < n; i++) {
		double P = fja.f(x);
		x[i] += Dx[i]; // povecamo za Dx
		double N = fja.f(x);

		if (N > P) { // ne valja pozitivni pomak
			x[i] -= 2 * Dx[i]; // smanjimo za Dx
			N = fja.f(x);
			if (N > P) { // ne valja ni negativni
				x[i] += Dx[i]; // vratimo na staro
			}
		}
	}
	return x;
}

vector<double> hooke_jeeves(vector<double> x0, vector<double> Dx, vector<double> e, funkcijaAps &fja) {
	int n = x0.size();
	vector<double> xB; // bazna tocka
	vector<double> xP; // pocetna tocka pretrazivanja
	vector<double> xN; // tocka dobivena pretrazivanjem

	bool uvjet = true; // Dx > e, kad to ne bude ispunjeno, izlaz
	xP = xB = x0;
	printf("\n\txB\t\txP\t\txN\tf(xN)\tf(xB)\n");
	for (int i = 0; i < n; i++) printf("%7.3f ", xB[i]);
	for (int i = 0; i < n; i++) printf("%7.3f ", xP[i]);
	do {
		xN = istrazi(xP, Dx, fja);
		for (int i = 0; i < n; i++) printf("%7.3f ", xN[i]);
		printf("%7.3f ", fja.f(xN));
		printf("%7.3f ", fja.f(xB));
		printf("\n");
		if (fja.f(xN) < fja.f(xB)) {
			for (int i = 0; i < n; i++) {
				xP[i] = 2 * xN[i] - xB[i];
				xB[i] = xN[i];
			}
		}
		else {
			for (int i = 0; i < n; i++) {
				Dx[i] /= 2;
				xP[i] = xB[i];
			}
		}
		for (int i = 0; i < n; i++) {
			if (Dx[i] <= e[i]) uvjet = false;
		}

		// ispis
		for (int i = 0; i < n; i++) printf("%7.3f ", xB[i]);
		for (int i = 0; i < n; i++) printf("%7.3f ", xP[i]);

	} while (uvjet);
	return xB;
}


int main() {

	// KOORDINATNO TRAŽENJE FUNKCIJA 2
	vector<double> x0;
	vector<double> e;
	
	double h = 0.1;
	int n = 2;
	x0.push_back(0.1); // pocetna tocka
	x0.push_back(0.3);
	e.push_back(0.0001);
	e.push_back(0.0001);

	funkcija2 fja21;
	vector<double> xmin;

	xmin = koordinatno_trazenje(x0, e, h, fja21);
	printf("\nKoordinate tocke minimuma FUNKCIJA 2, PO KOORDINATNIM OSIMA:\n");
	for (int i = 0; i < n; i++) {
		printf("%f ", xmin[i]);
	}
	
	// HOOKE-JEEVES POSTUPAK FUNKCIJA 2

	funkcija2 fja22;
	n = 2;
	vector<double> x0_3;
	x0_3.push_back(0.1); // pocetna tocka
	x0_3.push_back(0.3);
	vector<double> Dx;
	Dx.push_back(0.5);
	Dx.push_back(0.5);
	vector<double> e_3;
	e_3.push_back(0.000001);
	e_3.push_back(0.000001);

	vector<double> min;

	min = hooke_jeeves(x0_3, Dx, e_3, fja22);
	printf("\n\nPronadjena tocka minimuma FUNKCIJA 2, POSTUPAK HJ:\n");
	for (int i = 0; i < n; i++) {
		printf(" %f", min[i]);
	}
	printf("\n");

	// SIMPLEX POSTUPAK FUNKCIJA 2

	funkcija2 fja23;
	n = 2;
	vector<double> x0_4;
	x0_4.push_back(0.1); // pocetna tocka
	x0_4.push_back(0.3);
	double e4 = 0.000001, pomak = 0.01, alfa = 1, beta = 0.5, gamma = 2, sigma = 0.5;
	vector<double> min_4;

	min_4 = simplex(x0_4, e4, pomak, alfa, beta, gamma, sigma, fja23);
	printf("\n\nPronadjena tocka minimuma, FUNKCIJA 2 SIMPLEX :\n");
	for (int i = 0; i < n; i++) {
		printf(" %f", min_4[i]);
	}
	printf("\n");


	// POZIVI

	int poziv1f2 = fja21.getBr();
	int poziv2f2 = fja22.getBr();
	int poziv3f2 = fja23.getBr();

	printf("\nBroj poziva funkcije 2- PRETRAGA PO KOORD. OSIMA : %d", poziv1f2);
	printf("\nBroj poziva funkcije 2- HJ : %d", poziv2f2);
	printf("\nBroj poziva funkcije 2- SIMPLEX : %d", poziv3f2);
	printf("\n");

	return 0;
}