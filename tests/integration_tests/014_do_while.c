// Validates loop structure for do-while loops.

int main()
{
    int x = 42;

    do
    {
        x /= 2;
    } while (x > 0);

    return x;
}
