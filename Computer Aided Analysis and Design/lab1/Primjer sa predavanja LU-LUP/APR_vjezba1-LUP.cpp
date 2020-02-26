// APR_vjezba1.cpp
//

#include <vector>
#include "stdafx.h"
#include "matrica.h"

using namespace std;

int main() {

	Matrica A("A.txt");
	Matrica P = A.LUP();

	Matrica b("b.txt");

	A = zamjenaRedaka(A, P);
	b = zamjenaRedaka(b, P);

	Matrica L = A.getL();
	Matrica U = A.getU();

	Matrica y = L.sUnaprijed(b);
	Matrica x = U.sUnatrag(y);

	x.print();

	return 0;
}