#include<iostream>
using namespace std;

int fib(int n)
{
	if (n == 1) {
		return 1;
	}
	
	if (n == 2) {
		return 2;
	}
	
	int temp;

	temp = n - 2;
	
	temp = fib(temp);
	
	int temp2;
	
	temp2 = n - 1;

	temp2 = fib(temp2);
	
	temp = temp + temp2;
	
	return temp;
}

int main()
{
	int n; while (cin >> n) cout << fib(n) << endl;
}

