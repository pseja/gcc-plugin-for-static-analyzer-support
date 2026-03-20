// Verifies complex initialization patterns, such as aggregate initialization lists for arrays of structs.

struct vec2i
{
    int x;
    int y;
};

int main()
{
    struct vec2i arr[] = {{1, 2}, {3, 4}, {5, 6}};

    int a = arr[1].x;
    return a;
}
