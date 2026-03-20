// Validates loop structure and control flow back-edges for for loops.

int main()
{
    int x = 42;

    for (int i = 0; i < 10; ++i)
    {
        x += i;
    }

    return x;
}
