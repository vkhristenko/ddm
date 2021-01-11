#include <iostream>

#include "FileInterfaces.hpp"
#include "MatrixView.hpp"

using ScalarType = int;

using namespace ddm;

int main(int argc, char *argv[]) {
    if (argc!=5) {
        printf("run with: ./exe <path to a file to create> "
                "<number of records/events> "
                "<number of matrices per record/event> "
                "<N - dimension of mats>\n");
        return 1;
    }

    // cli args
    std::string pathToFile{argv[1]};
    int const nevents = std::atoi(argv[2]);
    int const nmats = std::atoi(argv[3]);
    int const N = std::atoi(argv[4]);
    
    // output file
    OutputFile out{pathToFile};

    // write the header
    out.Write(sizeof(decltype(nevents)), &nevents);
    out.Write(sizeof(decltype(nmats)), &nmats);
    out.Write(sizeof(decltype(N)), &N);

    // loop and write
    for (int ie=0; ie<nevents; ie++) {
        // reuse the memory
        auto *data = static_cast<ScalarType*>(std::malloc(sizeof(ScalarType) * N*N));

        auto fillMatrix = [](auto& m, int ie, int im, int lhsrhs) {
            for (int irow=0; irow<m.nrows; irow++)
                for (int icol=0; icol<m.ncols; icol++)
                    m(irow, icol) = ie - im + irow - icol + lhsrhs;
        };

        // lhs matrices
        for (int im=0; im<nmats; im++) {
            MatrixView<ScalarType> m{data, N, N};
            fillMatrix(m, ie, im, 1);
            out.Write(m.TotalBytes(), m.data);
        }

        // rhs 
        for (int im=0; im<nmats; im++) {
            MatrixView<ScalarType> m{data, N, N};
            fillMatrix(m, ie, im, -1);
            out.Write(m.TotalBytes(), m.data);
        }
    }

    return 0;
}
