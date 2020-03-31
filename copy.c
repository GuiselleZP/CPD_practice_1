#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <pthread.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image/stb_image_write.h"

#define THREADS 1
//int* scl,int* tcl,int w,int h,int r
struct canales
{
    int* sclr;
    int* tclr;
    int* sclg;
    int* tclg;
    int* sclb;
    int* tclb;
    int w;
    int h;
    int r;
};
struct canales *canal;	

int max(int num1, int num2){
    return (num1 > num2 ) ? num1 : num2;
}

int min(int num1, int num2) {
    return (num1 > num2 ) ? num2 : num1;
}

void boxesForGauss(int sigma, int n, double* sizes) { // standard deviation, number of boxes
    
    double wIdeal = sqrt((12*sigma*sigma/n)+1);  // Ideal averaging filter width 
    double wl = floor(wIdeal);  
    if(fmod(wl,2.0) == 0.0){
        wl--;	
    } 
    double wu = wl+2;
                
    double mIdeal = (12*sigma*sigma - n*wl*wl - 4*n*wl - 3*n)/(-4*wl - 4);
    double m = round(mIdeal);
    // var sigmaActual = Math.sqrt( (m*wl*wl + (n-m)*wu*wu - n)/12 );
                
    for(int i=0; i<n; i++){
        *(sizes+i) = (i<m?wl:wu);
    } 
}

void boxBlurT_3 (int* scl,int* tcl,int w,int h,int r, int threadId) {
    int initIterationH, endIterationH;
    initIterationH = (h / THREADS) * threadId;
    if(threadId == THREADS - 1){
        endIterationH = h;
    }else{
        endIterationH = (h / THREADS)*(threadId + 1);
    }
    printf("BlurT ID: %i init: %i end %i    ", threadId, initIterationH, endIterationH);
    for(int i=initIterationH; i<endIterationH; i++)
        for(int j=0; j<w; j++) {
            int val = 0;
            for(int iy=i-r; iy<i+r+1; iy++) {
                int y = min(h-1, max(0, iy));
                val += *(scl+(y*w+j));
            }
            *(tcl+(i*w+j)) = val/(r+r+1);
        }
}

void boxBlurH_3 (int* scl,int* tcl,int w,int h,int r, int threadId) {
    int initIterationW, endIterationW;
    initIterationW = (w / THREADS) * threadId;
    if(threadId == THREADS - 1){
        endIterationW = w;
    }else{
        endIterationW = (w / THREADS)*(threadId + 1);
    }
    for(int i = 0; i < h; i++)
        for(int j = initIterationW; j < endIterationW; j++) {
            int val = 0;
            for(int ix = j - r; ix < j + r + 1; ix++) {
                int x = min(w-1, max(0, ix));
                val += *(scl + (i*w+x));
            }
            *(tcl + (i*w+j)) = val/(r+r+1);
        }
} 

void boxBlur_3 (int* scl,int* tcl,int w,int h,int r, int threadId) {
    for(int i=0; i<(w*h); i++) {
        int aux = *(scl+i);
        *(tcl+i) = aux;
    }
    boxBlurH_3(tcl, scl, w, h, r, threadId);
    boxBlurT_3(scl, tcl, w, h, r, threadId);
}


void  gaussBlur_3 (int* scl,int* tcl,int w,int h,int r, int threadId) {
    double  *bxs = (double*)malloc(sizeof(double)*3);
    boxesForGauss(r,3,bxs);
    boxBlur_3 (scl, tcl, w, h, (int)((*(bxs)-1)/2), threadId);
    boxBlur_3 (tcl, scl, w, h, (int)((*(bxs+1)-1)/2), threadId);
    boxBlur_3 (scl, tcl, w, h, (int)((*(bxs+2)-1)/2), threadId);
}

void *parallel(void *arg){
    int threadId = *(int *)arg;
    
    gaussBlur_3(canal -> sclr,canal -> tclr, canal -> w, canal -> h, canal -> r,threadId);
    gaussBlur_3(canal -> sclg,canal -> tclg, canal -> w, canal -> h, canal -> r,threadId);
    gaussBlur_3(canal -> sclb,canal -> tclb, canal -> w, canal -> h, canal -> r,threadId);
}

int main(void) {

    int width, height, channels;
    unsigned char *img = stbi_load("lobo4k.jpg", &width, &height, &channels, 0); //// cero para cargar todos los canales
    if(img == NULL) {
        printf("Error in loading the image\n");
        exit(1);
    }
    printf("Loaded image with a width of %dpx, a height of %dpx and %d channels\n", width, height, channels);

    int img_size  = width * height * channels;
    printf("%i \n",img_size);
    int  *r = (int*)malloc(sizeof(int)*(img_size/3));
    int  *g = (int*)malloc(sizeof(int)*(img_size/3));
    int  *b = (int*)malloc(sizeof(int)*(img_size/3));
    
    int i = 0;
    for(unsigned char *p = img; p != img + img_size; p += channels,i++) {
        *(r+i) = (uint8_t) *p ;
        *(g+i) = (uint8_t) *(p + 1);
        *(b+i) = (uint8_t) *(p + 2);
    }


    int  *r_target = (int*)malloc(sizeof(int)*(img_size/3));
    int  *g_target = (int*)malloc(sizeof(int)*(img_size/3));
    int  *b_target = (int*)malloc(sizeof(int)*(img_size/3));  
    int kernel = 3;
    
    int threadId[THREADS], k, *retval;
    pthread_t thread[THREADS];
    canal = malloc(sizeof(struct canales));
    canal -> sclr = r;
    canal -> tclr = r_target;
    canal -> sclg = g;
    canal -> tclg = g_target;
    canal -> sclb = b;
    canal -> tclb = b_target;
    canal -> w = width;
    canal -> h = height;
    canal -> r = kernel;

    for (k=0; k<THREADS; k++){
        threadId[k] = k;
        pthread_create(&thread[k], NULL, parallel, &threadId[k]);
    }

	/* Join the threads - barrier*/
	for (k=0; k<THREADS; k++)
		pthread_join(thread[k], (void **)&retval);

    int j = 0;
    for(int i = 0; i <img_size/3; i++ ){
        img[j] =  *(r_target+i);
        img[j+1] =  *(g_target+i);
        img[j+2] =  *(b_target+i);
        j+=3;
    }

    stbi_write_jpg("loboblur2.jpg", width, height, channels, img, 100);
    free(r_target);
	free(g_target);
	free(b_target);
	free(r);
	free(g);
	free(b);
}
//gcc -pthread -Wall -pedantic copy.c -o copy -lm