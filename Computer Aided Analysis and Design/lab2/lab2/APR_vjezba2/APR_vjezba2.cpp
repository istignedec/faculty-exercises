// APR_vjezba2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "funkcija.h"
#include <stdio.h>
#include <math.h>
#include <vector>
#include <time.h>

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

	// ZLATNI REZ

	/*
	double a, b, h, tocka, e;
	int odabir;

	printf("Odaberi postupak zlatnog reza :\n");
	printf("1 - za zadanu tocku i pomak h\n");
	printf("2 - za zadani unimodalni interval\n");
	scanf_s("%d", &odabir);

	funkcijaTest fja;

	if (odabir == 1) {
	printf("\nUpisi (odvojeno razmakom) pomak h, pocetnu tocku i preciznost e :\n");
	scanf_s("%lf %lf %lf", &h, &tocka, &e);
	double x = zlatni_rez_tocka(h, tocka, &a, &b, e, fja);
	printf("\nInterval = [%f, %f], x = %f\n", a, b, x);
	}
	if (odabir == 2) {
	printf("\nUpisi (odvojeno razmakom) lijevu i desnu granicu intervala, te preciznost e :\n");
	scanf_s("%lf %lf %lf", &a, &b, &e);
	double x = zlatni_rez_unim(&a, &b, e, fja);
	printf("\nInterval = [%f, %f], x = %f\n", a, b, x);
	}

	*/
	// KOORDINATNO TRAŽENJE
	vector<double> x0;
	vector<double> e;
	double h;
	int n;

	funkcija6 fja;

	vector<double> xmin;

	printf("\nUpisi (odvojeno razmakom) broj dimenzije prostora i pomak h :\n");
	scanf_s("%d %lf", &n, &h);

	printf("\nUpisi koordinate pocetne tocke :\n");
	for (int i = 0; i < n; i++) {
		double xi;
		scanf_s("%lf", &xi);
		x0.push_back(xi);
	}

	printf("\nUpisi vektor granicne preciznosti :\n");
	for (int i = 0; i < n; i++) {
		double ei;
		scanf_s("%lf", &ei);
		e.push_back(ei);
	}

	xmin = koordinatno_trazenje(x0, e, h, fja);
	printf("\nKoordinate tocke minimuma :\n");
	for (int i = 0; i < n; i++) {
		printf("%f ", xmin[i]);
	}


	/*
	// HOOKE-JEEVES POSTUPAK

	funkcijaTestHJ fja;
	int n;
	vector<double> x0;
	vector<double> Dx;
	vector<double> e;

	vector<double> min;

	printf("\nUpisite broj dimenzija n :\n");
	scanf_s("%d", &n);

	double pom;
	printf("\nUpisite pocetnu tocku x0 :\n");
	for (int i = 0; i < n; i++) {
	scanf_s("%lf", &pom);
	x0.push_back(pom);
	}

	printf("\nUpisite vektor pomaka Dx :\n");
	for (int i = 0; i < n; i++) {
	scanf_s("%lf", &pom);
	Dx.push_back(pom);
	}

	printf("\nUpisite vektor granicne preciznosti :\n");
	for (int i = 0; i < n; i++) {
	scanf_s("%lf", &pom);
	e.push_back(pom);
	}

	min = hooke_jeeves(x0, Dx, e, fja);
	printf("\n\nPronadjena tocka minimuma :\n");
	for (int i = 0; i < n; i++) {
	printf(" %f", min[i]);
	}
	printf("\n");



	// SIMPLEX POSTUPAK

	funkcijaTestSimplex fja;
	int n;

	vector<double> x0;
	double e, pomak, alfa, beta, gamma, sigma;
	vector<double> min;

	printf("\nUpisite broj dimenzija n :\n");
	scanf_s("%d", &n);

	double pom;
	printf("\nUpisite pocetnu tocku x0 :\n");
	for (int i = 0; i < n; i++) {
	scanf_s("%lf", &pom);
	x0.push_back(pom);
	}

	printf("\nUpisite (odvojeno razmakom) preciznost e i pomak za generiranje simpleksa :\n");
	scanf_s("%lf %lf", &e, &pomak);

	printf("\nUpisite (odvojeno razmakom) alfa, beta, gamma i sigma :\n");
	scanf_s("%lf %lf %lf %lf", &alfa, &beta, &gamma, &sigma);

	min = simplex(x0, e, pomak, alfa, beta, gamma, sigma, fja);
	printf("\n\nPronadjena tocka minimuma :\n");
	for (int i = 0; i < n; i++) {
	printf(" %f", min[i]);
	}
	printf("\n");

	*/
	return 0;
}