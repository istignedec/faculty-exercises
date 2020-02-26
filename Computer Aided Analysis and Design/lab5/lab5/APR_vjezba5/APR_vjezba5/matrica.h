// matrica.h
#pragma once

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

#define EPSILON 10e-6

using namespace std;

class Matrica {
private:
	unsigned m_brRedaka;
	unsigned m_brStupaca;
	vector<vector<double> > mat; // struktura podataka za matricu

public:
	// glavni konstruktor
	Matrica(unsigned brRedaka, unsigned brStupaca, double initial) { // initial - pocetno stanje celije
		m_brRedaka = brRedaka;
		m_brStupaca = brStupaca;

		// postavke velicine matrice
		setDim(brRedaka, brStupaca);
	}

	// konstruktor za ucitavanje iz datoteke
	Matrica(const char * datoteka) {

		ifstream file_A(datoteka); // ulazni stream za otvaranje datoteke matrice

		int brStupaca = 0;
		int brRedaka = 0;

		// ucitaj matricu
		string line_A;
		int idx = 0;
		double element_A;
		double *vector_A = nullptr;

		if (file_A.is_open() && file_A.good()) {
			while (getline(file_A, line_A)) {
				brRedaka += 1;
				stringstream stream_A(line_A);
				brStupaca = 0;
				while (1) {
					stream_A >> element_A;
					if (!stream_A)
						break;
					brStupaca += 1;
					double *tempArr = new double[idx + 1];
					copy(vector_A, vector_A + idx, tempArr);
					tempArr[idx] = element_A;
					vector_A = tempArr;
					idx += 1;
				}
			}
		}
		else {
			cout << "Datoteka " << datoteka << " ne postoji. \n";
		}

		int j;
		idx = 0;
		mat.resize(brRedaka);
		for (unsigned i = 0; i < mat.size(); i++) {
			mat[i].resize(brStupaca);
		}
		for (int i = 0; i < brRedaka; i++) {
			for (j = 0; j < brStupaca; j++) {
				this->mat[i][j] = vector_A[idx];
				idx++;
			}
		}
		m_brStupaca = brStupaca;
		m_brRedaka = brRedaka;
		delete[] vector_A;
	}

	// zbroj dvije matrice
	Matrica operator+(Matrica &B) {
		Matrica sum(m_brRedaka, m_brStupaca, 0.0);
		for (unsigned i = 0; i < m_brRedaka; i++) {
			for (unsigned j = 0; j < m_brStupaca; j++) {
				sum(i, j) = this->mat[i][j] + B(i, j);
			}
		}
		return sum;
	}

	// razlika dvije matrice
	Matrica operator-(Matrica & B) {
		Matrica diff(m_brRedaka, m_brStupaca, 0.0);
		for (unsigned i = 0; i < m_brRedaka; i++) {
			for (unsigned j = 0; j < m_brStupaca; j++) {
				diff(i, j) = this->mat[i][j] - B(i, j);
			}
		}
		return diff;
	}

	// umnozak dvije matrice
	Matrica operator*(Matrica & B) {
		Matrica multip(m_brRedaka, B.getBrStupaca(), 0.0);
		if (m_brStupaca == B.getBrRedaka()) { // provjera jel moguce mnozenje
			double temp = 0.0;
			for (unsigned i = 0; i < m_brRedaka; i++) {
				for (unsigned j = 0; j < B.getBrStupaca(); j++) {
					temp = 0.0;
					for (unsigned k = 0; k < m_brStupaca; k++) {
						temp += mat[i][k] * B(k, j);
					}
					multip(i, j) = temp;
				}
			}
			return multip;
		}
		else {
			return "Error";
		}
	}

	// pridruzivanje A=B
	Matrica& operator=(Matrica & B) {
		for (unsigned i = 0; i < B.getBrRedaka(); i++) {
			for (unsigned j = 0; j < B.getBrStupaca(); j++) {
				this->mat[i][j] = B(i, j);
			}
		}
		return *this;
	}

	// +=
	void operator+=(Matrica & B) {
		for (unsigned i = 0; i < m_brRedaka; i++) {
			for (unsigned j = 0; j < m_brStupaca; j++) {
				this->mat[i][j] += B(i, j);
			}
		}
	}

	// -=
	void operator-=(Matrica & B) {
		for (unsigned i = 0; i < m_brRedaka; i++) {
			for (unsigned j = 0; j < m_brStupaca; j++) {
				this->mat[i][j] -= B(i, j);
			}
		}
	}


	// vraca transponiranu matricu (~A)
	Matrica operator~() {
		Matrica Transpose(m_brRedaka, m_brStupaca, 0.0);
		for (unsigned i = 0; i < m_brStupaca; i++) {
			for (unsigned j = 0; j < m_brRedaka; j++) {
				Transpose(i, j) = this->mat[j][i];
			}
		}
		return Transpose;
	}

	// A==B
	bool operator==(Matrica & B) {
		// provjera jesu li jednake velicine
		if (this->mat.size() != B.getBrRedaka()) {
			return false;
		}
		for (unsigned i = 0; i < this->mat.size(); i++) {
			if (this->mat[i].size() != B.getBrStupaca()) {
				return false;
			}
		}

		// provjera jesu li sve vrijednosti jednake
		for (unsigned i = 0; i < B.getBrRedaka(); i++) {
			for (unsigned j = 0; j < B.getBrStupaca(); j++) {
				if (! jednakost(this->mat[i][j], B(i, j))) {
					return false;
				}
			}
		}
		return true;
	}

	// umnozak matrice i skalara (skalar desno)
	Matrica operator*(double scalar) {
		Matrica result(m_brRedaka, m_brStupaca, 0.0);
		for (unsigned i = 0; i < m_brRedaka; i++) {
			for (unsigned j = 0; j < m_brStupaca; j++) {
				result(i, j) = this->mat[i][j] * scalar;
			}
		}
		return result;
	}

	// umnozak skalara i matrice (skalar lijevo)
	friend Matrica operator*(double scalar, Matrica & A) {
		Matrica result(A.getBrRedaka(), A.getBrStupaca(), 0.0);
		for (unsigned i = 0; i < A.getBrRedaka(); i++) {
			for (unsigned j = 0; j < A.getBrStupaca(); j++) {
				result(i, j) = A(i, j) * scalar;
			}
		}
		return result;
	}

	// vraca vrijednost na lokaciji A(i, j)
	double& operator()(const unsigned &red, const unsigned & stupac) {
		return this->mat[red][stupac];
	}
	
	// ispis matrice na zaslon
	void print() const {
		for (unsigned i = 0; i < m_brRedaka; i++) {
			for (unsigned j = 0; j < m_brStupaca; j++) {
				cout << mat[i][j] << " ";
			}
			cout << endl;
		}
	}

	// ispis matrice u datoteku
	void printTo(const char * datoteka) {
		ofstream dat;
		dat.open(datoteka);
		for (unsigned i = 0; i < m_brRedaka; i++) {
			for (unsigned j = 0; j < m_brStupaca; j++) {
				dat << this->mat[i][j] << " ";
			}
			dat << endl;
		}
		dat.close();
	}

	// vraca broj redaka
	unsigned getBrRedaka() const {
		return this->m_brRedaka;
	}

	// vraca broj stupaca
	unsigned getBrStupaca() const {
		return this->m_brStupaca;
	}

	// promjena dimenzija
	void setDim(int brRedaka, int brStupaca) {
		mat.resize(brRedaka);
		for (unsigned i = 0; i < mat.size(); i++) {
			mat[i].resize(brStupaca, 0.0);
		}
	}

	// jednakost sa preciznosti EPSILON
	bool jednakost(double a, double b) {
		return fabs(a - b) < EPSILON;
	}

	// supstitucija unaprijed (L * y = b)
	Matrica sUnaprijed(Matrica b) {
		for (unsigned i = 0; i < b.getBrRedaka() - 1; i++) {
			for (unsigned j = i + 1; j < b.getBrRedaka(); j++) {
				b(j, 0) -= this->mat[j][i] * b(i, 0);
			}
		}
		return b; // y
	}

	// supstitucija unatrag (U * x = y)
	Matrica sUnatrag(Matrica y) {
		for (unsigned i = y.getBrRedaka() - 1; i >= 0; i--) {
			if (fabs(this->mat[i][i]) < EPSILON) {
				cout << "NADJEN JE PIVOT ELEMENT MANJI OD GRANICNE VRIJEDNOSTI \"EPSILON\", MOGUCE DA JE MATRICA SINGULARNA" << endl;
				//exit(1);
			}
			y(i, 0) /= this->mat[i][i];
			if (i == 0) {
				return y;
			}
			for (unsigned j = 0; j < i; j++) {
				y(j, 0) -= this->mat[j][i] * y(i, 0);
			}
		}
		return y; // x
	}

	// LU dekompozicija
	void LU() {
		for (unsigned i = 0; i < mat.size() - 1; i++) {
			for (unsigned j = i + 1; j < mat.size(); j++) {
				if (fabs(this->mat[i][i]) < EPSILON) {
					cout << "NADJEN JE PIVOT ELEMENT MANJI OD GRANICNE VRIJEDNOSTI \"EPSILON\", LU DEKOMPOZICIJA JE ZAUSTAVLJENA" << endl;
					cout << "KORISTI LUP!" << endl;
					//exit(1);
				}
				this->mat[j][i] /= this->mat[i][i];
				for (unsigned k = i + 1; k < mat.size(); k++) {
					this->mat[j][k] -= this->mat[j][i] * this->mat[i][k];
				}
			}
		}
	}

	// LUP dekompozicija
	Matrica LUP() {
		double pivot;
		Matrica P(this->mat.size(), 1, 0.0);
		for (unsigned i = 0; i < this->mat.size(); i++) { // postavi permutacijsku matricu
			P(i, 0) = i;
		}
		for (unsigned i = 0; i < this->mat.size() - 1; i++) {
			pivot = i;
			for (unsigned j = i + 1; j < this->mat.size(); j++) { // nadji najveci element stupca
				if (abs(this->mat[P(j, 0)][i]) > abs(this->mat[P(pivot, 0)][i])) {
					pivot = j;
				}
			}
			double pom;
			pom = P(i, 0);
			P(i, 0) = P(pivot, 0);
			P(pivot, 0) = pom;
			for (unsigned j = i + 1; j < this->mat.size(); j++) {
				if (fabs(this->mat[P(i, 0)][i]) < EPSILON) {
					cout << "NADJEN JE PIVOT ELEMENT MANJI OD GRANICNE VRIJEDNOSTI \"EPSILON\", LUP DEKOMPOZICIJA JE ZAUSTAVLJENA" << endl;
					//exit(1);
				}
				this->mat[P(j, 0)][i] /= this->mat[P(i, 0)][i];
				for (unsigned k = i + 1; k < this->mat.size(); k++) {
					this->mat[P(j, 0)][k] -= this->mat[P(j, 0)][i] * this->mat[P(i, 0)][k];
				}
			}
		}
		return P;
	}

	Matrica getL() {
		Matrica L(this->mat.size(), this->mat.size(), 0.0);
		for (unsigned i = 0; i < this->mat.size(); i++) {
			L(i, i) = 1;
		}
		for (unsigned i = 1; i < this->mat.size(); i++) {
			for (unsigned j = 0; j < i; j++) {
				L(i, j) = this->mat[i][j];
			}
		}
		return L;
	}

	Matrica getU() {
		Matrica U(this->mat.size(), this->mat.size(), 0.0);
		for (unsigned i = 0; i < this->mat.size(); i++) {
			for (unsigned j = i; j < this->mat.size(); j++) {
				U(i, j) = this->mat[i][j];
			}
		}
		return U;
	}

	void zamjenaRedaka(Matrica P) {
		Matrica pom = *this;
		for (unsigned i = 0; i < m_brRedaka; i++) {
			unsigned indexRetka = P(i, 0);
			for (unsigned j = 0; j < m_brStupaca; j++) {
				this->mat[i][j] = pom(indexRetka, j);
			}
		}
	}

	// vraca inverznu matricu (A^-1) - operator !A
	Matrica operator!() {
		Matrica Inverz(m_brRedaka, m_brStupaca, 0.0);
		Matrica P = LUP();

		cout << "LUP od matrice :" << endl;
		zamjenaRedaka(P);
		print();

		Matrica L = getL();
		Matrica U = getU();
		cout << "L :" << endl;
		L.print();
		cout << "U :" << endl;
		U.print();

		// idi po petlji (stavljaj u Inverz svaki stupac)
		for (unsigned n = 0; n < getBrRedaka(); n++) {
			Matrica b(getBrRedaka(), 1, 0);
			b(n, 0) = 1;
			b.zamjenaRedaka(P);
			Matrica y = L.sUnaprijed(b);
			Matrica x = U.sUnatrag(y);
			for (unsigned i = 0; i < x.getBrRedaka(); i++) {
				Inverz(i, n) = x(i, 0);
			}
		}
		return Inverz;
	}
};
