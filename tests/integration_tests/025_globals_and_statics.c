// Tests handling of global variables and static local variables (storage duration).

int global_var = 42;
const int global_const = 100;

int counter()
{
    static int count = 0;
    count++;
    return count;
}

int main()
{
    global_var += counter();
    global_var += counter();
    return global_var;
}
