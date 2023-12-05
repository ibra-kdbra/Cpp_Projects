#include "iostream"
#include "ThreadPool.h"

void test1(int a, int b)
{
	a = a + b;
	return;
}

void test2(double a, double b)
{
	a = a + b;
	return;
}

void test3(int a, int b, int& result)
{
	result = a + b;
	return;
}

void callback()
{
	std::cout << "call back is end\n" << std::endl;
}

int main()
{
	ThreadPool pool;
	pool.init(5);
	pool.add(std::bind(&test1, 1, 2), std::bind(&callback));
	pool.add(std::bind(&test2, 1.0, 2.0), std::bind(&callback));
	pool.add(std::bind(&test2, 1.0, 2.0));

	/* Add a unit test that asynchronously passes the reference and gets the asynchronous 
	reference result in the callback function. */
	int result = 0;
	pool.add(std::bind(&test3, 1.0, 2.0, std::ref(result)), [&]()
	{
		std::cout << "result = " << result << std::endl;
	});

	system("pause");
	return 1;
}