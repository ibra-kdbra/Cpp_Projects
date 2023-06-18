class Food
{
private:
    struct position
    {
        int x;
        int y;
    };
    char shape = 15;

public:
    position pos;

    Food();
    void print();
};
