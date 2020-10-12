#include "Halide.h"
#include "halide_image_io.h"

using namespace Halide;
int main(int argc, char **argv){
    Buffer<float> input = Tools::load_and_convert_image("images/horse.png");
    int W = input.width();
    int H = input.height();

    Buffer<bool> one2zero(W, H);

    int n_x_idx[8] = {-1,-1,0,1,1,1,0,-1};
    int n_y_idx[8] = {0,1,1,1,0,-1,-1,-1};
    
    Buffer<int> n_x_idx_buf(n_x_idx);
    Buffer<int> n_y_idx_buf(n_y_idx);
    Var x, y, k;
    Func in_bounded = BoundaryConditions::repeat_edge(input);
    Func erase;

    Func neigh;
    neigh(x, y, k) = in_bounded(x+n_x_idx_buf(k), y+n_y_idx_buf(k));
    
    RDom j(0, 7);
    Func nsum;
    nsum(x, y) = sum(neigh(x, y, j));
    nsum.compute_root();
    nsum.trace_stores();
    nsum.realize(H, W);
    return 0;
}