#ifndef _BMP_H
#define _BMP_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define PADDING_CHAR "\0"

/**
 * Struktura obsahujuca informacie o type, velkosti, rozlozeni, rozmeroch
 * a farebnom formate BMP suboru. Ma velkost 54 bajtov.
*/
struct bmp_header {
    uint16_t type;              // "BM" (0x42, 0x4D)
    uint32_t size;              // file size
    uint16_t reserved1;         // not used (0)
    uint16_t reserved2;         // not used (0)
    uint32_t offset;            // offset to image data (54B)
    uint32_t dib_size;          // DIB header size (40B)
    uint32_t width;             // width in pixels
    uint32_t height;            // height in pixels
    uint16_t planes;            // 1
    uint16_t bpp;               // bits per pixel (1/4/8/24)
    uint32_t compression;       // compression type (0/1/2) 0
    uint32_t image_size;        // size of picture in bytes, 0
    uint32_t x_ppm;             // X Pixels per meter (0)
    uint32_t y_ppm;             // X Pixels per meter (0)
    uint32_t num_colors;        // number of colors (0)
    uint32_t important_colors;  // important colors (0)
} __attribute__((__packed__));

/**
 * Struktura popisujuca intenzitu farebnych zloziek R,G,B.
 */
struct pixel {
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    //uint8_t alpha;
} __attribute__((__packed__));

/**
 * Struktura popisujuca format BMP suboru, ktory pozostava z dvoch casti:
 * 1. hlavicka (metadata)
 * 2. obrazove data (body)
 */
struct bmp_image {
    struct bmp_header* header;
    struct pixel* data;  // pocet bodov = sirka * vyska
};

/**
 * Cita BMP subor zo vstupneho prudu
 *
 * Vytvori strukturu bmp_image. Ak stream je NULL 
 * alebo  je subor poskodeny (nejde o BMP subor), funkcia vrati NULL
 * a zobrazi chybovu spravu na standardny vystup.
 *
 * @param stream otvoreny prud, v ktorom sa nachadzju data
 * @return smernik na strukturu bmp_image alebo NULL
 */
struct bmp_image* read_bmp(FILE* stream);

/**
 * Zapise BMP obrazok do vystupneho prudu
 *
 * Funkcia zapise BMP obrazok do otvoreneho prudu. Ak stream nie je otvoreny (NULL)
 * alebo image je NULL, funkcia vrati false.
 *
 * @param stream otvoreny prud pre zapis obrazku
 * @param image smernik na zapisovany obrazok
 * @return true, ak BMP obrazok bol uspesne zapisany, inac false.
 */
bool write_bmp(FILE* stream, const struct bmp_image* image);

/**
 * Cita hlavicku BMP suboru zo vstupneho prudu
 *
 * Cita a vrati smernik na hlavicku BMP suboru. Hlavicka je umiestnena na zaciatku.
 * Ak prud nie je otvoreny, alebo je poskodeny (NULL), funkcia vrati NULL.
 *
 * @param stream otvoreny prud, v ktorom sa nachadzaju obrazove udaje
 * @return smernik na strukturu bmp_header alebo NULL
 */
struct bmp_header* read_bmp_header(FILE* stream);

/**
 * Cita obrazove body.
 *
 * Cita udaje (body) zo vstupneho prudu. Ak prud nie je otvoreny, alebo
 * hlavicka nebola poskytnuta, vrati NULL.
 *
 * @param stream otvoreny prud, v ktorom sa nachadzaju obrazove udaje
 * @param header smernik na nacitanu hlavicku BMP suboru
 * @return smernik na postupnost obrazovych bodov obrazku alebo NULL
 */
struct pixel* read_data(FILE* stream, const struct bmp_header* header);

/**
 * Uvolnenie BMP obrazku z pamate
 *
 * Funkcia uvolni alokovanu pamat pre BMP obrazok.
 *
 * @param image smernik na BMP obrazok
 */
void free_bmp_image(struct bmp_image* image);

#endif
