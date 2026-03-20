// Verifies function call semantics, argument passing, and return value handling.

float foo(int a, float b)
{
    return a * b;
}

int main()
{
    int x = 69;
    float y = 3.14;

    float result = foo(x, y);

    return 0;
}
