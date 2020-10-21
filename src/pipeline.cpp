#include "Halide.h"
#include "halide_target_check.h"
#include "halide_image_io.h"

using namespace Halide;

// The pipeline below is a single iteration of Zhang-Suen algorithm.
int main(int argc, char **argv) {
    Target target = find_gpu_target();
    if (!target.has_gpu_feature()) {
        return false;
    }
    ImageParam input(type_of<uint8_t>(), 2);
    Expr W = input.width();
    Expr H = input.height();

    Var x_outer, x_inner, y_outer, y_inner, tile_index;

    // offsets of neighbours of a given pixel (x, y)
    int n_x_idx[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
    int n_y_idx[8] = {0, 1, 1, 1, 0, -1, -1, -1};
    Buffer<int> n_x_idx_buf(n_x_idx);
    Buffer<int> n_y_idx_buf(n_y_idx);

    Var x, y, k;
    Func in_bounded = BoundaryConditions::repeat_edge(input);

    // fn to get the nth neighbours  of a pixel
    Func nbr;
    nbr(x, y, k) = in_bounded(x + n_x_idx_buf(k), y + n_y_idx_buf(k));

    // fn to count the number of non-zero neighbours
    RDom j(0, 8);
    Func nbr_cnt("nbr_cnt");
    nbr_cnt(x, y) = sum(select(nbr(x, y, j) > 0, 1, 0));

    // fn to count the number of transitions
    Func zero2one("zero2one");
    RDom t(0, 8);
    Expr zero2one_cond = nbr(x, y, t % 8) == 0 && nbr(x, y, (t + 1) % 8) == 255;
    zero2one(x, y) = sum(select(zero2one_cond, 1, 0));

    // step-1 of Zhang-Suen method
    Expr fst_cnd;
    fst_cnd = (nbr_cnt(x, y) >= 2 && nbr_cnt(x, y) <= 6 && zero2one(x, y) == 1 &&
               nbr(x, y, 0) * nbr(x, y, 2) * nbr(x, y, 4) == 0 &&
               nbr(x, y, 0) * nbr(x, y, 2) * nbr(x, y, 6) == 0);

    Func skel1("skel1");
    skel1(x, y) = cast<uint8_t>(select(fst_cnd, 255, in_bounded(x, y)));
//    skel1.compute_root().vectorize(x, 8).parallel(y);

    Var c, i, block, thread, x0, y0, xi, yi;
    skel1.compute_root().gpu_tile(x, y, x0, y0, xi, yi, 8, 8);

    // step-2 of Zhang-Suen method
    // operate on the array modified in step-1
    Func nbr2;
    nbr2(x, y, k) =
            skel1(clamp(x + n_x_idx_buf(k), 0, W), clamp(y + n_y_idx_buf(k), 0, H));

    RDom j2(0, 8);
    Func nbr_cnt2("nbr_cnt2");
    nbr_cnt2(x, y) = sum(select(nbr2(x, y, j2) > 0, 1, 0));

    Func zero2one2("zero2one2");
    RDom t2(0, 8);
    Expr zero2one_cond2 =
            nbr2(x, y, t2 % 8) == 0 && nbr2(x, y, (t2 + 1) % 8) == 255;
    zero2one2(x, y) = sum(select(zero2one_cond2, 1, 0));

    Expr snd_cnd;
    snd_cnd =
            (nbr_cnt2(x, y) >= 2 && nbr_cnt2(x, y) <= 6 && zero2one2(x, y) == 1 &&
             nbr2(x, y, 0) * nbr2(x, y, 4) * nbr2(x, y, 6) == 0 &&
             nbr2(x, y, 2) * nbr2(x, y, 4) * nbr2(x, y, 6) == 0);

    Func skel2("skel2");
    skel2(x, y) = cast<uint8_t>(select(snd_cnd, 255, skel1(x, y)));
//    skel2.vectorize(x, 8).parallel(y);
    skel2.gpu_tile(x, y, x0, y0, xi, yi, 8, 8);

    // Compiling to a static lib
    skel2.compile_to_static_library("skeletonide", {input}, "skel", target);

    return 0;
}
