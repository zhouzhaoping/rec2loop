#include <iostream>
using namespace std;

int globalm;

int hehe(int n)
{
	int m;
	if (n == 0)
		return 0;
	else if (n > 5)
	{
		m = n - 1;
		for (;m <100; ++m)
		{
			m = hehe(m);
			if (m < 10)
			{
				return m;
				m = m + 1;
			}	
		}
	}
	else
	{
		while (m < 100)
		{
			m = m + 1;
			m = hehe(m);
			if (m > 10)
				return m;
		}
	}
	return 0;
}

int main()
{
	hehe(10);
}