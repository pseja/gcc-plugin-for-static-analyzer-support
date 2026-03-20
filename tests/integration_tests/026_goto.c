// Verifies unconditional jumps and handling of arbitrary goto statements.

int main()
{
    int x = 0;

start:
    x++;
    if (x < 5)
    {
        goto start;
    }

    return x;
}
