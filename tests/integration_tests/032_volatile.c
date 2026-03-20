// Verifies enumeration type definitions and their usage as integer constants.

int main()
{
    volatile int hardware_register = 0;

    hardware_register = 0xFF;
    int read_back = hardware_register;

    return read_back;
}
