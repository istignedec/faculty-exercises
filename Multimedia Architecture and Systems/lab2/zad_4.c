#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define VELICINABLOKA 16
#define BROJBLOKOVA 32

typedef struct {
     char c;
} piksel;

typedef struct {
     int sirina;
     int visina;
     piksel *p;
} slika;

void ucitaj(slika *sl, FILE *dat) {
	
	int sirina = 0, visina = 0, max = 0;
	int komentar;
	char pom;
	
	if (dat == NULL) {
		printf("Datoteka neuspjesno otvorena.\n");
		return;
	}
	
	pom = getc(dat);
	if (pom != 'P' && (pom = getc(dat)) != '5') {
		printf("Datoteka nije P5.\n");
		return;
	}

	pom = getc(dat);
	pom = getc(dat);
	while (pom == '\n' && pom == '\t' && pom == ' ' && pom == '\r') {
		pom = getc(dat);
	}
	
	komentar = 0;
	while (pom == '#') {
		while (getc(dat) != '\n');
		pom = getc(dat);
		komentar++;
	}
	if (komentar != 0) {
		ungetc(pom, dat);
	}

	fscanf(dat, "%d %d", &sirina, &visina);
	while (pom == '\n' && pom == '\t' && pom == ' ' && pom == '\r') {
		pom = getc(dat);
	}

	fscanf(dat, "%d", &max);

	while (pom == '\n' && pom == '\t' && pom == ' ' && pom == '\r') {
		pom = getc(dat);
	}

	pom = getc(dat);
	
    sl->visina=(visina);
    sl->sirina=(sirina);
	sl->p = (piksel*)malloc(visina * sirina * sizeof(piksel));
	fread(sl->p, visina, sirina, dat);
}
	
int main(int argc, char *argv[]) {
	
	int brojBloka = atoi(argv[1]);
	
	FILE *dat1;
	FILE *dat2;
	
	slika *sl1;
	slika *sl2;
	
	int red1, stupac1, red2, stupac2, red, i, j, k, l, vr, pomak;
	int blok[VELICINABLOKA][VELICINABLOKA] = {0};
	int startX, startY, krajX, krajY, vX, vY;
	double mad, minMad;
	
	dat1 = fopen("lenna.pgm", "rb");
	dat2 = fopen("lenna1.pgm", "rb");
	
	/* UCITAVANJE SLIKA */
	
	sl1 = (slika*)malloc(sizeof(slika));
	sl2 = (slika*)malloc(sizeof(slika));
	
	ucitaj(sl1, dat1);
	ucitaj(sl2, dat2);
	
	/* TRAZENJE BLOKA */
	
	red1 = (int)floor(brojBloka / (sl2->sirina / VELICINABLOKA));
	stupac1 = brojBloka - red1 * (sl2->sirina / VELICINABLOKA);
	red2 = VELICINABLOKA * stupac1;
	stupac2 = VELICINABLOKA * red1;

	k = 0; red = 0;
    pomak = brojBloka / BROJBLOKOVA;
    vr = pomak * sl2->visina * VELICINABLOKA;
	
	for(i = 0; i < VELICINABLOKA; i++) {
		int indeks = (brojBloka % BROJBLOKOVA) * VELICINABLOKA + red + vr;
        for(j = indeks; j < indeks + VELICINABLOKA; j++) {
			blok[i][k++] = sl2->p[j].c;
        }
        red += sl2->sirina;
        k = 0;
    }

	/* GORNJI RUB */
	if (red2 - VELICINABLOKA < 0) startX = 0;
	else startX = red2 - VELICINABLOKA;

	/* DESNI RUB */
	if ((red2 + BROJBLOKOVA) > sl2->sirina) krajX = sl2->sirina - VELICINABLOKA;
	else krajX = red2 + VELICINABLOKA;

	/* LIJEVI RUB */
	if (stupac2 - VELICINABLOKA < 0) startY = 0;
	else startY = stupac2 - VELICINABLOKA;

	/* DONJI RUB */
	if ((stupac2 + BROJBLOKOVA) > sl2->visina) krajY = sl2->visina - VELICINABLOKA;
	else krajY = stupac2 + VELICINABLOKA;

	minMad = 10000;
	
	for(i = startY; i <= krajY; i++) {
		for(j = startX; j <= krajX; j++) {
			mad = 0;
			for(k = 0; k < VELICINABLOKA; k++) {
				for(l = 0; l < VELICINABLOKA; l++) {
					mad += abs(blok[k][l] - (sl1->p[(k+i) * sl2->sirina + (l+j)].c));
				}
			}

			mad = (double) mad / (VELICINABLOKA * VELICINABLOKA);
			if (mad < minMad) {
				minMad = mad;
				vX = -(red2 - j);
				vY = -(stupac2 - i);
			}
		}
	}

	printf("(%d,%d)", vX, vY);
	return 0;
}