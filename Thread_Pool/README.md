# Thread_Pool

A simple async thread pool implementation

## Testing

You can change the Thread_Pool to singleton. In the project, I will keep the situation without modification, and may require multiple thread pools.

```:
void test1(int a, int b)
{
 // TODO
 a = a + b;
 return;
}

void test2(double a, double b)
{
 // TODO
 a = a + b;
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
 system("pause");
 return 1;
}
```
