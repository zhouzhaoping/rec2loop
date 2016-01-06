#include <iostream>
using namespace std;
//int add(int n);
//int hehe();
//int test();
//int main();

int main()
{
	int n;
	add(n);
	while (cin >> n) cout << add(n) << endl;
}

int test()
{
	hehe();
        return 0;
}
int add(int n, int m, int a)
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
int hehe()
{
	add(1);
	add(1);
        add(1);
        return test();
}
