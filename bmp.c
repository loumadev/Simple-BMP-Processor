#include "bmp.h"

#include "processing.h"

void printHeader(struct bmp_header* header) {
    printf(
        "uint16_t type: 0x%x %d\n\
uint32_t size: 0x%x %d\n\
uint16_t reserved1: 0x%x %d\n\
uint16_t reserved2: 0x%x %d\n\
uint32_t offset: 0x%x %d\n\
uint32_t dib_size: 0x%x %d\n\
uint32_t width: 0x%x %d\n\
uint32_t height: 0x%x %d\n\
uint16_t planes: 0x%x %d\n\
uint16_t bpp: 0x%x %d\n\
uint32_t compression: 0x%x %d\n\
uint32_t image_size: 0x%x %d\n\
uint32_t x_ppm: 0x%x %d\n\
uint32_t y_ppm: 0x%x %d\n\
uint32_t num_colors: 0x%x %d\n\
uint32_t important_colors: 0x%x %d\n",
        header->type, header->type,
		header->size, header->size,
		header->reserved1, header->reserved1, 
		header->reserved2, header->reserved2,
		header->offset, header->offset,
		header->dib_size, header->dib_size,
		header->width, header->width,
		header->height, header->height,
		header->planes, header->planes,
		header->bpp, header->bpp,
		header->compression, header->compression,
		header->image_size, header->image_size,
		header->x_ppm, header->x_ppm,
		header->y_ppm, header->y_ppm,
		header->num_colors, header->num_colors,
		header->important_colors, header->important_colors
	);
}

void printData(struct bmp_header* header, struct pixel* data) {
    for (int i = 0; i < header->height; i++) {
        for (int j = 0; j < header->width; j++) {
            struct pixel* pixel = &data[header->width * i + j];
            printf("[%d, %d, %d] ", pixel->red, pixel->green, pixel->blue);
        }
        printf("\n");
    }
}

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
struct bmp_image* read_bmp(FILE* stream) {
    if (!stream) return fprintf(stderr, "[Error] Failed to open the file!\n"), NULL;

    struct bmp_image* img = malloc(sizeof(struct bmp_image));

    img->header = read_bmp_header(stream);
    img->data = read_data(stream, img->header);

    if (!img->header) return fprintf(stderr, "[Error] Failed to parse the BMP header data!\n"), NULL;
    if (!img->data) return fprintf(stderr, "[Error] Failed to parse the BMP image data!\n"), NULL;

    return img;
}

/**
 * Cita hlavicku BMP suboru zo vstupneho prudu
 *
 * Cita a vrati smernik na hlavicku BMP suboru. Hlavicka je umiestnena na zaciatku.
 * Ak prud nie je otvoreny, alebo je poskodeny (NULL), funkcia vrati NULL.
 *
 * @param stream otvoreny prud, v ktorom sa nachadzaju obrazove udaje
 * @return smernik na strukturu bmp_header alebo NULL
 */
struct bmp_header* read_bmp_header(FILE* stream) {
    if (!stream) return NULL;

    struct bmp_header* header = malloc(sizeof(struct bmp_header));
    fread(header, sizeof(struct bmp_header), 1, stream);

    if (header->type != 0x4D42 /*0x424D*/) return NULL;

    printHeader(header);

    return header;
}

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
struct pixel* read_data(FILE* stream, const struct bmp_header* header) {
    if (!stream) printf("stream");
    if (!header) printf("header");
    if (!stream || !header) return NULL;

    //Calculate required values
    int rowSize = header->width * 3;
    int padding = (rowSize % 4 == 0 ? rowSize : 4 * (rowSize / 4 + 1)) - rowSize;
    int size = header->width * header->height;
    struct pixel* data = malloc(size * sizeof(struct pixel));

    if (!data) return printf("malloc error\n"), NULL;

    printf("padding: %d\n", padding);

    //Read data
    for (int i = 0; i < size; i++) {
        int offset = toOffset(header, i % header->width, header->height - i / header->width - 1);

        fread(&data[offset].blue, 1, sizeof(uint8_t), stream);
        fread(&data[offset].green, 1, sizeof(uint8_t), stream);
        fread(&data[offset].red, 1, sizeof(uint8_t), stream);

        if ((i + 1) % header->width == 0) fseek(stream, padding, SEEK_CUR);
    }

    return data;
}

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
bool write_bmp(FILE* stream, const struct bmp_image* image) {
    if (!stream || !image) return false;

    //Write header
    fwrite(image->header, sizeof(struct bmp_header), 1, stream);

    //Calculate required values
    int rowSize = image->header->width * 3;
    int padding = (rowSize % 4 == 0 ? rowSize : 4 * (rowSize / 4 + 1)) - rowSize;
    int size = image->header->width * image->header->height;

    //Write data
    for (int i = 0; i < size; i++) {
        int x = i % image->header->width;
        int y = image->header->height - i / image->header->width - 1;

        fwrite(&image->data[image->header->width * y + x], sizeof(struct pixel), 1, stream);

        if ((i + 1) % image->header->width == 0) {
            for (int j = 0; j < padding; j++) {
                fwrite(PADDING_CHAR, sizeof(char), 1, stream);
            }
        }
    }

    return true;
}

/**
 * Uvolnenie BMP obrazku z pamate
 *
 * Funkcia uvolni alokovanu pamat pre BMP obrazok.
 *
 * @param image smernik na BMP obrazok
 */
void free_bmp_image(struct bmp_image* image) {
    free(image->header);
    free(image->data);
    free(image);
    image = NULL;
}
