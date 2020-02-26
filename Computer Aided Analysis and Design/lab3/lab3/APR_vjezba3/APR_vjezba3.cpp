// APR_vjezba3.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "funkcija.h"
#include "ogranicenja.h"
#include <stdio.h>
#include <math.h>
#include <cstdlib>
#include <ctime>
#include <cstdarg>

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

double fja_unutrasnja_tocka(double start, vector<double> x, funkcijaAps &fja, vector<int> ogranicenja) {
	
	for (int j = 0; j < ogranicenja.size(); j++) {
		if (ogranicenja[j] == 1) if (fja.o1(x) < 0) start -= fja.o1(x); // ako ne zadovoljava
		if (ogranicenja[j] == 2) if (fja.o2(x) < 0) start -= fja.o2(x);
		if (ogranicenja[j] == 3) if (fja.o3(x) < 0) start -= fja.o3(x);
		if (ogranicenja[j] == 4) if (fja.o4(x) < 0) start -= fja.o4(x);
	}
	return start;
}

// za trazenje unutrasnje tocke
vector<double> istrazi1(vector<double> xP, vector<double> Dx, funkcijaAps &fja, vector<int> ogranicenja) {
	vector<double> x = xP;
	int n = xP.size();
	for (int i = 0; i < n; i++) {
		// NOVO
		double P = 0;
		P = fja_unutrasnja_tocka(P, x, fja, ogranicenja);
		// NOVO

		x[i] += Dx[i]; // povecamo za Dx

		// NOVO
		double N = 0;
		N = fja_unutrasnja_tocka(N, x, fja, ogranicenja);
		// NOVO

		if (N > P) { // ne valja pozitivni pomak
			x[i] -= 2 * Dx[i]; // smanjimo za Dx

			N = 0;
			N = fja_unutrasnja_tocka(N, x, fja, ogranicenja);
			
			if (N > P) { // ne valja ni negativni
				x[i] += Dx[i]; // vratimo na staro
			}
		}
	}
	return x;
}

// za trazenje unutrasnje tocke
vector<double> hooke_jeeves1(vector<double> x0, vector<double> Dx, vector<double> e, funkcijaAps &fja, vector<int> ogranicenja) {
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
		xN = istrazi1(xP, Dx, fja, ogranicenja);
		for (int i = 0; i < n; i++) printf("%7.3f ", xN[i]);

		double fjaxN = 0;
		fjaxN = fja_unutrasnja_tocka(fjaxN, xN, fja, ogranicenja);
		
		double fjaxB = 0;
		fjaxB = fja_unutrasnja_tocka(fjaxB, xB, fja, ogranicenja);
		
		printf("%7.3f ", fjaxN);
		printf("%7.3f ", fjaxB);
		printf("\n");
		if (fjaxN < fjaxB) {
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

double fja_U(double start, vector<double> x, double t, funkcijaAps &fja, vector<int> ogranicenja) {
	for (int j = 0; j < ogranicenja.size(); j++) {
		if (ogranicenja[j] == 1) if (fja.o1(x) <= 0) start -= (1. / t) * numeric_limits<int>::infinity();
		else start -= (1. / t)*log(fja.o1(x));
		if (ogranicenja[j] == 2) if (fja.o2(x) <= 0) start -= (1. / t) * numeric_limits<int>::infinity();
		else start -= (1. / t)*log(fja.o2(x));
		if (ogranicenja[j] == 3) if (fja.o3(x) <= 0) start -= (1. / t) * numeric_limits<int>::infinity();
		else start -= (1. / t)*log(fja.o3(x));
		if (ogranicenja[j] == 4) if (fja.o4(x) <= 0) start -= (1. / t) * numeric_limits<int>::infinity();
		else start -= (1. / t)*log(fja.o4(x));
		if (ogranicenja[j] == 5) start += t*pow(fja.o5(x), 2);
	}
	return start;
}

// za trazenje minimuma sa ogranicenjima
vector<double> istrazi2(vector<double> xP, vector<double> Dx, funkcijaAps &fja, double t, vector<int> ogranicenja) {
	vector<double> x = xP;
	int n = xP.size();
	for (int i = 0; i < n; i++) {
		// NOVO 
		double P = fja.f(x);
		P = fja_U(P, x, t, fja, ogranicenja);
		// NOVO

		x[i] += Dx[i]; // povecamo za Dx

		// NOVO
		double N = fja.f(x);
		N = fja_U(N, x, t, fja, ogranicenja);
		// NOVO

		if (N > P) { // ne valja pozitivni pomak
			x[i] -= 2 * Dx[i]; // smanjimo za Dx
			
			// NOVO
			N = fja.f(x);
			N = fja_U(N, x, t, fja, ogranicenja);
			// NOVO

			if (N > P) { // ne valja ni negativni
				x[i] += Dx[i]; // vratimo na staro
			}
		}
	}
	return x;
}

// za trazenje minimuma sa ogranicenjima
vector<double> hooke_jeeves2(vector<double> x0, vector<double> Dx, vector<double> e, funkcijaAps &fja, double t, vector<int> ogranicenja) {
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
		xN = istrazi2(xP, Dx, fja, t, ogranicenja);
		for (int i = 0; i < n; i++) printf("%7.3f ", xN[i]);
		double fjaxN = fja.f(xN);
		fjaxN = fja_U(fjaxN, xN, t, fja, ogranicenja);
		
		double fjaxB = fja.f(xB);
		fjaxB = fja_U(fjaxB, xB, t, fja, ogranicenja);
		
		printf("%7.3f ", fjaxN);
		printf("%7.3f ", fjaxB);
		printf("\n");
		if (fjaxN < fjaxB) {
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

vector<double> gradijentni_spust(vector<double> x0, double e, double h, funkcijaAps &fja, bool optimal) {
	vector<double> x = x0;
	vector<double> gradijent(x0.size());
	int iter = 0;
	double fjaCilja = fja.f(x);
	double fjaCiljaMin = fjaCilja;

	do {
		gradijent = fja.g(x);
		//printf("\nx1=%f, x2=%f, f=%f, grad(x1)=%f grad(x2)=%f", x[0], x[1], fjaCilja, gradijent[0], gradijent[1]);
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

vector<double> newton_raphson(vector<double> x0, double e, double h, funkcijaAps &fja, bool optimal) {
	vector<double> x = x0;
	vector<double> gradijent(x0.size());
	Matrica H = Matrica(x0.size(), x0.size(), 0);
	Matrica grad = Matrica(x0.size(), 1, 0);
	vector<double> dX(x0.size());
	int iter = 0;
	double fjaCilja = fja.f(x);
	double fjaCiljaMin = fjaCilja;

	do {
		gradijent = fja.g(x); // vektor
		grad = Matrica(x0.size(), 1, gradijent); // matrica
		grad = (-1) * grad;
		H = fja.hess(x); // matrica

		// LUP
		Matrica P = H.LUP();
		H = zamjenaRedaka(H, P);
		grad = zamjenaRedaka(grad, P);
		Matrica L = H.getL();
		Matrica U = H.getU();
		Matrica y = L.sUnaprijed(grad);
		Matrica deltaX = U.sUnatrag(y); // matrica
		for (int i = 0; i < deltaX.getBrRedaka(); i++) {
			for (int j = 0; j < deltaX.getBrStupaca(); j++) {
				dX[i] = deltaX(i, j);
			}
		}

		//printf("\nBroj poziva fje :%d", fja.getBrF());
		//printf("\nx1=%f, x2=%f, f=%f, grad(x1)=%f grad(x2)=%f dX[0]=%f dX[1]=%f", x[0], x[1], fjaCilja, gradijent[0], gradijent[1], dX[0], dX[1]);
		double norma = 0;
		for (int i = 0; i < dX.size(); i++)
			norma += pow(dX[i], 2); // sad se gleda dX
		norma = sqrt(norma);
		if (norma < e) goto izlaz; // ako je norma dX-a <= epsilon, izlaz
		if (!optimal) {
			for (int i = 0; i < x0.size(); i++) {
				x[i] += dX[i];
			}
		}
		else {
			double l, r;
			for (int i = 0; i < dX.size(); i++) dX[i] *= -1;
			funkcijaOmot fomot(fja, x, dX); // x je coef
			double coef = zlatni_rez_tocka(h, 1, &l, &r, e, fomot);
			for (int i = 0; i < dX.size(); i++) dX[i] *= -1;
			for (int i = 0; i < x0.size(); i++) {
				x[i] += coef*dX[i];
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

vector<double> box(vector<double> x0, double e, double alfa, funkcijaAps &fja) {
	if (x0[0] < -100 || x0[0] > 100 || x0[1] < -100 || x0[1] > 100) {
		printf("\nPocetna tocka ne zadovoljava eksplicitna ogranicenja.\n");
		exit(1);
	}
	if ((x0[1] - x0[0]) < 0 || (2 - x0[0]) < 0) {
		printf("\nPocetna tocka ne zadovoljava implicitna ogranicenja.\n");
		exit(1);
	}
	vector<double> xC = x0;
	vector<vector<double>> tocke((2 * x0.size()), vector<double>(x0.size()));
	tocke[0] = xC;
	double max;
	int h;
	double kriterij;
	int brTocaka = 1;

	srand(time(NULL));
	for (int t = 1; t < 2 * x0.size(); t++) { // generiranje skupa 2n tocaka
		for (int i = 0; i < x0.size(); i++) {
			double R = ((double)rand() / (RAND_MAX));
			tocke[t][i] = -100 + R*(100 + 100); // nova tocka je unutar eksplicitnih ogr.
		}
		brTocaka++;
		while ((tocke[t][1] - tocke[t][0]) < 0 || (2 - tocke[t][0]) < 0) { // nisu zadovoljena implicitna ogr.
			for (int i = 0; i < x0.size(); i++) {
				tocke[t][i] = 0.5*(tocke[t][i] + xC[i]); // pomakni prema centroidu prihvacenih tocaka
			}
		}
		
		// izracunaj novi centroid (sa novom prihvacenom tockom)

		// odredi indeks h, F(X[h])=max
		max = fja.f(tocke[0]); h = 0;
		for (int i = 0; i < brTocaka; i++) {
			if (fja.f(tocke[i]) > max) {
				max = fja.f(tocke[i]);
				h = i;
			}
		}
		// odredi centroid xC
		fill(xC.begin(), xC.end(), 0);
		for (int j = 0; j < x0.size(); j++) {
			for (int i = 0; i < brTocaka; i++) {
				if (i == h) continue;
				xC[j] += tocke[i][j];
			}
			xC[j] /= brTocaka; // treba bit brTocaka-1, al nebitno
		}
	}

	double Max, Max2;
	int H, H2, brIter = 0;
	do {
		brIter++;

		// odredi indekse h, h2 : F(X[h]) = max, F(X[h2]) = drugi najlosiji
		Max = fja.f(tocke[0]), Max2 = fja.f(tocke[0]);
		H = 0, H2 = 0;
		for (int i = 0; i < tocke.size(); i++) {
			if (fja.f(tocke[i]) > Max) {
				Max2 = Max; Max = fja.f(tocke[i]);
				H2 = H;	H = i;
			}
		}

		// izracunaj xC (bez X[h])
		fill(xC.begin(), xC.end(), 0);
		for (int j = 0; j < x0.size(); j++) {
			for (int i = 0; i < tocke.size(); i++) {
				if (i == H) continue;
				xC[j] += tocke[i][j];
			}
			xC[j] /= (tocke.size()-1);
		}

		// >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>ISPIS<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

		printf("\n\nTocke :\t");
		for (int t = 0; t < tocke.size(); t++) {
			for (int i = 0; i < x0.size(); i++) {
				printf("%f ", tocke[t][i]);
			}
			printf("\t");
		}
		printf("\nVrijednost f :\t");
		for (int t = 0; t < tocke.size(); t++) {
			printf("%f\t\t", fja.f(tocke[t]));
		}
		printf("\n");
		printf("\nH = %d, F(H) = %f", H, fja.f(tocke[H]));
		printf("\nxC = ");
		for (int i = 0; i < x0.size(); i++) { // centroid
			printf("%f ", xC[i]);
		}

		// refleksija
		vector<double> xR(x0.size());
		for (int i = 0; i < x0.size(); i++)
			xR[i] = (1 + alfa)*xC[i] - alfa*tocke[H][i];

		for (int i = 0; i < x0.size(); i++) {
			if (xR[i] < -100) xR[i] = -100; // pomicemo na granicu ekspl. ogranicenja
			else if (xR[i] > 100) xR[i] = 100;
		}

		while ((xR[1] - xR[0]) < 0 || (2 - xR[0]) < 0) { // provjeravamo implicitna ogr.
			for (int i = 0; i < x0.size(); i++) {
				xR[i] = 0.5*(xR[i] + xC[i]); // pomakni prema centroidu prihvacenih tocaka
			}
		}
		if (fja.f(xR) > fja.f(tocke[H2])) { // ako je to i dalje najlosija tocka
			for (int i = 0; i < x0.size(); i++) {
				xR[i] = 0.5*(xR[i] + xC[i]); // pomakni prema centroidu prihvacenih tocaka
			}
		}
		tocke[H] = xR;

		// lijeva strana kriterija zaustavljanja
		double sum = 0;
		for (int j = 0; j < tocke.size(); j++) {
			sum += pow((fja.f(tocke[j]) - fja.f(xC)), 2);
		}
		sum /= x0.size();
		kriterij = sqrt(sum);
	} while (kriterij > e && brIter < 2000);
	printf("\n\nBroj iteracija (max = 2000) = %d", brIter);
	return xC;
}

// postupak transformacije u problem bez ogranicenja na mjesovit nacin

vector<double> transform(vector<double> x0, double e, double t, funkcijaAps &fja, vector<int> ogranicenja) {
	
	vector<double> X0 = x0;
	// ucitaj funkcije ogranicenja
	// provjera je li tocka unutrasnja (zadovoljava li ogranicenja)

	bool neZadovoljava = false;
	for (int i = 0; i < ogranicenja.size(); i++) {
		if (ogranicenja[i] == 1) {
			if (fja.o1(x0) < 0) {
				neZadovoljava = true; break;
			}
		}
		else if (ogranicenja[i] == 2) {
			if (fja.o2(x0) < 0) {
				neZadovoljava = true; break;
			}
		}
		else if (ogranicenja[i] == 3) {
			if (fja.o3(x0) < 0) {
				neZadovoljava = true; break;
			}
		}
		else if (ogranicenja[i] == 4) {
			if (fja.o4(x0) < 0) {
				neZadovoljava = true; break;
			}
		}
	}

	// ako postoje ogranicenja koja ne zadovoljavaju, preko HJ izracunaj novu pocetnu tocku x0
	if (neZadovoljava) {
		vector<double> Dx;
		vector<double> ev;
		Dx.push_back(1); Dx.push_back(1);
		ev.push_back(e); ev.push_back(e);
		X0 = hooke_jeeves1(x0, Dx, ev, fja, ogranicenja);
		printf("\n\nPocetna tocka ne zadovoljava uvjete, nova pocetna tocka : ");
		for (int i = 0; i < 2; i++) {
			printf(" %f", X0[i]);
		}
		printf("\n");
	}

	bool uvjet = true;
	vector<double> xmin0;
	vector<double> xmin;
	vector<double> Dx;
	vector<double> ev;
	Dx.push_back(1); Dx.push_back(1);
	ev.push_back(e); ev.push_back(e);
	xmin0 = hooke_jeeves2(X0, Dx, ev, fja, t, ogranicenja);

	do {
		// iteracija
		t *= 10;
		xmin = hooke_jeeves2(xmin0, Dx, ev, fja, t, ogranicenja);
		for (int i = 0; i < x0.size(); i++) {
			if (fabs(xmin0[i]-xmin[i]) <= e) uvjet = false;
		}
		xmin0 = xmin;
	} while (uvjet);

	return xmin;
}

void zadatak1() {

	// POSTUPAK NAJBRZEG SPUSTA

	vector<double> x0;
	x0.push_back(0);
	x0.push_back(0);
	double e = 0.000001;
	double h = 1;
	int n = 2;
	vector<double> xmin;

	funkcija3 fja1;

	xmin = gradijentni_spust(x0, e, h, fja1, false); // false - nije optimalno

	printf("\nZADATAK 1 - GRADIJENTNI SPUST :");
	printf("\n\nKoordinate tocke minimuma : ");
	for (int i = 0; i < n; i++)
		printf("%f ", xmin[i]);
	printf("\nFunkcija cilja u tocki minimuma = %f", fja1.f(xmin));
	printf("\nBroj poziva funkcije : %d", fja1.getBrF());
	printf("\nBroj poziva racunanja gradijenta : %d\n", fja1.getBrG());

	funkcija3 fja2;

	xmin = gradijentni_spust(x0, e, h, fja2, true); // true - optimalno

	printf("\n\nZADATAK 1 - GRADIJENTNI SPUST UZ OPTIMALNI POMAK :");
	printf("\n\nKoordinate tocke minimuma : ");
	for (int i = 0; i < n; i++)
		printf("%f ", xmin[i]);
	printf("\nFunkcija cilja u tocki minimuma = %f", fja2.f(xmin));
	printf("\nBroj poziva funkcije : %d", fja2.getBrF());
	printf("\nBroj poziva racunanja gradijenta : %d\n", fja2.getBrG());
}

void zadatak2() {

	// POSTUPAK NAJBRZEG SPUSTA

	vector<double> x0_1;
	x0_1.push_back(-1.9);
	x0_1.push_back(2);
	double e = 0.000001;
	double h = 1;
	int n = 2;
	vector<double> xmin;

	funkcija1 fja1;

	xmin = gradijentni_spust(x0_1, e, h, fja1, true); // true - optimalno

	printf("\n\nZADATAK 2 - GRADIJENTNI SPUST funkcije 1 UZ OPTIMALNI POMAK :");
	printf("\n\nKoordinate tocke minimuma : ");
	for (int i = 0; i < n; i++)
		printf("%f ", xmin[i]);
	printf("\nFunkcija cilja u tocki minimuma = %f", fja1.f(xmin));
	printf("\nBroj poziva funkcije : %d", fja1.getBrF());
	printf("\nBroj poziva racunanja gradijenta : %d\n", fja1.getBrG());

	vector<double> x0_2;
	x0_2.push_back(0.1);
	x0_2.push_back(0.3);

	funkcija2 fja2;

	xmin = gradijentni_spust(x0_2, e, h, fja2, true); // true - optimalno

	printf("\n\nZADATAK 2 - GRADIJENTNI SPUST funkcije 2 UZ OPTIMALNI POMAK :");
	printf("\n\nKoordinate tocke minimuma : ");
	for (int i = 0; i < n; i++)
		printf("%f ", xmin[i]);
	printf("\nFunkcija cilja u tocki minimuma = %f", fja2.f(xmin));
	printf("\nBroj poziva funkcije : %d", fja2.getBrF());
	printf("\nBroj poziva racunanja gradijenta : %d\n", fja2.getBrG());

	// NEWTON - RAPHSONOV POSTUPAK
	vector<double> x0_3;
	x0_3.push_back(-1.9);
	x0_3.push_back(2);

	funkcija1 fja3;

	xmin = newton_raphson(x0_3, e, h, fja3, true); // true - optimalno

	printf("\n\nZADATAK 2 - NEWTON-RAPHSONOV POSTUPAK funkcije 1 UZ OPTIMALNI POMAK :");
	printf("\n\nKoordinate tocke minimuma : ");
	for (int i = 0; i < n; i++)
		printf("%f ", xmin[i]);
	printf("\nFunkcija cilja u tocki minimuma = %f", fja3.f(xmin));
	printf("\nBroj poziva funkcije : %d", fja3.getBrF());
	printf("\nBroj poziva racunanja gradijenta : %d", fja3.getBrG());
	printf("\nBroj poziva racunanja Hessove matrice : %d\n", fja3.getBrH());

	vector<double> x0_4;
	x0_4.push_back(0.1);
	x0_4.push_back(0.3);

	funkcija2 fja4;

	xmin = newton_raphson(x0_4, e, h, fja4, true); // true - optimalno

	printf("\n\nZADATAK 2 - NEWTON-RAPHSONOV POSTUPAK funkcije 2 UZ OPTIMALNI POMAK :");
	printf("\n\nKoordinate tocke minimuma : ");
	for (int i = 0; i < n; i++)
		printf("%f ", xmin[i]);
	printf("\nFunkcija cilja u tocki minimuma = %f", fja4.f(xmin));
	printf("\nBroj poziva funkcije : %d", fja4.getBrF());
	printf("\nBroj poziva racunanja gradijenta : %d", fja4.getBrG());
	printf("\nBroj poziva racunanja Hessove matrice : %d\n", fja4.getBrH());

}

void zadatak31() {

	// FUNKCIJA 1 - BOX
	vector<double> x0;
	x0.push_back(-1.9);
	x0.push_back(2);
	double e = 0.000001;
	double alfa = 1.3;
	vector<double> xmin;

	funkcija1 fja1;

	xmin = box(x0, e, alfa, fja1);
	printf("\n\nKoordinate tocke minimuma za funkciju 1 po Boxu : ");
	for (int i = 0; i < 2; i++)
		printf("%f ", xmin[i]);
}

void zadatak32() {

	// FUNKCIJA 2 - BOX
	vector<double> x0;
	x0.push_back(0.1);
	x0.push_back(0.3);
	double e = 0.000001;
	double alfa = 1.3;
	vector<double> xmin;

	funkcija2 fja1;

	xmin = box(x0, e, alfa, fja1);
	printf("\n\nKoordinate tocke minimuma za funkciju 2 po Boxu : ");
	for (int i = 0; i < 2; i++)
		printf("%f ", xmin[i]);
}

void zadatak41() {
	vector<double> x0;
	x0.push_back(-1.9);
	x0.push_back(2);
	double e = 0.000001;
	double t = 1;
	vector<double> xmin;

	funkcija1 fja1;

	vector<int> ogranicenja;
	ogranicenja.push_back(1);
	ogranicenja.push_back(2);

	xmin = transform(x0, e, t, fja1, ogranicenja);
	printf("\nPronadjena tocka minimuma funkcije 1 uz ogranicenja 1 i 2 : ");
	for (int i = 0; i < 2; i++)
		printf("%f ", xmin[i]);
	printf("\nBroj iteracija : %d", fja1.getBrF());
}

void zadatak42() {
	vector<double> x0;
	x0.push_back(0.1);
	x0.push_back(0.3);
	double e = 0.000001;
	double t = 1;
	vector<double> xmin;

	funkcija2 fja;

	vector<int> ogranicenja;
	ogranicenja.push_back(1);
	ogranicenja.push_back(2);

	xmin = transform(x0, e, t, fja, ogranicenja);
	printf("\nPronadjena tocka minimuma funkcije 1 uz ogranicenja 1 i 2 : ");
	for (int i = 0; i < 2; i++)
		printf("%f ", xmin[i]);
	printf("\nBroj iteracija : %d", fja.getBrF());
}

void zadatak5() {
	vector<double> x0;
	x0.push_back(5);
	x0.push_back(5);
	double e = 0.000001;
	double t = 1;
	vector<double> xmin;

	funkcija4 fja;

	vector<int> ogranicenja;
	ogranicenja.push_back(3);
	ogranicenja.push_back(4);
	ogranicenja.push_back(5);

	xmin = transform(x0, e, t, fja, ogranicenja);
	printf("\nPronadjena tocka minimuma : ");
	for (int i = 0; i < 2; i++)
		printf("%f ", xmin[i]);
	printf("\nBroj iteracija : %d", fja.getBrF());
}

int main() {
	zadatak1();
    return 0;
}
