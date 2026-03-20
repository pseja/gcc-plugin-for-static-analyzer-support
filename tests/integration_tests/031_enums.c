// Verifies enumeration type definitions and their usage as integer constants.

enum State
{
    STATE_INIT = 0,
    STATE_RUNNING = 5,
    STATE_ERROR = 10
};

int main()
{
    enum State current = STATE_RUNNING;

    if (current == STATE_ERROR)
    {
        return -1;
    }

    return 0;
}
