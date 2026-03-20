// Tests union type handling and memory aliasing between different members.

union data {
    int i;
    char c;
    char *str;
};

int main()
{
    union data d;
    d.c = 'A';
    d.i = 42;

    return 0;
}
