// Tests the ternary conditional operator and its translation to conditional branches or specific instructions.

int main()
{
    int a = 10;
    int b = 20;

    // MAX is a ternary operator in GIMPLE, so this should be represented as a single instruction with opcode MAX
    int max = (a > b) ? a : b;

    return max;
}
