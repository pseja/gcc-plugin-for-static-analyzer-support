// Tests indirect function calls via function pointers.

int add(int a, int b)
{
    return a + b;
}

int sub(int a, int b)
{
    return a - b;
}

int main()
{
    int (*operation)(int, int) = add;
    int result = operation(10, 5);

    operation = sub;
    result += operation(10, 5);

    return result;
}
