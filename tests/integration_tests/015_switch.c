int main()
{
    int x = 42;

    switch (x)
    {
    case 0:
        x += 1;
        break;
    case 1:
        x += 2;
        break;
    case 2 .. 41:
        x += 3;
        break;
    default:
        x += 4;
        break;
    }

    return x;
}
