#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "stb_image/stb_image.h"
#include "stb_image/stb_image_write.h"

// Error utility macro
#define ON_ERROR_EXIT(cond, message) \
do { \
    if((cond)) { \
        printf("Error in function: %s at line %d\n", __func__, __LINE__); \
        perror((message)); \
        exit(1); \
    } \
} while(0)

enum allocation_type{
	NO_ALLOCATION, SELF_ALLOCATED, STB_ALLOCATED
};

typedef struct{
	int width;
	int height;
	int channels;
	size_t size;
	uint8_t *data;
	enum allocation_type allocation_;
}Image;

static inline bool str_ends_in(const char *str, const char *ends){
	char *pos = strrchr(str, '.');
	return !strcmp(pos, ends);
}

void imageFree(Image *img){
	if(img->allocation_ != NO_ALLOCATION && img->data != NULL){
		if(img->allocation_ == STB_ALLOCATED){
			stbi_image_free(img->data);
		}else{
			free(img->data);
		}
		img->data = NULL;
		img->width = 0;
		img->height = 0;
		img->size = 0;
		img->allocation_ = NO_ALLOCATION;
	}
}

void imageCreate(Image *img, int width, int height, int channels, bool zero){
	size_t size = width * height * channels;
	if(zero) {
        img->data = calloc(size, 1);
    } else {
        img->data = malloc(size);
    }

    if(img->data != NULL) {
        img->width = width;
        img->height = height;
        img->size = size;
        img->channels = channels;
        img->allocation_ = SELF_ALLOCATED;
    }
}

void imageLoad(Image *img, const char *fname){
	img->data = stbi_load(fname, &img->width, &img->height, &img->channels, 0);
	ON_ERROR_EXIT(img->data == NULL, "Error in stbi_load");
	img->size = img->width * img->height * img->channels;
	img->allocation_ = STB_ALLOCATED;
}

void imageSave(const Image *img, const char *fname){
	if(str_ends_in(fname, ".jpg")||str_ends_in(fname, ".JPG")||str_ends_in(fname, ".jpeg")||str_ends_in(fname, ".jpeg")){
		stbi_write_jpg(fname, img->width, img->height, img->channels, img->data, 100);
	}else if(str_ends_in(fname, ".png")||str_ends_in(fname, ".PNG")){
		stbi_write_png(fname, img->width, img->height, img->channels, img->data, img->width * img->channels);
	}else{
		ON_ERROR_EXIT(false, "Invalid format");
	}
}

void imageToGray(const Image *orig, Image *gray){
    ON_ERROR_EXIT(!(orig->allocation_ != NO_ALLOCATION && orig->channels >= 3), "The input image must have at least 3 channels.");
    int channels = orig->channels == 4 ? 2 : 1;
    imageCreate(gray, orig->width, orig->height, channels, false);
    ON_ERROR_EXIT(gray->data == NULL, "Error in creating the image");
 
    for(unsigned char *p = orig->data, *pg = gray->data; p != orig->data + orig->size; p += orig->channels, pg += gray->channels) {
        *pg = (uint8_t)((*p + *(p + 1) + *(p + 2))/3.0);
        if(orig->channels == 4) {
            *(pg + 1) = *(p + 3);
        }
    }
}

int main(int argc, char *argv[]){
	if(argc < 5){
		printf("./blr nameImageImput nameImageOutput\n");
		return 0;
	}

	char nameInp[32] = "",
		nameOut[32] = "";

	int kernel = atoi(argv[3]),
		thread = atoi(argv[4]);

	strcpy(nameInp, "input_images/");
	strcat(nameInp, argv[1]);
	strcpy(nameOut, "output_images/");
	strcat(nameOut, argv[2]);

	Image img, out;
	imageLoad(&img, nameInp);
	ON_ERROR_EXIT(img.data == NULL, "Error in loading the image");

	imageToGray(&img, &out);
	imageSave(&out, nameOut);

	imageFree(&img);
	imageFree(&out);

	return 0;
}
