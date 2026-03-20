// Verifies basic pointer operations.

int main()
{
    int a = 100;
    int *p = &a;
    int **pp = &p;

    **pp = 200;
    return a;
}
