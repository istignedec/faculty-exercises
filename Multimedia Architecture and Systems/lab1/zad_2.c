/* DEKODIRANJE */
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

void IDCT(int f[8][8], int F[8][8]) { /* funkcija za racunanje inverzne DCT, vraca f */
	float a;
	int x, y, u, v;
	for (x = 0; x < 8; x++) {
		for (y = 0; y < 8; y++) {
			a = 0.0;
			for (u = 0; u < 8; u++)
				for (v = 0; v < 8; v++)
					a += C(u) * C(v) * (float)F[u][v] * cos((2.0*(float)x + 1.0)*(float)u*3.14 / 16.0) * cos((2.0*(float)y + 1.0)*(float)v*3.14 / 16.0);
			a *= 0.25;
			f[x][y] = a;
		}
	}

}

void DCT_u_YUV() { /* obrnuti DCT */
	int i, j, k, x, y;
	for (i = 0; i < N; i += 8) {
		for (j = 0; j < M; j += 8) {
			for (k = 0; k < 3; k++) {
				int f[8][8] = { 0 };
				int F[8][8] = { 0 };
				for (x = 0; x < 8; x++) {
					for (y = 0; y < 8; y++) {
						F[x][y] = DCT_koef[x + i][y + j][k];
					}
				}
				IDCT(f, F);
				for (x = 0; x < 8; x++) {
					for (y = 0; y < 8; y++) {
						YUV_slika[x + i][y + j][k] = f[x][y];
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

void ZZ_u_DCT(int x, int y, int YUV) { /* sprema iz zig-zag polja u DCT blok */
	int i, j;
	for (i = 0; i < 8; ++i)
		for (j = 0; j < 8; ++j)
			DCT_koef[i + x][j + y][YUV] = zig_zag_polje[zig_zag_tablica[i][j]];
}

void pomak_obrnuto() { /* pomak sa [-128 - 127] na [0 - 255] */
	int i, j, k;
	for (i = 0; i < N; i++)
		for (j = 0; j < M; j++)
			for (k = 0; k < 3; k++)
				YUV_slika[i][j][k] += 128;
}

void generiraj_RGB() { /* YUV slika u RGB (standardna) */
	int i, j;
	for (i = 0; i < N; i++) {
		for (j = 0; j < M; j++) {
			RGB_slika[i][j][0] = 1.0*YUV_slika[i][j][0] + 0.0*(YUV_slika[i][j][1] - 128) + 1.400*(YUV_slika[i][j][2] - 128);
			if(RGB_slika[i][j][0] < 0) RGB_slika[i][j][0] = 0;
			if(RGB_slika[i][j][0] > 255) RGB_slika[i][j][0] = 255;
			RGB_slika[i][j][1] = 1.0*YUV_slika[i][j][0] - 0.343*(YUV_slika[i][j][1] - 128) - 0.711*(YUV_slika[i][j][2] - 128);
			if(RGB_slika[i][j][1] < 0) RGB_slika[i][j][1] = 0;
			if(RGB_slika[i][j][1] > 255) RGB_slika[i][j][1] = 255;
			RGB_slika[i][j][2] = 1.0*YUV_slika[i][j][0] + 1.765*(YUV_slika[i][j][1] - 128) + 0.000*(YUV_slika[i][j][2] - 128);
			if(RGB_slika[i][j][2] < 0) RGB_slika[i][j][2] = 0;
			if(RGB_slika[i][j][2] > 255) RGB_slika[i][j][2] = 255;
		}
	}
}

void kvantiziraj_obrnuto() { /* (obrnuta) kvantizacija DCT koeficijenata */
	int i, j, k;
	for (i = 0; i < N; i++) {
		for (j = 0; j < M; j++) {
			for (k = 0; k < 3; k++) {
				DCT_koef[i][j][k] *= k ? K2[i % 8][j % 8] : K1[i % 8][j % 8];
			}
		}
	}
}


int main(int argc, char* argv[]) {
	
	FILE *ulaz; /* out.txt */
	FILE *izlaz; /* out.ppm */
	
	int sirina = 0; /* sirina slike */
	int visina = 0; /* visina slike */
	
	int i = 0, j = 0, k = 0, l = 0;
	
	int kursor = 0;
	
	size_t velicina;
	char *y_red;
	char *u_red;
	char *v_red;

	unsigned char pom = '\n';

	fopen_s(&ulaz, "out.txt", "r"); /* otvori out.txt */

	if (ulaz == NULL) {
		printf("Datoteka \"out.txt\" ne postoji.");
		return 0;
	}

	fscanf(ulaz, "%d %d", &visina, &sirina);
	while (pom == '\n') {
		pom = getc(ulaz);
	}
	
	N = visina;
	M = sirina;

	ungetc(pom, ulaz);

	velicina = visina * sirina;
	y_red = malloc(velicina);
	u_red = malloc(velicina);
	v_red = malloc(velicina);

	for (i = 0; i < velicina; i++) {
		fscanf(ulaz, "%d ", y_red + i);
	}
	for (i = 0; i < velicina; i++) {
		fscanf(ulaz, "%d ", u_red + i);
	}
	for (i = 0; i < velicina; i++) {
		fscanf(ulaz, "%d ", v_red + i);
	}

	memset(DCT_koef, 0, sizeof(short) * sirina * visina);

	for (i = 0; i < N; i += 8) {
		for (j = 0; j < M; j += 8) {
			for (l = 0; l < 64; l++)
				zig_zag_polje[l] = y_red[kursor++];
			ZZ_u_DCT(i, j, 0);
		}
	}

	kursor = 0;
	for (i = 0; i < N; i += 8) {
		for (j = 0; j < M; j += 8) {
			for (l = 0; l < 64; ++l)
				zig_zag_polje[l] = u_red[kursor++];
			ZZ_u_DCT(i, j, 1);
		}
	}

	kursor = 0;
	for (i = 0; i < N; i += 8) {
		for (j = 0; j < M; j += 8) {
			for (l = 0; l < 64; l++)
				zig_zag_polje[l] = v_red[kursor++];
			ZZ_u_DCT(i, j, 2);
		}
	}

	kvantiziraj_obrnuto();
	DCT_u_YUV();
	pomak_obrnuto();
	generiraj_RGB();

	izlaz = fopen("out.ppm", "wb+");

	fprintf(izlaz, "P6\n");
	fprintf(izlaz, "%d %d\n", visina, sirina);
	fprintf(izlaz, "%d\n", 255);

	for (i = 0; i < visina; i++) {
		for (j = 0; j < sirina; j++) {
			for (k = 0; k < 3; k++) {
				fprintf(izlaz, "%c", RGB_slika[i][j][k]);
			}
		}
	}

	return 0;
}