#include "processing.h"

#include <string.h>

#include "bmp.h"

int toOffset(struct bmp_header* header, int x, int y) {
    return header->width * y + x;
}

double lerp(double v0, double v1, double t) {
    return v0 + t * (v1 - v0);
}

int clamp(int value, int min, int max) {
    return value < min ? min : (value > max ? max : value);
}

struct bmp_image* pixelProcessor(struct bmp_image* image, PixelIterator (*iterator)(struct pixel*, int, int, int, struct bmp_image*, struct pixel*)) {
    int size = image->header->width * image->header->height;

    //Create copy of the original data array
    struct bmp_image* copy = malloc(size * sizeof(struct bmp_image));

    //Fill in fields
    copy->header = malloc(sizeof(struct bmp_header));
    *copy->header = *image->header;

    copy->data = malloc(size * sizeof(struct pixel));
    for (int i = 0; i < size; i++) copy->data[i] = image->data[i];

    //Process each pixel in buffer
    for (int i = 0; i < size; i++) {
        int x = i % image->header->width;
        int y = i / image->header->width;

        (*iterator)(&copy->data[i], x, y, i, image, image->data);
    }

    printf("processing done!\n");

    return copy;
}

PixelIterator flipHorizontally(struct pixel* pixel, int x, int y, int offset, struct bmp_image* image, struct pixel* originalBuffer) {
    *pixel = originalBuffer[toOffset(image->header, image->header->width - 1 - x, y)];
}

PixelIterator flipVertically(struct pixel* pixel, int x, int y, int offset, struct bmp_image* image, struct pixel* originalBuffer) {
    *pixel = originalBuffer[toOffset(image->header, x, image->header->height - 1 - y)];
}

PixelIterator rotate90left(struct pixel* pixel, int x, int y, int offset, struct bmp_image* image, struct pixel* originalBuffer) {
    *pixel = originalBuffer[toOffset(image->header, y, image->header->width - 1 - x)];
}

PixelIterator rotate90right(struct pixel* pixel, int x, int y, int offset, struct bmp_image* image, struct pixel* originalBuffer) {
    *pixel = originalBuffer[toOffset(image->header, image->header->height - 1 - y, x)];
}