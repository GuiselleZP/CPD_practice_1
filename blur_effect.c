#include <stdio.h>
#include <stdlib.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

int main(void) {
    int width, height, channels;
    unsigned char *img = stbi_load("test.jpg", &width, &height, &channels, 0); //// cero para cargar todos los canales
    if(img == NULL) {
        printf("Error in loading the image\n");
        exit(1);
    }
    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, channels);


    size_t img_size  = width * height * channels;
    int s = 0;

    int r[img_size/3];
    int g[img_size/3];
    int b[img_size/3];

    //Dividir canales RGB
    int i = 0;
    for(unsigned char *p = img; p != img + img_size; p += channels,i++) {
        r[i] = (uint8_t) *p ;
        g[i] = (uint8_t) *(p + 1);
        b[i] = (uint8_t) *(p + 2);

    }
    printf("%i ",i);

    //Matriz de rgb
    int count = 0;
    for(int x = 0; x< img_size/3; x++,count++){
        if(count > width){
            printf("\n");
            count = 0;
        }

        printf("%i ", r[x]);

    }

    //Rearmar imagen desde la matriz
    unsigned char new_image[img_size];
    int j =0;
    for(int i = 0; i <img_size/3; i++ ){
        new_image[j] = (char) r[i];
        new_image[j+1] = (char) g[i];
        new_image[j+2] = (char) b[i];
        j+=3;
    }

    stbi_write_jpg("testChanged.jpg", width, height, channels, new_image, 100);

}
//gcc -Wall -pedantic blur_effect.c -o blur -lm

//Algoritmo 1
// source channel, target channel, width, height, radius
function gaussBlur_1 (int* scl,int* tcl,int w,int h,int r) {
    double rs = Math.ceil(r * 2.57);     // significant radius
    for(int i = 0; i < h; i++)
        for(int j = 0; j < w; j++) {
            int val = 0, wsum = 0;
            for(int iy = i-rs; iy < i + rs + 1; iy++)
                for(int ix = j-rs; ix < j + rs + 1; ix++) {
                    int x = Math.min(w - 1, Math.max(0, ix));
                    int y = Math.min(h - 1, Math.max(0, iy));
                    int dsq = (ix - j)*(ix - j)+(iy - i)*(iy - i);
                    int wght = Math.exp( -dsq / (2*r*r) ) / (Math.PI*2*r*r);
                    val += scl[y * w + x] * wght;  wsum += wght;
                }
            tcl[i * w + j] = Math.round(val / wsum);
        }
}
