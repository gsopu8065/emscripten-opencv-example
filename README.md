# emscripten-opencv-example
This repository is to demonstrate how to access C++ code from javascript


## Use and installation
To run this project, you need to install [Emscripten](https://emscripten.org/) and [Opencv](https://opencv.org/).

### Install OpenCV 
Step1: Installing opencv is easy through brew in mac
```
brew install opencv@3
```
Step2: Download opencv3 from [here](https://github.com/opencv/opencv/archive/3.4.7.zip). This is useful to buld our own wasm files.

### Install Emscripten 

Please follow these [instructions](https://emscripten.org/docs/getting_started/downloads.html) to install emscripten

## Activate Emscripten in the shell

You can add some code to your shell config file.
Personally I just run the following (fish shell) when I need it.

```
source ~/programs/emsdk/emsdk_env.fish
```

## Build OpenCV.js

OpenCV.js is the name of the Emscripten build of OpenCV.
In our case, we are building it for WebAssembly (wasm).

```
python ./platforms/js/build_js.py --build_wasm --emscripten_dir /usr/Emscripten/emsdk/fastcomp/emscripten build_wasm
```

> PS: You can modify the `build_js.py` file to enable/disable modules. If you get any memory related error while building add this line in get_build_flags function <br/>
  flags = "-s WASM_MEM_MAX=512MB -s DEMANGLE_SUPPORT=0 "

## Checkout the project

Once wasm file is build, checkout this project and update CMakeLists.txt file in each directory and run build.sh

```
EmscriptenExample1: This is an example of how to convert C++ to ASM.js
EmscriptenExample2: This is an example of how to convert C++ to WASM
OpenCVExample1: This is an example of how to convert OpenCV C++ (detect rectangle code) to WASM
OpenCVExample2: This is an example of how to convert OpenCV C++ (detect rectangle  and get bird eye view code) to WASM
```

## Output

Open index.html on the browser to see the output

