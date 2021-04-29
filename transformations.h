#ifndef _TRANSFORMATIONS_H
#define _TRANSFORMATIONS_H

#include "bmp.h"

/**
 * Preklopi obrazok okolo zvislej osi.
 *
 * Vytvori kopiu obrazku, ktory je preklopeny okolo zvislej osi.
 * @param image smernik na strukturu bmp_image, v ktorej je nacitany obrazok
 * @return smernik na novy obrazok preklopeny okolo zvislej osi alebo NULL
 */
struct bmp_image* flip_horizontally(const struct bmp_image* image);

/**
 * Preklopi obrazok okolo vodorovnej osi.
 *
 * Vytvori kopiu obrazku, ktory je preklopeny okolo vodorovnej osi.
 * @param image smernik na strukturu bmp_image, v ktorej je nacitany obrazok
 * @return smernik na novy obrazok preklopeny okolo vodorovnej osi alebo NULL
 */
struct bmp_image* flip_vertically(const struct bmp_image* image);

/**
 * Otoci obrazok o 90 stupnov v smere hodinovych ruciciek.
 *
 * Vytvori kopiu obrazka, ktory je otoceny o 90 stupnov v smere hodinovych ruciciek.
 * @param image smernik na strukturu bmp_image, v ktorej je nacitany obrazok
 * @return smernik na novy obrazok otoceny o 90 stupnov v smere hodinovych ruciciek
 */
struct bmp_image* rotate_right(const struct bmp_image* image);

/**
 * Otoci obrazok o 90 stupnov proti smeru hodinovych ruciciek.
 *
 * Vytvori kopiu obrazka, ktory je otoceny o 90 stupnov proti smeru hodinovych ruciciek.
 * @param image smernik na strukturu bmp_image, v ktorej je nacitany obrazok
 * @return smernik na novy obrazok otoceny o 90 stupnov proti smeru hodinovych ruciciek
 */
struct bmp_image* rotate_left(const struct bmp_image* image);

/**
 * Extrahuje z obrazku jeden alebo viacej farebnych kanalov.
 *
 * Vytvori kopiu obrazku, ktory obsahuje len vybrany farebny kanal.
 *
 * @param image smernik na strukturu bmp_image, v ktorej je nacitany obrazok
 * @param colors_to_keep retazec obsahujuci farebne zlozky (b-blue, g-green, r-red), ktore sa odstrania
 * @return smernik na novy obrazok, v ktorom je len vybrany farebny kanal alebo NULL
 */
struct bmp_image* extract(const struct bmp_image* image, const char* colors_to_keep);

struct bmp_image* grayscale(const struct bmp_image* image, const double amount);
struct bmp_image* sharpen(const struct bmp_image* image, const double amount);
struct bmp_image* rotate(const struct bmp_image* image, const double angle, const bool clampToEdge);

#endif
