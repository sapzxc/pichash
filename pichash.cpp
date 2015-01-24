
#include <stdio.h>
#include <inttypes.h>

typedef unsigned long long ulong64;

#define cimg_debug 0
#define cimg_display 0
#include "CImg.h"
using namespace cimg_library;

CImg<float>* ph_dct_matrix(const int N){
    CImg<float> *ptr_matrix = new CImg<float>(N,N,1,1,1/sqrt((float)N));
    const float c1 = sqrt(2.0/N); 
    for (int x=0;x<N;x++){
        for (int y=1;y<N;y++){
            *ptr_matrix->data(x,y) = c1*cos((cimg::PI/2/N)*y*(2*x+1));
        }
    }
    return ptr_matrix;
}

int ph_dct_imagehash(CImg<uint8_t> src,ulong64 &hash)
{
    CImg<float> meanfilter(7,7,1,1,1);
    CImg<float> img;
    if (src.spectrum() == 3){
        img = src.RGBtoYCbCr().channel(0).get_convolve(meanfilter);
    } else if (src.spectrum() == 4){
        int width = img.width();
        int height = img.height();
        int depth = img.depth();
        img = src.crop(0,0,0,0,width-1,height-1,depth-1,2).RGBtoYCbCr().channel(0).get_convolve(meanfilter);
    } else {
    	img = src.channel(0).get_convolve(meanfilter);
    }

    img.resize(32,32);
    CImg<float> *C  = ph_dct_matrix(32);
    CImg<float> Ctransp = C->get_transpose();
    CImg<float> dctImage = (*C)*img*Ctransp;
    CImg<float> subsec = dctImage.crop(1,1,8,8).unroll('x');
    float median = subsec.median();

    ulong64 one = 0x0000000000000001;
    hash = 0x0000000000000000;
    for (int i=0;i< 64;i++){
        float current = subsec(i);
        if (current > median)  hash |= one;
    	one = one << 1;
    }
  
    delete C;

    return 0;
}

int main(int argc, void** argv)
{
    if(argc < 2)
    {
        fprintf(stderr, "Image Hash Programm. It return hash of visualy picture from jpeg and png formats.\n");
        fprintf(stderr, "use: %s <image file>\n", argv[0]);
        return 1;
    }

    char *file = (char*)argv[1];

    CImg<uint8_t> src;

    try {
        src.load(file);
    } catch (CImgIOException ex){
        try {
            src.load_other(file);
        } catch (CImgIOException ex){   
            fprintf(stderr, "Could not read file [%s]\n", file);
            return 1;
        }   
    }

    ulong64 h;
    int ret = ph_dct_imagehash(src, h);
    
    // if hash is null try reload image with _other function
    if(!h)
    {
        try {
            src.load_other(file);
        } catch (CImgIOException ex){   
            fprintf(stderr, "Could not read file [%s]\n", file);
            return 1;
        }

        ret = ph_dct_imagehash(src, h);
    }
     
    // out results   
    if(!ret)
    {
        printf("%016" PRIX64 "\n",h);
    }
    else
    {
        fprintf(stderr, "Error when try get hash of file [%s]\n", argv[1]);
    }
    return 0;
}
