#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        std::cout << "usage: fontgen <input file> <output file>" << std::endl;
        std::cout << "supported formats: jpg,png,tga,bmp,psd,gif,hdr,pic" << std::endl;
        return 1;
    }

    // open image file
    FILE* file = fopen(argv[1], "r");
    if(!file)
    {
        std::cout << "error: couldn't open file '" << argv[1] << "'" << std::endl;
        return 1;
    }
    int x, y, channels;
    stbi_uc* data = stbi_load_from_file(file, &x, &y, &channels, 1);
    if(!data)
    {
        std::cout << "error: couldn't load image from file" << std::endl;
        return 1;
    }

    std::cout << "image: " << x << "x" << y << "@" << channels << std::endl;

    // display to std::cout for now
    for(int i = 0; i < y; i++)
    {
        for(int j = 0; j < x; j++)
        {
            stbi_uc c = data[i * x + j];
            if(c == 0)
                std::cout << "  ";
            else
                std::cout << "##";
        }
        std::cout << std::endl;
    }

    // TODO: analyze the image and generate font
    // using the data!

    stbi_image_free(data);

    return 0;
}
