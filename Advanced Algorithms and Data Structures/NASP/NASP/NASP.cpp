// NASP.cpp : Defines the entry point for the console application.
//

#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include "cvor.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>

using namespace std;

class AVL_stablo {
public:
	cvor* glava;

	AVL_stablo() {
		glava = NULL;
	}

	void dodaj(int element) {
		glava = dodaj_cvor(glava, element);
	}

	int dubina(cvor* cv) {
		if (cv == NULL) return 0;
		else return cv->dubina_cvora;
	}

	void preorder_ispis(cvor* cv) {
		if (cv == NULL) return;
		printf("%d ", cv->vrijednost);
		preorder_ispis(cv->lijevo_dijete);
		preorder_ispis(cv->desno_dijete);
	}

	void inorder_ispis(cvor* cv) {
		if (cv == NULL) return;
		inorder_ispis(cv->lijevo_dijete);
		printf("%d ", cv->vrijednost);
		inorder_ispis(cv->desno_dijete);
	}

	void ispis(cvor* cv) {
		if (cv->lijevo_dijete == NULL && cv->desno_dijete == NULL) return;
		printf("\nCvor : %d", cv->vrijednost);
		if(cv->lijevo_dijete != NULL) printf(", Lijevo dijete : %d", cv->lijevo_dijete->vrijednost);
		if(cv->desno_dijete != NULL) printf(", Desno dijete : %d", cv->desno_dijete->vrijednost);
		if(cv->lijevo_dijete == NULL || cv->desno_dijete == NULL) return;
		ispis(cv->lijevo_dijete);
		ispis(cv->desno_dijete);
	}

	cvor* dodaj_cvor(cvor* cv, int broj) {
		if (cv == NULL) cv = new cvor(broj);
		else if (broj > cv->vrijednost) {
			cv->desno_dijete = dodaj_cvor(cv->desno_dijete, broj);
			int FR = dubina(cv->desno_dijete) - dubina(cv->lijevo_dijete);
			if (FR == 2) {
				if (broj > cv->desno_dijete->vrijednost) {
					cv = lijeva_rotacija(cv);
				}
				else {
					cv->desno_dijete = desna_rotacija(cv->desno_dijete);
					cv = lijeva_rotacija(cv);
				}
			}
		}
		else if (broj < cv->vrijednost) {
			cv->lijevo_dijete = dodaj_cvor(cv->lijevo_dijete, broj);
			int FR = dubina(cv->lijevo_dijete) - dubina(cv->desno_dijete);
			if (FR == 2) {
				if (broj <= cv->lijevo_dijete->vrijednost) {
					cv = desna_rotacija(cv);
				}
				else {
					cv->lijevo_dijete = lijeva_rotacija(cv->lijevo_dijete);
					cv = desna_rotacija(cv);
				}
			}
		}
		cv->dubina_cvora = max(dubina(cv->lijevo_dijete), dubina(cv->desno_dijete)) + 1;
		return cv;
	}

	cvor* lijeva_rotacija(cvor* cv) {
		cvor* desno_dijete = cv->desno_dijete;
		cv->desno_dijete = desno_dijete->lijevo_dijete;

		if (desno_dijete != NULL) {
			desno_dijete->lijevo_dijete = cv;
			desno_dijete->dubina_cvora = max(dubina(desno_dijete->desno_dijete), dubina(cv)) + 1;
		}
		cv->dubina_cvora = max(dubina(cv->lijevo_dijete), dubina(cv->desno_dijete)) + 1;

		if (desno_dijete == NULL) return cv;
		return desno_dijete;
	}

	cvor* desna_rotacija(cvor* cv) { // cv = B
		cvor* lijevo_dijete = cv->lijevo_dijete; // C
		cv->lijevo_dijete = lijevo_dijete->desno_dijete; // B(lijevo) pokazuj na d

		if (lijevo_dijete != NULL) {
			lijevo_dijete->desno_dijete = cv; // C(desno) pokazuj na B
			lijevo_dijete->dubina_cvora = max(dubina(lijevo_dijete->lijevo_dijete), dubina(cv)) + 1;
		}
		cv->dubina_cvora = max(dubina(cv->lijevo_dijete), dubina(cv->desno_dijete)) + 1;
		if (lijevo_dijete == NULL) return cv;
		return lijevo_dijete; // C
	}
};

int main(int argc, char* argv[]) {
	ifstream datoteka;
	string ulaz;
	vector<int> brojevi;

	// ucitavanje datoteke
	datoteka.open(argv[1], fstream::in);
	while (!datoteka.eof()) {
		getline(datoteka, ulaz, ' ');
		brojevi.push_back(atoi(ulaz.c_str()));
	}

	AVL_stablo* stablo = new AVL_stablo();
	stablo->glava = new cvor(brojevi[0]);

	for (vector<int>::iterator i = brojevi.begin() + 1; i != brojevi.end(); i++)
		stablo->dodaj(*i);

	printf("\nPreorder ispis : ");
	stablo->preorder_ispis(stablo->glava);
	printf("\nInorder ispis : ");
	stablo->inorder_ispis(stablo->glava);
	printf("\nIspis : ");
	stablo->ispis(stablo->glava);

	while (true) {
		string pom;
		printf("\n\nUpisi vrijednost novog cvora ili \"x\" za izlaz iz programa : ");
		cin >> pom;
		if (pom[0] == 'x') break;
		int novi = atoi(pom.c_str());
		stablo->dodaj(novi);

		printf("\nPreorder ispis : ");
		stablo->preorder_ispis(stablo->glava);
		printf("\nInorder ispis : ");
		stablo->inorder_ispis(stablo->glava);
		printf("\nIspis : ");
		stablo->ispis(stablo->glava);
	}
    return 0;
}