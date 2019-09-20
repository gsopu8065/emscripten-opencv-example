rm -rf ./build
mkdir build
cd build
emconfigure cmake ../src
emmake make
