// Verifies usage of void* for generic pointers and casting between pointer types.

int main()
{
    int val = 42;
    void *generic_ptr = &val;

    int *int_ptr = (int *)generic_ptr;

    return *int_ptr;
}
