// Validates conditional branching and control flow graphs for if statements.

int main()
{
    int x = 42;

    if (x > 0)
    {
        x = 10;
    }
    else if (x < 0)
    {
        x = 20;
    }
    else
    {
        x = 30;
    }

    return x;
}
