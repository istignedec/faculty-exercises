#pragma once
#include <stdio.h>

using namespace std;

class cvor {
public:
	int vrijednost;
	int dubina_cvora;

	cvor* lijevo_dijete;
	cvor* desno_dijete;

	cvor(int broj) {
		vrijednost = broj;
		dubina_cvora = 0;
		lijevo_dijete = NULL;
		desno_dijete = NULL;
	}
};