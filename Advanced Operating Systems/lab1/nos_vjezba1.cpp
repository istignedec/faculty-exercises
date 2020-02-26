// nos_vjezba1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <windows.h>
#include <ctime>
#include <vector>
#include <stack>

using namespace std;

#define ZAHTJEV 1
#define ODGOVOR 0

int logicki_sat;
typedef int Poruka; // vrijednost logickog sata

void ispisi_razmak(int x) {
	printf("\n");
	for (int i = 0; i < x; i++) {
		printf(" ");
	}
}

void uskladi_sat(Poruka dolazna) {
	int dolazno_vrijeme = dolazna;
	if (dolazno_vrijeme >= logicki_sat)	logicki_sat = dolazno_vrijeme + 1;
	else
		logicki_sat++;
}

void odgovori_na_postojece(vector<Poruka>& zahtjevi, vector<MPI_Status>& statusi) {
	for (int i = zahtjevi.size() - 1; i >= 0; i--) {
		Poruka odlazna = logicki_sat;
		MPI_Status s = statusi[i];
		MPI_Send(&odlazna, sizeof(Poruka), MPI_INT, s.MPI_SOURCE, ODGOVOR, MPI_COMM_WORLD);
	}
	zahtjevi.clear();
	statusi.clear();
}

int main(int argc, char** argv) {

	int world_size, world_rank;
	int name_len;
	char processor_name[MPI_MAX_PROCESSOR_NAME];
	vector<Poruka> zahtjevi;
	vector<MPI_Status> statusi;
	int koliko_misli, koliko_jede, brojac;

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Get_processor_name(processor_name, &name_len);

	srand((unsigned)time(NULL));

	ispisi_razmak(world_rank);
	printf("Stvoren filozof %d.", world_rank);
	logicki_sat = 0;

	printf("\n");
	fflush(stdout);

	while (1) {

		koliko_misli = rand() % (5000 - 50 + 1) + 50;
		brojac = 0;

		ispisi_razmak(world_rank);
		printf("Filozof %d : mislim", world_rank);
		fflush(stdout);

		// misli i primaj zahtjeve
		while (brojac <= koliko_misli) {
			brojac += 50;
			Sleep(50);

			int flag;
			MPI_Iprobe(MPI_ANY_SOURCE, ZAHTJEV, MPI_COMM_WORLD, &flag, MPI_STATUS_IGNORE);
			if (flag) {
				// dosao zahtjev za vrijeme spavanja
				Poruka dolazna;
				MPI_Status status;
				// posalji da ne zeli u KO
				MPI_Recv(&dolazna, sizeof(Poruka), MPI_INT, MPI_ANY_SOURCE, ZAHTJEV, MPI_COMM_WORLD, &status);
				uskladi_sat(dolazna);
				ispisi_razmak(world_rank);
				printf("Filozof %d : dosla poruka za vrijeme spavanja od filozofa %d, posalji odgovor, ne zelim u KO, (dajem svoje vilice).", world_rank, status.MPI_SOURCE);
				fflush(stdout);
				Poruka odlazna = dolazna;
				MPI_Send(&odlazna, sizeof(Poruka), MPI_INT, status.MPI_SOURCE, ODGOVOR, MPI_COMM_WORLD);
			}
		}

		// hoce jesti
		bool ceka_vilice = true;
		int odgovori = 0;
		while (ceka_vilice) {

			// kad proces zahtjeva ulazak u KO, salje svim ostalim procesima poruku zahtjev
			Poruka odlazna = logicki_sat;
			for (int i = 0; i < world_size; i++) {
				if(world_rank != i)
					MPI_Send(&odlazna, sizeof(Poruka), MPI_INT, i, ZAHTJEV, MPI_COMM_WORLD);
			}

			ispisi_razmak(world_rank);
			printf("Filozof %d : trazim svoje vilice", world_rank);
			fflush(stdout);

			while (1) {
				// primi bilo koju poruku
				Poruka dolazna;
				MPI_Status status;
				MPI_Recv(&dolazna, sizeof(Poruka), MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

				if (status.MPI_TAG == ZAHTJEV) {
					ispisi_razmak(world_rank);
					printf("Filozof %d : primljen zahtjev od filozofa %d", world_rank, status.MPI_SOURCE);
					fflush(stdout);
					uskladi_sat(dolazna);

					// ako postoji NJEGOV ZAHTJEV koji je dosao prije ovog mog, onda salji odgovor da ne zelim u KO, jer on ima prednost
					if (dolazna < odlazna || ((dolazna == odlazna) && (status.MPI_SOURCE < world_rank))) {
						ispisi_razmak(world_rank);
						printf("Filozof %d : saljem odgovor filozofu %d", world_rank, status.MPI_SOURCE);
						fflush(stdout);
						Poruka odlazna = dolazna;
						MPI_Send(&odlazna, sizeof(Poruka), MPI_INT, status.MPI_SOURCE, ODGOVOR, MPI_COMM_WORLD);
					}
					else {
						zahtjevi.push_back(dolazna);
						statusi.push_back(status);
						ispisi_razmak(world_rank);
						printf("Filozof %d : spremam zahtjev", world_rank);
						fflush(stdout);
					}
				}
				else {
					ispisi_razmak(world_rank);
					printf("Filozof %d : primio odgovor od filozofa %d", world_rank, status.MPI_SOURCE);
					fflush(stdout);
					odgovori++;
				}
				// primio odgovore svih ostalih cvorova, moze uci u KO
				if (odgovori == world_size - 1) {
					ceka_vilice = false;
					break;
				}
			}
		}

		// kad proces primi odgovore svih ostalih, moze uci u KO
		ispisi_razmak(world_rank);
		printf("Filozof %d : jedem", world_rank);
		fflush(stdout);
		koliko_jede = rand() % (5000 - 1000 + 1) + 1000;
		Sleep(koliko_jede);

		// kad izlazi, posalje odgovor svima ciji zahtjevi kod njega cekaju na odgovor
		odgovori_na_postojece(zahtjevi, statusi);
	}
	MPI_Finalize();
	return 0;
}
