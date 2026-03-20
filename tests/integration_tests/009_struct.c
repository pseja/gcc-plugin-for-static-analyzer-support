// Verifies structure type definitions, instantiation, and member access.

struct Point
{
    int x;
    int y;
};

int main()
{
    struct Point p;
    p.x = 10;
    p.y = 20;

    return 0;
}
