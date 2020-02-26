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