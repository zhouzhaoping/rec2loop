#include <iostream>

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
	int n; 
	while (std::cin >> n) 
	{
		if (n <= 0)
			break;
		std::cout << add(n) << std::endl;
	}
	long long n2;
	while (std::cin >> n2) 
	{
		if (n2 <= 0)
			break;
		std::cout << fac(n2) << std::endl;
	}
	while (std::cin >> n) 
	{
		if (n <= 0)
			break;
		std::cout << fib(n) << std::endl;
	}
	return 0;
}
