// A example to illustrate the usage of skeletonide.
// We load an in image and call the pipeline in a loop.
// TODO Check for completion flags

#include "skeletonide.h"
#include "HalideBuffer.h"
#include "halide_image_io.h"
#include <stdio.h>

using namespace Halide;
int main(int argc, char **argv){
    Runtime::Buffer<uint8_t> input = Tools::load_and_convert_image("test/images/horse.png");
    Runtime::Buffer<uint8_t> output(input.width(), input.height());

    int err;
    for(int i=0; i<32; i++){
        err = skel(input, output);
        input = output;
    }

    if (err) {
        printf("Error %d\n", err);
        return -1;
    }

    Tools::convert_and_save_image(output, "spook_out.png");
    printf("Image saved\n");
    return 0;
}