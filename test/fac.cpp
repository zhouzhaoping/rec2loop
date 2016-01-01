

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

