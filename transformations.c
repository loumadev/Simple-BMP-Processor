#include "transformations.h"

#include <math.h>

#include "processing.h"

#define PI 3.14159265359

/**
 * Preklopi obrazok okolo zvislej osi.
 *
 * Vytvori kopiu obrazku, ktory je preklopeny okolo zvislej osi.
 * @param image smernik na strukturu bmp_image, v ktorej je nacitany obrazok
 * @return smernik na novy obrazok preklopeny okolo zvislej osi alebo NULL
 */
struct bmp_image* flip_horizontally(const struct bmp_image* image) {
    return pixelProcessor(image, &flipHorizontally);
}

/**
 * Preklopi obrazok okolo vodorovnej osi.
 *
 * Vytvori kopiu obrazku, ktory je preklopeny okolo vodorovnej osi.
 * @param image smernik na strukturu bmp_image, v ktorej je nacitany obrazok
 * @return smernik na novy obrazok preklopeny okolo vodorovnej osi alebo NULL
 */
struct bmp_image* flip_vertically(const struct bmp_image* image) {
    return pixelProcessor(image, &flipVertically);
}

/**
 * Otoci obrazok o 90 stupnov v smere hodinovych ruciciek.
 *
 * Vytvori kopiu obrazka, ktory je otoceny o 90 stupnov v smere hodinovych ruciciek.
 * @param image smernik na strukturu bmp_image, v ktorej je nacitany obrazok
 * @return smernik na novy obrazok otoceny o 90 stupnov v smere hodinovych ruciciek
 */
struct bmp_image* rotate_right(const struct bmp_image* image) {
    struct bmp_image* img = pixelProcessor(image, &rotate90right);

    img->header->width = image->header->height;
    img->header->height = image->header->width;

    return img;
}

/**
 * Otoci obrazok o 90 stupnov proti smeru hodinovych ruciciek.
 *
 * Vytvori kopiu obrazka, ktory je otoceny o 90 stupnov proti smeru hodinovych ruciciek.
 * @param image smernik na strukturu bmp_image, v ktorej je nacitany obrazok
 * @return smernik na novy obrazok otoceny o 90 stupnov proti smeru hodinovych ruciciek
 */
struct bmp_image* rotate_left(const struct bmp_image* image) {
    struct bmp_image* img = pixelProcessor(image, &rotate90left);

    img->header->width = image->header->height;
    img->header->height = image->header->width;

    return img;
}

/**
 * Extrahuje z obrazku jeden alebo viacej farebnych kanalov.
 *
 * Vytvori kopiu obrazku, ktory obsahuje len vybrany farebny kanal.
 *
 * @param image smernik na strukturu bmp_image, v ktorej je nacitany obrazok
 * @param colors_to_keep retazec obsahujuci farebne zlozky (b-blue, g-green, r-red), ktore sa odstrania
 * @return smernik na novy obrazok, v ktorom je len vybrany farebny kanal alebo NULL
 */
struct bmp_image* extract(const struct bmp_image* image, const char* colors_to_keep) {
    return pixelProcessor(
        image,
        ({
            PixelIterator __fn__(struct pixel * pixel, int x, int y, int offset, struct bmp_image* img, struct pixel* originalBuffer) {
                if (!strchr(colors_to_keep, 'r')) pixel->red = 0;
                if (!strchr(colors_to_keep, 'g')) pixel->green = 0;
                if (!strchr(colors_to_keep, 'b')) pixel->blue = 0;
            }
            __fn__;
        })
	);
}

struct bmp_image* grayscale(const struct bmp_image* image, const double amount) {
    return pixelProcessor(
        image,
        ({
            PixelIterator __fn__(struct pixel * pixel, int x, int y, int offset, struct bmp_image* img, struct pixel* originalBuffer) {
                double color = (pixel->red + pixel->green + pixel->blue) / 3;

                pixel->red = lerp(pixel->red, color, amount);
                pixel->green = lerp(pixel->green, color, amount);
                pixel->blue = lerp(pixel->blue, color, amount);
            }
            __fn__;
        })
	);
}

struct bmp_image* sharpen(const struct bmp_image* image, const double amount) {
    const int kernel_size = 3;

    //The compiler is fucked up so have to use numbers instead of consts
    const double kernel[3][3] = {
        {1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0},
        {1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0},
        {1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0}
	};

    const int off = floor(kernel_size / 2.0);

    return pixelProcessor(
        image,
        ({
            PixelIterator __fn__(struct pixel * pixel, int x, int y, int offset, struct bmp_image* img, struct pixel* originalBuffer) {
                double sum[3] = {0.0};

                for (int ky = 0; ky < kernel_size; ky++) {
                    for (int kx = 0; kx < kernel_size; kx++) {
                        int nx = clamp(x - off + kx, 0, img->header->width - 1);
                        int ny = clamp(y - off + ky, 0, img->header->height - 1);

                        struct pixel p = originalBuffer[toOffset(img->header, nx, ny)];

                        sum[0] += p.red * kernel[ky][kx];
                        sum[1] += p.green * kernel[ky][kx];
                        sum[2] += p.blue * kernel[ky][kx];
                    }
                }

                pixel->red = clamp(pixel->red + (pixel->red - sum[0]) * amount, 0, 255);
                pixel->green = clamp(pixel->green + (pixel->green - sum[1]) * amount, 0, 255);
                pixel->blue = clamp(pixel->blue + (pixel->blue - sum[2]) * amount, 0, 255);
            }
            __fn__;
        })
	);
}

struct bmp_image* rotate(const struct bmp_image* image, const double angle, const bool clampToEdge) {
    //image->header->width += cos(angle) * sqrt(2) * image->header->width;
    //image->header->height += sin(angle) * sqrt(2) * image->header->height;

    return pixelProcessor(
        image,
        ({
            PixelIterator __fn__(struct pixel * pixel, int x, int y, int offset, struct bmp_image* img, struct pixel* originalBuffer) {
                int w = img->header->width;
                int h = img->header->height;

                double cx = w / 2.0;
                double cy = h / 2.0;

                double dx = cx /*- 1*/ - x;
                double dy = cy /*- 1*/ - y;

                double nx = cx + dx * cos(angle + PI) - dy * sin(angle + PI);
                double ny = cy + dy * cos(angle + PI) + dx * sin(angle + PI);

                if (!clampToEdge && (nx < 0 || nx > w - 1 || ny < 0 || ny > h - 1)) {
                    pixel->red = 0;
                    pixel->green = 0;
                    pixel->blue = 0;

                    return;
                }

                *pixel = originalBuffer[toOffset(
                    img->header,
                    clamp(round(nx), 0, w - 1),
                    clamp(round(ny), 0, h - 1)
				)];
            }
            __fn__;
        })
	);
}