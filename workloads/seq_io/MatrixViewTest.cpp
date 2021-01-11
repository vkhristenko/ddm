#include <iostream>
#include <cassert>

#include "MatrixView.hpp"

using ScalarType = float;
constexpr int NRows = 10;
constexpr int NCols = 10;

using namespace dmwm;

int main(int argc, char* argv[]) {
    // allocate storage
    auto *data = static_cast<ScalarType*>(std::malloc(sizeof(ScalarType) * NRows * NCols));

    // fill the view
    MatrixView<ScalarType> m{data, NRows, NCols};
    for (int i=0; i<NRows; i++)
        for (int j=0; j<NCols; j++)
            m(i, j) = i+j;
    
    // test 
    for (int i=0; i<NRows; i++)
        for (int j=0; j<NCols; j++)
            assert(m(i, j) == i+j);

    printf("all works!\n");

    return 0;
}
