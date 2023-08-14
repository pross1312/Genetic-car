#pragma once
#include <cassert>
#include <functional>
#include <cstring>
#include <string>
#include <sstream>
#include <cmath>

inline size_t count = 0;
template<typename Type> class RowVectorX;
template<typename Type> class VectorX;
template<typename Type>
class MatrixX {
private:
    friend class RowVectorX<Type>;
    MatrixX(size_t Rows, size_t Cols, Type* data, bool borrowed):
        nRows         { Rows     },
        nCols         { Cols     },
        data          { data     },
        borrowed_data { borrowed } {}
public:
    using BiIndexFunc    = std::function<Type(size_t, size_t)>;
    using UnaryIndexFunc = std::function<RowVectorX<Type>(size_t)>;
    using NoIndexFunc    = std::function<Type()>;
    template<typename Op2Type>
    using BiTypeFunc     = std::function<Type(Type, Op2Type)>;
    using UnaryTypeFunc  = std::function<Type(Type)>;
    virtual ~MatrixX() {
        if (!borrowed_data) delete[] data;
    }
    explicit MatrixX(size_t Rows, size_t Cols):
        nRows         { Rows                   },
        nCols         { Cols                   },
        data          { new Type[Rows*Cols] {} },
        borrowed_data { false                  }{
            memset(data, 0, nRows*nCols);
    }

    MatrixX(const MatrixX& base): MatrixX(base.nRows, base.nCols) {
        memcpy(this->data, base.data, nRows*nCols*sizeof(Type));
    }

    explicit MatrixX(size_t Rows, size_t Cols, Type val): MatrixX(Rows, Cols) {
        for (size_t i = 0;  i < Rows*Cols; i++) data[i] = val;
    }

    explicit MatrixX(size_t Rows, size_t Cols, const BiIndexFunc& init_func): MatrixX(Rows, Cols) {
        for (size_t row = 0; row < nRows; row++) {
            for (size_t col = 0; col < nCols; col++) {
                data[row*Cols + col] = init_func(row, col);
            }
        }
    }

    explicit MatrixX(size_t Rows, size_t Cols, const UnaryIndexFunc& init_func): MatrixX(Rows, Cols) {
        for (size_t i = 0; i < nRows; i++) {
            memcpy(&this->data[i*nCols], init_func(i).data, nCols*sizeof(Type));
        }
    }

    explicit MatrixX(size_t Rows, size_t Cols, const NoIndexFunc& init_func): MatrixX(Rows, Cols) {
        for (size_t i = 0; i < nRows*nCols; i++) data[i] = init_func();
    }

    inline Type& at(size_t r, size_t c) {
        assert(r < nRows && c < nCols);
        return data[r*nCols + c];
    }

    inline const Type& at(size_t r, size_t c) const {
        assert(r < nRows && c < nCols);
        return data[r*nCols + c];
    }

    inline size_t size() const { return nRows*nCols; }

    inline const MatrixX& replace_with(const MatrixX& object) {
        this->nRows = object.nRows;
        this->nCols = object.nCols;
        delete[] this->data;
        this->data = new Type[object.size()];
        memcpy(this->data, object.data, object.size()*sizeof(Type));
        return *this;
    }

    inline const MatrixX& operator=(const MatrixX& object) {
        if (!(this->nRows == object.nRows && this->nCols == object.nCols)) {
            printf("%zu %zu\n%zu %zu\n", this->nRows, this->nCols, object.nRows, object.nCols);
        }
        assert(this->nRows == object.nRows && this->nCols == object.nCols);
        memcpy(this->data, object.data, nRows*nCols);
        return *this;
    }

    inline RowVectorX<Type> operator[](size_t index) {
        return RowVectorX<Type>(nCols, &this->data[index * nCols], true);
    }

    template<typename Op2Type>
    inline MatrixX<Type> operator+(const MatrixX<Op2Type>& that) const {
        return bi_transformed<Op2Type>(that, [](Type a, Op2Type b) { return a + Type(b); });
    }

    template<typename Op2Type>
    inline void operator+=(const MatrixX<Op2Type>& that) {
        bi_transform<Op2Type>(that, [](Type a, Op2Type b) { return a + Type(b); });
    }

    template<typename Op2Type>
    inline MatrixX<Type> operator-(const MatrixX<Op2Type>& that) const {
        return bi_transformed<Op2Type>(that, [](Type a, Op2Type b) { return a - type(b); });
    }

    template<typename Op2Type>
    inline void operator-=(const MatrixX<Op2Type>& that) {
        bi_transform<Op2Type>(that, [](Type a, Op2Type b) { return a - type(b); });
    }

    inline MatrixX<Type> operator*(Type scalar) const {
        return unary_transformed([&scalar](Type a) { return scalar * a; });
    }

    inline void operator*=(Type scalar) {
        unary_transform([&scalar](Type a) { return scalar * a; });
    }

    template<typename Op2Type>
    inline MatrixX<Type> operator*(const MatrixX<Op2Type>& that) const {
        assert(this->nCols == that.nRows && "Invalid size");
        MatrixX<Type> result(this->nRows, that.nCols);
        for (size_t r = 0; r < this->nRows; r++) {
            for (size_t c = 0; c < that.nCols; c++) {
                for (size_t i = 0; i < this->nCols; i++) {
                    result.at(r, c) += this->at(r, i) * (Type)that.at(i, c);
                }
            }
        }
        return result;
    }

    template<typename Op2Type>
    inline MatrixX bi_transformed(const MatrixX<Op2Type>& object, const BiTypeFunc<Op2Type>& func) const {
        assert(this->nRows == object.nRows && this->nCols == object.nCols);
        MatrixX<Type> result(nRows, nCols);
        for (size_t i = 0; i < nRows*nCols; i++) result.data[i] = (Type)func(this->data[i], object.data[i]);
        return result;
    }

    inline MatrixX unary_transformed(const UnaryTypeFunc& func) const {
        MatrixX<Type> result(nRows, nCols);
        for (size_t i = 0; i < nRows*nCols; i++) result.data[i] = func(data[i]);
        return result;
    }

    template<typename Op2Type>
    inline void bi_transform(const MatrixX<Op2Type>& object, const BiTypeFunc<Op2Type>& func) {
        assert(this->nRows == object.nRows && this->nCols == object.nCols);
        for (size_t i = 0; i < nRows*nCols; i++) this->data[i] = (Type)func(this->data[i], object.data[i]);
    }

    inline void unary_transform(const UnaryTypeFunc& func) {
        for (size_t i = 0; i < nRows*nCols; i++) this->data[i] = func(data[i]);
    }

    friend std::ostream& operator<<(std::ostream& out, const MatrixX<Type>& mat) {
        for (size_t i = 0; i < mat.nRows; i++) {
            out << "[ ";
            for (size_t j = 0; j < mat.nCols; j++) {
                out << std::to_string(mat.at(i, j)) << (j == mat.nCols - 1 ? "" : " ");
            }
            out << " ]\n";
        }
        return out;
    }

    std::string to_string() {
        std::stringstream ss;
        ss << (*this);
        return ss.str();
    }
    inline operator VectorX<Type>() const { return VectorX(*this); } // OK if done through typedef

    size_t nRows;
    size_t nCols;
    Type* data;
    bool borrowed_data;
};

template<typename Type>
class RowVectorX: public MatrixX<Type> {
private:
    friend class MatrixX<Type>;
    RowVectorX(size_t size, Type* data, bool borrowed): MatrixX<Type>(1, size, data, borrowed) {}
public:
    using Base           = MatrixX<Type>;
    using UnaryIndexFunc = std::function<Type(size_t)>;
    using NoIndexFunc    = std::function<Type()>;
    template<typename Op2Type>
    using BiTypeFunc     = std::function<Type(Type, Op2Type)>;
    using Base::unary_transform;
    using Base::unary_transformed;
    using Base::bi_transform;
    using Base::bi_transformed;
    using Base::to_string;
    using Base::operator=;
    using Base::operator+;
    using Base::operator-;
    using Base::operator*;
    using Base::operator+=;
    using Base::operator-=;
    using Base::operator*=;
    using Base::size;
    explicit RowVectorX(size_t size)                                                    : Base(1, size) {}
    explicit RowVectorX(size_t size, Type val)                                          : Base(1, size, val) {}
    explicit RowVectorX(size_t size, Type* data)                                        : RowVectorX() { memcpy(this->data, data, size*sizeof(Type)); }
    explicit RowVectorX(size_t size, std::function<Type(size_t r, size_t c)> init_func) : Base(1, size, init_func) {}
    RowVectorX(const RowVectorX& base)                                                  : Base(base) {}
    explicit RowVectorX(size_t size, const UnaryIndexFunc& init_func)                   : Base(1, size, init_func) {}
    explicit RowVectorX(size_t size, const NoIndexFunc&    init_func)                   : Base(1, size, init_func) {}

    inline       Type&  operator[](size_t index)       { return this->at(0, index); }
    inline const Type&  operator[](size_t index) const { return this->at(0, index); }
};

template<typename Type>
class VectorX: public MatrixX<Type> {
public:
    using Base           = MatrixX<Type>;
    using UnaryIndexFunc = std::function<Type(size_t)>;
    using NoIndexFunc    = std::function<Type()>;
    template<typename Op2Type>
    using BiTypeFunc     = std::function<Type(Type, Op2Type)>;
    using Base::unary_transform;
    using Base::unary_transformed;
    using Base::bi_transform;
    using Base::bi_transformed;
    using Base::to_string;
    using Base::operator=;
    using Base::operator+;
    using Base::operator-;
    using Base::operator*;
    using Base::operator+=;
    using Base::operator-=;
    using Base::operator*=;
    using Base::size;
    VectorX(const VectorX& base)                                                     : Base(base) {}
    explicit VectorX(size_t size)                                                    : Base(size, 1) {}
    explicit VectorX(size_t size, Type val)                                          : Base(size, 1, val) {}
    explicit VectorX(size_t size, Type* data)                                        : VectorX()
        { memcpy(this->data, data, size*sizeof(Type)); }
    explicit VectorX(size_t size, std::function<Type(size_t r, size_t c)> init_func) : Base(size, 1, init_func) {}
    explicit VectorX(size_t size, const UnaryIndexFunc& init_func)                   : Base(size, 1, init_func) {}
    explicit VectorX(size_t size, const NoIndexFunc&    init_func)                   : Base(size, 1, init_func) {}
    explicit VectorX(const MatrixX<Type>& base)                                      : Base(base) {}

    inline       Type&  operator[](size_t index)       { return this->at(index, 0); }
    inline const Type&  operator[](size_t index) const { return this->at(index, 0); }
};
using VectorXf = VectorX<float>;
using MatrixXf = MatrixX<float>;
