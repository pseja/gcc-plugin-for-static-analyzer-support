int foo(int x)
{
    return x * 2;
}

int bar(int y)
{
    return foo(y) + 1;
}

int baz(int z)
{
    return bar(z) - foo(z);
}

int main()
{
    int a = foo(10);
    int b = baz(20);

    return a + b;
}
