/* dithering, an easy way */

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <png.h>

#define HEADER_SIZE 8

static png_uint_32 height;
static png_uint_32 width;
static int bit_depth;
static int color_type;
static png_bytep* row_pointers;

void read_png(char const* const filename);
void transform_png(uint8_t const factor);
void write_png(char const* const filename);

int main(int argc, char** argv) {

    if (argc < 4) {
        fprintf(stdout, "provide a png file and output name and scale factor you dumass!\n");
        exit(EXIT_FAILURE);
    }

    read_png(argv[1]);
    transform_png(strtol(argv[3], NULL, 10));
    write_png(argv[2]);

    return EXIT_SUCCESS;
}

void read_png(char const* const filename) {

    FILE *fp = fopen(filename, "r"); // add "b" if windows but WINDOWS SUCKS SO NO!!!
    
    if (!fp) {
        fprintf(stdout, "couldn't open the file!\n");
        exit(EXIT_FAILURE);
    }

    uint8_t header[HEADER_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0}; // i mean 

    if (fread(header, sizeof(header[0]), HEADER_SIZE, fp) != HEADER_SIZE) {
        fprintf(stdout, "error reading header bytes\n");
        exit(EXIT_FAILURE);
    } 

    /* check whether file is a png */
    
    const bool is_png = (png_sig_cmp(header, 0, HEADER_SIZE) == 0); 
    if (!is_png) {
        fprintf(stdout, "not a png!\n");
        exit(EXIT_FAILURE);
    }

    /* basic info */

    fprintf(stdout, " Compiled with libpng %s. \n", PNG_LIBPNG_VER_STRING);

    /* initializing the necessary structures */

    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (!png_ptr) {
        fprintf(stdout, "failed to create a png struct!\n");
        exit(EXIT_FAILURE);
    }

    png_infop png_info_ptr = png_create_info_struct(png_ptr);

    if (!png_info_ptr) {
        fprintf(stdout, "couldn't create a png info struct!\n");
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        exit(EXIT_FAILURE);
    }

    /* jumping when libpng encounters an error */

    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stdout, "jumping from libpng, caused by an error!\n"); 
        png_destroy_read_struct(&png_ptr, &png_info_ptr, NULL);    
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    /* setting up the input code */

    png_init_io(png_ptr, fp);
    png_set_sig_bytes(png_ptr, HEADER_SIZE);

    /* reading, finally! */
    
    png_read_info(png_ptr, png_info_ptr);
    
    /* getting some info about the file */

    png_get_IHDR(png_ptr, png_info_ptr, &width, &height, &bit_depth, &color_type, NULL, NULL, NULL);
    fprintf(stdout, " Width: %d Height: %d Bit depth: %d Color type: %d\n", width, height, bit_depth, color_type);

    /* reading rows */

    row_pointers = malloc(sizeof(png_bytep) * height);

    for (size_t row = 0; row < height; row++) row_pointers[row] = NULL;
    for (size_t row = 0; row < height; row++) row_pointers[row] = png_malloc(png_ptr, png_get_rowbytes(png_ptr, png_info_ptr)); 

    png_read_image(png_ptr, row_pointers);
    png_destroy_read_struct(&png_ptr, &png_info_ptr, NULL);
    fclose(fp);
}

void transform_png(uint8_t const factor) {

    /* TODO: modifications */

    /* png_set_quantize? FOR N00BS! */

    printf("\n%d\n", factor);
    for(int y = 0; y < height; y++) {
        png_bytep row = row_pointers[y];
        for(int x = 0; x < width; x++) {
            png_bytep px = &(row[x * 4]);

            unsigned char* red = &px[0];
            unsigned char* green = &px[1];
            unsigned char* blue = &px[2];

            *red = (unsigned char)(round((double)(*red * factor) / 255.0) * (255.0/(double)factor));
            *green = (unsigned char)(round((double)(*green * factor) / 255.0) * (255.0/(double)factor));
            *blue = (unsigned char)(round((double)(*blue * factor) / 255.0) * (255.0/(double)factor));
        }
    }
}

void write_png(char const* const filename) {

    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stdout, "couldn't open the file!\n");
        exit(EXIT_FAILURE);
    }

    png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
        fprintf(stdout, "failed to create a png struct!\n");
        exit(EXIT_FAILURE);
    }

    png_infop png_info_ptr = png_create_info_struct(png_ptr);
    if (!png_info_ptr) {
        fprintf(stdout, "failed to create an info struct!\n");
        exit(EXIT_FAILURE);
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        fprintf(stdout, "jumping from libpng, caused by an error!\n"); 
        png_destroy_write_struct(&png_ptr, &png_info_ptr); 
        fclose(fp);
        exit(EXIT_FAILURE);
    }

    png_init_io(png_ptr, fp);

    /* TODO: do stuff with info, stolen from the internet */

    png_set_IHDR(
        png_ptr,
        png_info_ptr,
        width, height,
        8,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT
        );
    
    png_write_info(png_ptr, png_info_ptr);

    if (!row_pointers) {
        fprintf(stdout, "where are the row pointers?!\n");
        exit(EXIT_FAILURE);
    }

    png_write_image(png_ptr, row_pointers);
    png_write_end(png_ptr, NULL);

    /* clean up */
    
    for(int y = 0; y < height; y++) free(row_pointers[y]);
    free(row_pointers);

    fclose(fp);
    png_destroy_write_struct(&png_ptr, &png_info_ptr);
}
