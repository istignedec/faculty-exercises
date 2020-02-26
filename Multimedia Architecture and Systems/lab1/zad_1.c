/* KODIRANJE */
/* Program napisan i kompajliran unutar GCC - MinGW (Windows 10) */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

int N, M;
short RGB_slika[4096][4096][3]; /* slika u standardnom formatu */
short YUV_slika[4096][4096][3]; /* boje preslikane u YUV */
short DCT_koef[4096][4096][3]; /* DCT koeficijenti po svakom bitu */

float RGB_YUV_faktori[3][4] = { /* faktori za promjenu prostora boja */
	{ 0.299, 0.587, 0.114, 0 },
	{ -0.168736, -0.331264, 0.5, 128 },
	{ 0.5, -0.418688, -0.081312, 128 }
};

char K1[8][8] = { { 16, 11, 10, 16, 24, 40, 51, 61 }, /* kvantizacijska tablica za Y */
				  { 12, 12, 14, 19, 26, 58, 60, 55 },
				  { 14, 13, 16, 24, 40, 57, 69, 56 },
				  { 14, 17, 22, 29, 51, 87, 80, 62 },
				  { 18, 22, 37, 56, 68, 109, 103, 77 },
				  { 24, 36, 55, 64, 81, 104, 113, 92 },
				  { 49, 64, 78, 87, 103, 121, 120, 101 },
				  { 72, 92, 95, 98, 112, 100, 103, 99 } };

char K2[8][8] = { { 17, 18, 24, 47, 99, 99, 99, 99 }, /* kvantizacijska tablica za U i V */
				  { 18, 21, 26, 66, 99, 99, 99, 99 },
				  { 24, 26, 56, 99, 99, 99, 99, 99 },
				  { 47, 66, 99, 99, 99, 99, 99, 99 },
				  { 99, 99, 99, 99, 99, 99, 99, 99 },
				  { 99, 99, 99, 99, 99, 99, 99, 99 },
				  { 99, 99, 99, 99, 99, 99, 99, 99 },
				  { 99, 99, 99, 99, 99, 99, 99, 99 } };

float C(int u) { /* funkcija koristena u DCT */
	if (u == 0)
		return (1.0 / sqrt(2.0));
	else
		return 1.0;
}

void DCT(int f[8][8], int F[8][8]) { /* funkcija za racunanje DCT, vraca F */
	float a;
	int x, y, u, v;
	for (u = 0; u < 8; u++) {
		for (v = 0; v < 8; v++) {
			a = 0.0;
			for (x = 0; x < 8; x++)
				for (y = 0; y < 8; y++)
					a += (float)f[x][y] * cos((2.0*(float)x + 1.0)*(float)u*3.14 / 16.0)*cos((2.0*(float)y + 1.0)*(float)v*3.14 / 16.0);
			F[u][v] = (int)(0.25*C(u)*C(v)*a);
		}
	}
}

void YUV_u_DCT() { /* DCT */
	int i, j, k, x, y;
	for (i = 0; i < N; i += 8) {
		for (j = 0; j < M; j += 8) {
			for (k = 0; k < 3; k++) {
				int f[8][8] = { 0 };
				int F[8][8] = { 0 };
				for (x = 0; x < 8; x++) {
					for (y = 0; y < 8; y++) {
						f[x][y] = YUV_slika[x + i][y + j][k];
					}
				}
				DCT(f, F);
				for (x = 0; x < 8; x++) {
					for (y = 0; y < 8; y++) {
						DCT_koef[x + i][y + j][k] = F[x][y];
					}
				}
			}
		}
	}
}

short zig_zag_polje[64];

short zig_zag_tablica[8][8] = {
	{ 0, 1, 5, 6, 14, 15, 27, 28 },
	{ 2, 4, 7, 13, 16, 26, 29, 42 },
	{ 3, 8, 12, 17, 25, 30, 41, 43 },
	{ 9, 11, 18, 24, 31, 40, 44, 53 },
	{ 10, 19, 23, 32, 39, 45, 52, 54 },
	{ 20, 22, 33, 38, 46, 51, 55, 60 },
	{ 21, 34, 37, 47, 50, 56, 59, 61 },
	{ 35, 36, 48, 49, 57, 58, 62, 63 }
};

void DCT_u_ZZ(int x, int y, int YUV) { /* sprema iz DCT bloka u zig-zag polje */
	int i, j;
	for (i = 0; i < 8; ++i)
		for (j = 0; j < 8; ++j)
			zig_zag_polje[zig_zag_tablica[i][j]] = DCT_koef[i + x][j + y][YUV];
}

void pomak() { /* pomak sa [0 - 255] na [-128 - 127] */
	int i, j, k;
	for (i = 0; i < N; i++)
		for (j = 0; j < M; j++)
			for (k = 0; k < 3; k++)
				YUV_slika[i][j][k] -= 128;
}

void generiraj_YUV() { /* RGB (standardna) u YUV slika */
	float pom;
	int i, j, k, l;
	for (i = 0; i < N; i++) {
		for (j = 0; j < M; j++) {
			for (k = 0; k < 3; k++) {
				pom = RGB_YUV_faktori[k][3];
				for (l = 0; l < 3; l++)
					pom += RGB_slika[i][j][l] * RGB_YUV_faktori[k][l];
				YUV_slika[i][j][k] = pom;
			}
		}
	}
}

void kvantiziraj() { /* kvantizacija DCT koeficijenata */
	int i, j, k;
	for (i = 0; i < N; i++) {
		for (j = 0; j < M; j++) {
			for (k = 0; k < 3; k++) {
				DCT_koef[i][j][k] /= k ? K2[i % 8][j % 8] : K1[i % 8][j % 8];
			}
		}
	}
}


int main(int argc, char* argv[]) {

	char *datoteka = argv[1]; /* ulazna slika (lenna.ppm) */
	FILE *ulaz; /* ulazni tok podataka */
	FILE *izlaz; /* izlazni tok za out.txt */
	
	int sirina = 0; /* sirina slike */
	int visina = 0; /* visina slike */
	int maxvrijednost = 0; /* maxval, 255 */

	int i = 0, j = 0, k = 0, l = 0;
	
	int komentar = 0;
	unsigned char pom;
	
	ulaz = fopen(datoteka, "rb");
	if (ulaz == NULL) {
		printf("Datoteka neuspjesno otvorena.\n");
		return 0;
	}
	
	pom = getc(ulaz);
	if (pom != 'P' && (pom = getc(ulaz)) != '6') {
		printf("Datoteka nije P6.\n");
		return 0;
	}

	pom = getc(ulaz);
	pom = getc(ulaz);
	while (pom == '\n' && pom == '\t' && pom == ' ' && pom == '\r') {
		pom = getc(ulaz);
	}
	
	komentar = 0;
	while (pom == '#') {
		while (getc(ulaz) != '\n');
		pom = getc(ulaz);
		komentar++;
	}
	if (komentar != 0) {
		ungetc(pom, ulaz);
	}

	fscanf(ulaz, "%d %d", &visina, &sirina);
	while (pom == '\n' && pom == '\t' && pom == ' ' && pom == '\r') {
		pom = getc(ulaz);
	}

	fscanf(ulaz, "%d", &maxvrijednost);

	while (pom == '\n' && pom == '\t' && pom == ' ' && pom == '\r') {
		pom = getc(ulaz);
	}

	pom = getc(ulaz);

	for (i = 0; i < visina; i++) {
		for (j = 0; j < sirina; j++) {
			for (k = 0; k < 3; k++) {
				fread(&RGB_slika[i][j][k], 1, 1, ulaz); /* ucitaj sliku */
			}
		}
	}

	izlaz = fopen("out.txt", "w+"); /* otvori tok podataka za out.txt */
	
	N = visina;
	M = sirina;
	
	generiraj_YUV();
	pomak();
	YUV_u_DCT();
	kvantiziraj();

	fprintf(izlaz, "%d %d", sirina, visina);
	
	fputs("\n\n", izlaz);
	for (i = 0; i < N; i += 8){ 
		for (j = 0; j < M; j += 8) {
			DCT_u_ZZ(i, j, 0);
			for (l = 0; l < 64; l++) {
				fprintf(izlaz, "%d ", zig_zag_polje[l]); /* Y */
			}
		}
	}
	
	fputs("\n\n", izlaz);
	for (i = 0; i < N; i += 8) {
		for (j = 0; j < M; j += 8) {
			DCT_u_ZZ(i, j, 1);
			for (l = 0; l < 64; ++l) {
				fprintf(izlaz, "%d ", zig_zag_polje[l]); /* Cb */
			}
		}
	}

	fputs("\n\n", izlaz);
	for (i = 0; i < N; i += 8) {
		for (j = 0; j < M; j += 8) {
			DCT_u_ZZ(i, j, 2);
			for (l = 0; l < 64; l++) {
				fprintf(izlaz, "%d ", zig_zag_polje[l]); /* Cr */
			}
		}
	}

	fclose(izlaz);
	return 0;
}