#include <iostream>
using namespace std;

long long fac(long long n)
{
	
	if (n == 1) 
	{
		return 1;
	}

	long long temp;

	temp = n - 1;

	temp = fac(temp);

	return n * temp;

}

int main()
{
	int n; while (cin >> n) cout << add(n) << endl;
}
