// Tests handling of bit-fields within structures and bit-masking operations in IR.

struct Flags
{
    unsigned int is_ready : 1;
    unsigned int status_code : 3;
    unsigned int : 4;
    unsigned int mode : 8;
};

int main()
{
    struct Flags f;
    f.is_ready = 1;
    f.status_code = 5;
    f.mode = 255;

    return f.status_code;
}
