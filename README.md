# skeletonide
Skeletonide is a parallel implementaion of Zhang-Suen morphological
thinning algorithm written in Halide-lang. It can be used for fast
skeletonization of binary masks.

Status: It can only run one pass at a time 
natively in halide.  

TODO: Either support multiple passes in halide,
or offload that to an extern C function.

## Build
```
mkdir build
cd build
cmake ..
cmake --build .
```

## License
MIT License