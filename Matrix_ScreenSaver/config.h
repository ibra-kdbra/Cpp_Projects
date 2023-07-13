
// Are the characters bold  
#define OPTBOLD
// Time between frames
#define OPTTIME 50000
// Gap between matrix lines
#define OPTGAP 25
// Chance for a line to spawn after OPTGAP frames
#define OPTCHANCE 7
// Smallest possible matrix line height
#define OPTSIZEMIN 5
// Biggest possible matrix line height
#define OPTSIZEMAX 25
// Circular buffer size for matrix lines
#define OPTBUFSIZE 500

// Return random char to be used for matrix (n = rand())
inline static char randchar(unsigned int n) {
	return n % ('~' - '!') + '!';
}

// Set color to the color for the matrix line
#define R 0
#define G 128
#define B 0
#define V 50 // Variance
static unsigned char color[3] = {R, G, B};
inline static void randcolor() {
    color[0] = R + (rand() % V);
    color[1] = G + (rand() % V);
    color[2] = B + (rand() % V);
}
