class Snake
{
private:
    struct position
    {
        int x;
        int y;
    };

public:
    position pos[100];
    position pre_pos[100];
    int length;

    Snake();
    void print();
};
