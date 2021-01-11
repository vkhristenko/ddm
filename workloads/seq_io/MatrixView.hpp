#ifndef MatrixView_hpp
#define MatrixView_hpp

namespace ddm {

enum class StorageType : char {
    RowMajor,
    ColMajor
};

template<typename T, StorageType Storage = StorageType::ColMajor>
struct MatrixView {
    MatrixView(T* data, int nrows, int ncols) 
        : nrows{nrows}
        , ncols{ncols}
        , data{data}
    {}

    T& operator()(int irow, int icol) {
        if constexpr (Storage == StorageType::ColMajor) {
            return data[icol*nrows + irow];
        } else {
            return data[irow*ncols + icol];
        }
    }
    
    T const& operator()(int irow, int icol) const {
        if constexpr (Storage == StorageType::ColMajor) {
            return data[icol*nrows + irow];
        } else {
            return data[irow*ncols + icol];
        }
    }

    int TotalBytes() const {
        return nrows * ncols * sizeof(T);
    }

    int nrows, ncols;
    T* data;
};

}

#endif // MatrixView_hpp
