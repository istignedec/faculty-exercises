// APR_vjezba1.cpp
//

#include <vector>
#include "stdafx.h"
#include "matrica.h"

using namespace std;

int main() {

	Matrica A("A.txt");
	cout << "A:" << endl;
	A.print();
	Matrica P = A.LUP();
	cout << "LUP od A:" << endl;
	A = zamjenaRedaka(A, P);
	A.print();

	Matrica b("b.txt");
	cout << "b:" << endl;
	b = zamjenaRedaka(b, P);
	b.print();

	Matrica L = A.getL();
	cout << "L:" << endl;
	L.print();

	Matrica U = A.getU();
	cout << "U:" << endl;
	U.print();

	Matrica y = L.sUnaprijed(b);
	cout << "y:" << endl;
	y.print();

	Matrica x = U.sUnatrag(y);
	cout << "x:" << endl;
	x.print();

	return 0;
}