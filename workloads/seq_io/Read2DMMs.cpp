#include <iostream>

#include "FileInterfaces.hpp"
#include "MatrixView.hpp"

using namespace ddm;

using ScalarType = int;

template<typename M>
void PrintMatrix(M const& m) {
    for (int i=0; i<m.nrows; i++) {
        for (int j=0; j<m.ncols; j++)
            printf("%d ", m(i, j));

        printf("\n");
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("run with ./exe <path to a file>\n");

        return 1;
    }

    // cli args
    std::string pathToFile{argv[1]};

    // input file
    InputFile in{pathToFile};

    // header vars
    int nevents = -1;
    int nmats = -1;
    int N = -1;

    // read the header
    in.Read(sizeof(decltype(nevents)), &nevents);
    in.Read(sizeof(decltype(nmats)), &nmats);
    in.Read(sizeof(decltype(N)), &N);

    // TODO: verify the total file size wrt the header + scalar type
    printf("%d events with %d mats/event with N=%d\n",
        nevents, nmats, N);

    // loop and read
    for (int ie=0; ie<nevents; ie++) {
        // reuse memory
        auto *data = static_cast<ScalarType*>(std::malloc(sizeof(ScalarType) * N*N));

        auto assertMatrix = [](auto const& m, int ie, int im, int lhsrhs) {
            for (int irow=0; irow<m.nrows; irow++)
                for (int icol=0; icol<m.ncols; icol++) {
                    auto value = m(irow, icol) == ie-im+irow-icol+lhsrhs;
                    assert(value);
                }
        };

        // lhs matrices
        for (int im=0; im<nmats; im++) {
            MatrixView<ScalarType> m{data, N, N};
            in.Read(m.TotalBytes(), m.data);
            assertMatrix(m, ie, im, 1);
        }
        
        // rhs matrices
        for (int im=0; im<nmats; im++) {
            MatrixView<ScalarType> m{data, N, N};
            in.Read(m.TotalBytes(), m.data);
            assertMatrix(m, ie, im, -1);
        }
    }

    return 0;
}
