#include <emscripten/bind.h>

using namespace emscripten;

int asurionadd(int a, int b) {
    return a+b;
}

EMSCRIPTEN_BINDINGS(my_module) {
    function("asurionadd", &asurionadd);
}