# skeletonide
<img src="./assets/2spooky.png" width="250px"/>

Skeletonide is a parallel implementaion of Zhang-Suen morphological
thinning algorithm written in Halide-lang. It can be used for fast
skeletonization of binary masks.

When you build the project, it generates an ahead-of-time
compiled static library from the halide pipeline. It is then
linked with the caller code to generate a single binary.

Note: The halide pipeline represents a single pass of the
Zhang-Suen method. The iterations have to be handled by the 
caller code - see `spook.cpp` for an example. The number of
iterations is hardcoded right now. It should depend on the 
completion flags returned by the halide pipeline.

The output on the scikit-image's horse mask:

Mask:  
![mask](test/images/horse.png)    

Skeleton:  
![skel](test/output/spook_out.png)  

## Build

```sh
mkdir build
cd build
cmake ..
cmake --build .
# this will build the static library and its 
# header in `build/` and a single test binary 
# called `spook` in `skeletonide` - which can
# be run `./spook` to see it in action.
```

## License
MIT License