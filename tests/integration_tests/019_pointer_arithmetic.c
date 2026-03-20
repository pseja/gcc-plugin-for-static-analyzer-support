// Tests pointer arithmetic operations and their translation to IR memory offsets.

int main()
{
    int arr[5] = {10, 20, 30, 40, 50};
    int *ptr = arr;

    ptr = ptr + 2;
    ptr++;

    return *ptr;
}
