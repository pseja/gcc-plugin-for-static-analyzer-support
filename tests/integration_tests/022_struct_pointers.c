// Verifies usage of pointers to structures and the arrow operator for access.

struct Node
{
    int value;
    struct Node *next;
};

int main()
{
    struct Node a, b;
    a.value = 1;
    b.value = 2;

    a.next = &b;
    b.next = 0;

    return a.next->value;
}
