// Checks handling of noreturn functions (like abort) and unreachable code markers.

int main()
{
    int x = 10;
    if (x == 10)
    {
        __builtin_trap(); // forces ABORT instruction
    }

    __builtin_unreachable(); // forces UNREACHABLE instruction
}
