// APR_vjezba5.cpp

#include <vector>
#include "stdafx.h"
#include "matrica.h"
#include "postupci.h"
#include "gnugraph/GnuGraph.h"

void zadatak1() {
	Matrica A("zad1.txt");
	cout << "Matrica :" << endl;
	A.print();

	Matrica inverz = !A;
	cout << "Inverz :" << endl;
	inverz.print();
}

void zadatak2() {
	Matrica A("zad2.txt");
	cout << "Matrica :" << endl;
	A.print();

	Matrica inverz = !A;
	cout << "Inverz :" << endl;
	inverz.print();
}

void zadatak3() {
	Matrica A("A3.txt");
	cout << "Matrica A :" << endl;
	A.print();

	Matrica B("B3.txt");
	cout << "\nMatrica B :" << endl;
	B.print();

	Matrica x("x3.txt"); // pocetno stanje x(t=0)
	cout << "\nMatrica x :" << endl;
	x.print();

	double T, tmax;
	int n;
	string p;
	cout << "\nUpisite zeljeni korak integracije T : ";
	cin >> T;
	cout << "Upisite granicu vremenskog intervala : ";
	cin >> tmax;
	cout << "Upisite broj ucestalosti ispisa varijabli stanja : ";
	cin >> n;
	cout << "Upisite koji postupak se koristi (T ili RK) : ";
	cin >> p;

	if (p == "T") trapezni(A, B, x, T, tmax, n);
	else if (p == "RK") RK(A, B, x, T, tmax, n);
	else cout << "Greska!" << endl;
}

void zadatak4() {
	Matrica A("A4.txt");
	cout << "Matrica A :" << endl;
	A.print();

	Matrica B("B4.txt");
	cout << "\nMatrica B :" << endl;
	B.print();

	Matrica x("x4.txt"); // pocetno stanje x(t=0)
	cout << "\nMatrica x :" << endl;
	x.print();

	double T, tmax;
	int n;
	string p;
	cout << "\nUpisite zeljeni korak integracije T : ";
	cin >> T;
	cout << "Upisite granicu vremenskog intervala : ";
	cin >> tmax;
	cout << "Upisite broj ucestalosti ispisa varijabli stanja : ";
	cin >> n;
	cout << "Upisite koji postupak se koristi (T ili RK) : ";
	cin >> p;

	if (p == "T") trapezni(A, B, x, T, tmax, n);
	else if (p == "RK") RK(A, B, x, T, tmax, n);
	else cout << "Greska!" << endl;
}

int main() {
	
	zadatak3();
	return 0;
}
