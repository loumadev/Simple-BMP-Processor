#include <signal.h>

#include "bmp.h"
#include "processing.h"
#include "transformations.h"

void handler(int sig) {
    fprintf(stderr, "\n\033[1;31m[!] Error: signal %d\033[0;0m\n", sig);
    exit(1);
}

int main(int argc, char* argv[]) {
    signal(SIGSEGV, handler);

    //Load file
    FILE* stream = fopen("./assets/lenna.bmp", "rb");
    struct bmp_image* img = read_bmp(stream);
    fclose(stream);

    //Failed to open image
    if (!img) exit(1);

    //Do processing
    //img = flip_vertically(img);
    //img = flip_horizontally(img);
    //img = rotate_left(img);
    //img = rotate_right(img);
    //img = extract(img, "g");
    //img = grayscale(img, 0.5);
    //img = sharpen(img, 0.8);
    img = rotate(img, 38 * (3.1415 / 180), true);

    //Save file
    FILE* out = fopen("./assets/output.bmp", "wb");
    write_bmp(out, img);
    free_bmp_image(img);
    fclose(out);
}
