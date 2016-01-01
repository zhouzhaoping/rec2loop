#include <iostream>
using namespace std;

int add(int n)
{
	if (n == 1)
	{
		return 1;
	}
	
	int temp;
	
	temp = n - 1;
	
	temp = add(temp);
	
	return temp + 1;
}

int main()
{
	int n; while (cin >> n) cout << add(n) << endl;
}
