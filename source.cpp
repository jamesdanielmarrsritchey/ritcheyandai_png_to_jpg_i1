#include <cstdlib> // Added this line
#include <png.h>
#include <jpeglib.h>
#include <stdio.h>
#include <getopt.h>

int main(int argc, char *argv[]) {
    const char* source_png = NULL;
    const char* destination_jpg = NULL;
    int quality = 100; // Default quality

    struct option long_options[] = {
        {"source_png", required_argument, 0, 's'},
        {"destination_jpg", required_argument, 0, 'd'},
        {"quality", required_argument, 0, 'q'},
        {0, 0, 0, 0}
    };

    int opt;
    while ((opt = getopt_long(argc, argv, "s:d:q:", long_options, NULL)) != -1) {
        switch (opt) {
            case 's':
                source_png = optarg;
                break;
            case 'd':
                destination_jpg = optarg;
                break;
            case 'q':
                quality = atoi(optarg);
                break;
            default:
                fprintf(stderr, "Usage: %s --source_png value --destination_jpg value [--quality value]\n", argv[0]);
                return 1;
        }
    }

    if (source_png == NULL || destination_jpg == NULL) {
        fprintf(stderr, "Usage: %s --source_png value --destination_jpg value [--quality value]\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(source_png, "rb");
    png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);
    png_init_io(png_ptr, fp);
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

    png_bytep *row_pointers = png_get_rows(png_ptr, info_ptr);
    int width = png_get_image_width(png_ptr, info_ptr);
    int height = png_get_image_height(png_ptr, info_ptr);

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);

    FILE *outfile = fopen(destination_jpg, "wb");
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, quality, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    JSAMPROW row_pointer[1];
    while (cinfo.next_scanline < cinfo.image_height) {
        row_pointer[0] = row_pointers[cinfo.next_scanline];
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    jpeg_finish_compress(&cinfo);
    fclose(outfile);

    jpeg_destroy_compress(&cinfo);
    fclose(fp);

    return 0;
}

/*
To compile this program, you need to have libpng and libjpeg installed on your system. 
You can install these libraries on Debian-based systems with the following commands:

sudo apt-get update
sudo apt-get install libpng-dev
sudo apt-get install libjpeg-dev

Then, you can compile the program with:

g++ main.cpp -o main -lpng -ljpeg

To run the program, use:

./main --source_png input.png --destination_jpg output.jpg --quality 90

Replace 'input.png' with the path to your source PNG file, 'output.jpg' with the path where you want to save the output JPEG file, and '90' with the desired quality (an integer between 0 and 100).
*/