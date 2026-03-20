// Tests support for inline assembly blocks and asm statements within the IR.

int main()
{
    int result = 0;
    __asm__ __volatile__("nop" : "=r"(result));

    return result;
}
