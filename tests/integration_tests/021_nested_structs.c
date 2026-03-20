// Tests recursion/nesting in structure definitions and deep member access.

struct Engine
{
    int horsepower;
};

struct Car
{
    struct Engine engine;
    int wheels;
};

int main()
{
    struct Car my_car;
    my_car.wheels = 4;
    my_car.engine.horsepower = 500;

    return my_car.engine.horsepower;
}
