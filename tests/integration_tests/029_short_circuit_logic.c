// Verifies correct control flow generation for short-circuit evaluation of logical && and ||.

int check(int val)
{
    return val > 0;
}

int main()
{
    int a = 1;
    int b = -1;

    // GIMPLE lowers this into multiple basic blocks!
    if (a > 0 && check(b) || a == 5)
    {
        return 1;
    }

    return 0;
}
