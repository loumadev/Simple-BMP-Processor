#include "bmp.h"

typedef void PixelIterator;

int toOffset(struct bmp_header* header, int x, int y);
double lerp(double v0, double v1, double t);
int clamp(int value, int min, int max);

struct bmp_image* pixelProcessor(struct bmp_image* image, PixelIterator (*iterator)(struct pixel*, int, int, int, struct bmp_image*, struct pixel*));

PixelIterator flipHorizontally(struct pixel* pixel, int x, int y, int offset, struct bmp_image* image, struct pixel* originalBuffer);
PixelIterator flipVertically(struct pixel* pixel, int x, int y, int offset, struct bmp_image* image, struct pixel* originalBuffer);
PixelIterator rotate90left(struct pixel* pixel, int x, int y, int offset, struct bmp_image* image, struct pixel* originalBuffer);
PixelIterator rotate90right(struct pixel* pixel, int x, int y, int offset, struct bmp_image* image, struct pixel* originalBuffer);