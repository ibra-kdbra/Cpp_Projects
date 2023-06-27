
// Are the characters bold
#define OPTTBOLD
// Time between frames
#define OPTTIME 15000
// Chance for the pipe to change direction
#define OPTCHANCE 80

// Symbols to use for pipes
const char chars[][4] = {"┃", "━", "┏", "┓", "┗", "┛"};

// Set color of the pipe
#define R 255
#define G 221
#define B 255
#define V 50 // Variance
static unsigned char color[3] = {R, G, B};
inline static void randcolor() {
	color[0] = R - (rand() % V);
	color[1] = G - (rand() % V);
	color[2] = B - (rand() % V);
}
