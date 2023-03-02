#pragma onces

#include <types.h>
#include <std/vec.h>
#include <std/traits.h>

#include <cmath>

namespace core {

using namespace coretypes;

// TODO: Need to implement at least the following:
//       Implement these in transforms.h :
//       - translate
//       - rotate
//       - scale
//       Implement here:
//       - orhto ( orthographic projection matrix )
//       - persp ( perspective projection matrix )
//       - lookat ( look at matrix )

template<i32 DimRow, i32 DimCol, typename T> struct mat;

template<i32 DimRow, i32 DimCol, typename T1, typename T2>
constexpr bool mequals(const mat<DimRow, DimCol, T1>& m1, const mat<DimRow, DimCol, T2>& m2) {
    for (i32 i = 0; i < m1.dimensionsRows(); ++i) {
        if (!vequals(m1[i], m2[i])) return false;
    }
    return true;
}

template<i32 DimRow, i32 DimCol, typename T1, typename T2>
constexpr bool msafeequals(const mat<DimRow, DimCol, T1>& m1, const mat<DimRow, DimCol, T2>& m2, T1 epsilon) {
    for (i32 i = 0; i < m1.dimensionsRows(); ++i) {
        if (!vsafeequals(m1[i], m2[i], epsilon)) return false;
    }
    return true;
}

template<i32 DimRow, i32 DimCol, typename T1>
constexpr void mnegate(mat<DimRow, DimCol, T1>& m) {
    for (i32 i = 0; i < m.dimensionsRows(); ++i) {
        vnegate(m[i]);
    }
}

template<i32 DimRow, i32 DimCol, typename TDst, typename TVal>
constexpr void madd(mat<DimRow, DimCol, TDst>& dst, const TVal& val) {
    static_assert(std::is_arithmetic_v<TVal>, "TVal must be an arithmetic");
    for (i32 i = 0; i < dst.dimensionsRows(); ++i) {
        vadd(dst[i], val);
    }
}

template<i32 DimRow, i32 DimCol, typename TDst, typename TSrc>
constexpr void madd(mat<DimRow, DimCol, TDst>& dst, const mat<DimRow, DimCol, TSrc>& src) {
    for (i32 i = 0; i < dst.dimensionsRows(); ++i) {
        vadd(dst[i], src[i]);
    }
}

template<i32 DimRow, i32 DimCol, typename TDst, typename TVal>
constexpr void msub(mat<DimRow, DimCol, TDst>& dst, const TVal& val) {
    static_assert(std::is_arithmetic_v<TVal>, "TVal must be an arithmetic");
    for (i32 i = 0; i < dst.dimensionsRows(); ++i) {
        vsub(dst[i], val);
    }
}

template<i32 DimRow, i32 DimCol, typename TDst, typename TSrc>
constexpr void msub(mat<DimRow, DimCol, TDst>& dst, const mat<DimRow, DimCol, TSrc>& src) {
    for (i32 i = 0; i < dst.dimensionsRows(); ++i) {
        vsub(dst[i], src[i]);
    }
}

template<i32 DimRow, i32 DimCol, typename TDst, typename TVal>
constexpr void mmul(mat<DimRow, DimCol, TDst>& dst, const TVal& val) {
    static_assert(std::is_arithmetic_v<TVal>, "TVal must be an arithmetic");
    for (i32 i = 0; i < dst.dimensionsRows(); ++i) {
        vmul(dst[i], val);
    }
}

template<i32 DstDimRow, i32 DstDimCol, i32 SrcDimRow, i32 SrcDimCol, typename TDst, typename TSrc>
constexpr mat<DstDimRow, SrcDimCol, TDst> mmul2(const mat<DstDimRow, DstDimCol, TDst>& dst, const mat<SrcDimRow, SrcDimCol, TSrc>& src) {
    static_assert(DstDimCol == SrcDimRow, "Dimensions of matrices are not compatible");

    // Multiplyting matrix with different dimensions.
    // This code has to create a new matrix!
    mat<DstDimRow, SrcDimCol, TDst> ret;

    for (i32 i = 0; i < ret.dimensionsRows(); ++i) {
        for (i32 j = 0; j < ret.dimensionsCols(); ++j) {
            ret[i][j] = dst[i][0] * static_cast<TDst>(src[0][j]);
            for (i32 k = 1; k < DstDimCol; ++k) {
                ret[i][j] += dst[i][k] * static_cast<TDst>(src[k][j]);
            }
        }
    }

    return ret;
}

template<i32 DimRow, i32 DimCol, typename T>
struct mat {
    using DataType = T;
    static_assert(std::is_trivial_v<DataType>, "DataType must be a trivial");
    static_assert(std::is_arithmetic_v<DataType>, "DataType must be an arithmetic");
    static_assert(DimRow > 0, "DimRow must be greater than 0");
    static_assert(DimCol > 0, "DimCol must be greater than 0");

    static constexpr i32 dimensionsRows() { return DimRow; }
    static constexpr i32 dimensionsCols() { return DimCol; }

    using ColType = vec<DimCol, DataType>;
    template <typename TM>
    using MatrixType = mat<DimRow, DimCol, TM>;

    ColType data[DimRow] = {}; // initializing to zero allows use in constexpr.. c++ nonsense

    constexpr ColType& operator[](i32 i) {
        Assert(i >= 0 && i < DimRow, "Index out of bounds");
        return data[i];
    }
    constexpr const ColType& operator[](i32 i) const {
        Assert(i >= 0 && i < DimRow, "Index out of bounds");
        return data[i];
    }

    constexpr bool equals(const MatrixType<DataType>& other) const                   { return mequals(*this, other); }
    constexpr bool equals(const MatrixType<DataType>& other, DataType epsilon) const { return msafeequals(*this, other, epsilon); }

    constexpr void negate() { mnegate(*this); }

    template<typename U>  constexpr void add(const MatrixType<U>& other) { madd(*this, other); }
    template<typename TA> constexpr void add(TA v)                       { madd(*this, v); }
                          constexpr void inc()                           { add(1); }

    template<typename U>  constexpr void sub(const MatrixType<U>& other) { msub(*this, other); }
    template<typename TA> constexpr void sub(TA v)                       { msub(*this, v); }
                          constexpr void dec()                           { sub(1); }

    template<typename TA> constexpr void mul(TA v)                       { mmul(*this, v); }
    template<typename TA> constexpr void scale(TA v)                     { mul(v); }

    // TODO: Write div here...

#pragma region overloads "=="

    constexpr bool operator==(const MatrixType<T>& other) const { return equals(other); }
    constexpr bool operator!=(const MatrixType<T>& other) const { return !equals(other); }

#pragma endregion

#pragma region overloads negation

    constexpr MatrixType<DataType> operator-() const {
        MatrixType<DataType> ret = *this;
        ret.negate();
        return ret;
    }

#pragma endregion

#pragma region overloads increment and decrement

    constexpr MatrixType<DataType>& operator++() { inc(); return *this; }
    constexpr MatrixType<DataType>& operator--() { dec(); return *this; }

    constexpr MatrixType<DataType> operator++(int) { MatrixType<DataType> tmp(*this); inc(); return tmp; }
    constexpr MatrixType<DataType> operator--(int) { MatrixType<DataType> tmp(*this); dec(); return tmp; }

#pragma endregion

#pragma region overloads assignment operators

    constexpr MatrixType<DataType>& operator+=(const MatrixType<DataType>& other) { add(other); return *this; }
    constexpr MatrixType<DataType>& operator+=(DataType v)                        { add(v); return *this; }
    constexpr MatrixType<DataType>& operator-=(const MatrixType<DataType>& other) { sub(other); return *this; }
    constexpr MatrixType<DataType>& operator-=(DataType v)                        { sub(v); return *this; }
    constexpr MatrixType<DataType>& operator*=(DataType v)                        { mul(v); return *this; }

#pragma endregion

#pragma region overloads "+"

    friend constexpr MatrixType<DataType> operator+(const MatrixType<DataType>& v1, const MatrixType<DataType>& v2) {
        MatrixType<DataType> ret = v1;
        ret.add(v2);
        return ret;
    }
    template<typename U>
    friend constexpr MatrixType<DataType> operator+(const MatrixType<DataType>& v1, U v2) {
        MatrixType<DataType> ret = v1;
        ret.add(v2);
        return ret;
    }
    template<typename U>
    friend constexpr MatrixType<DataType> operator+(U v1, const MatrixType<DataType>& v2) {
        MatrixType<DataType> ret = v2;
        ret.add(v1);
        return ret;
    }

#pragma endregion

#pragma region overloads "-"

    friend constexpr MatrixType<DataType> operator-(const MatrixType<DataType>& v1, const MatrixType<DataType>& v2) {
        MatrixType<DataType> ret = v1;
        ret.sub(v2);
        return ret;
    }
    template<typename U>
    friend constexpr MatrixType<DataType> operator-(const MatrixType<DataType>& v1, U v2) {
        MatrixType<DataType> ret = v1;
        ret.sub(v2);
        return ret;
    }
    template<typename U>
    friend constexpr MatrixType<DataType> operator-(U v1, const MatrixType<DataType>& v2) {
        MatrixType<DataType> ret = v2;
        for (i32 i = 0; i < ret.dimensions(); ++i) {
            ret[i] = -(ret[i] - static_cast<T>(v1));
        }
        return ret;
    }

#pragma endregion

#pragma region overloads "*"

    friend constexpr MatrixType<DataType> operator*(const MatrixType<DataType>& v1, const MatrixType<DataType>& v2) {
        return mmul2(v1, v2);
    }
    template<typename U>
    friend constexpr MatrixType<DataType> operator*(const MatrixType<DataType>& v1, U v2) {
        MatrixType<DataType> ret = v1;
        ret.mul(v2);
        return ret;
    }
    template<typename U>
    friend constexpr MatrixType<DataType> operator*(U v1, const MatrixType<DataType>& v2) {
        MatrixType<DataType> ret = v2;
        ret.mul(v1);
        return ret;
    }

#pragma endregion

};

template<typename T> using mat1x1 = mat<1, 1, T>;
template<typename T> using mat1x2 = mat<1, 2, T>;
template<typename T> using mat1x3 = mat<1, 3, T>;
template<typename T> using mat1x4 = mat<1, 4, T>;
template<typename T> using mat2x1 = mat<2, 1, T>;
template<typename T> using mat2x2 = mat<2, 2, T>;
template<typename T> using mat2x3 = mat<2, 3, T>;
template<typename T> using mat2x4 = mat<2, 4, T>;
template<typename T> using mat3x1 = mat<3, 1, T>;
template<typename T> using mat3x2 = mat<3, 2, T>;
template<typename T> using mat3x3 = mat<3, 3, T>;
template<typename T> using mat3x4 = mat<3, 4, T>;
template<typename T> using mat4x1 = mat<4, 1, T>;
template<typename T> using mat4x2 = mat<4, 2, T>;
template<typename T> using mat4x3 = mat<4, 3, T>;
template<typename T> using mat4x4 = mat<4, 4, T>;

template<typename T>
mat1x1<T> m1x1(const vec<1, T>& v0) {
    mat1x1<T> ret; ret[0] = v0;
    return ret;
}
template<typename T>
mat1x1<T> m1x1(T x0) {
    mat1x1<T> ret;
    ret[0][0] = x0;
    return ret;
}

template<typename T>
mat1x2<T> m1x2(const vec2<T>& v0) {
    mat1x2<T> ret; ret[0] = v0;
    return ret;
}
template<typename T>
mat1x2<T> m1x2(T x0, T y0) {
    mat1x2<T> ret;
    ret[0][0] = x0; ret[0][1] = y0;
    return ret;
}
template<typename T>
mat1x2<T> m1x2(T v) {
    mat1x2<T> ret;
    ret[0][0] = ret[0][1] = v;
    return ret;
}

template<typename T>
mat1x3<T> m1x3(const vec3<T>& v0) {
    mat1x3<T> ret; ret[0] = v0;
    return ret;
}
template<typename T>
mat1x3<T> m1x3(T x0, T y0, T z0) {
    mat1x3<T> ret;
    ret[0][0] = x0; ret[0][1] = y0; ret[0][2] = z0;
    return ret;
}
template<typename T>
mat1x3<T> m1x3(T v) {
    mat1x3<T> ret;
    ret[0][0] = ret[0][1] = ret[0][2] = v;
    return ret;
}

template<typename T>
mat1x4<T> m1x4(const vec4<T>& v0) {
    mat1x4<T> ret; ret[0] = v0;
    return ret;
}
template<typename T>
mat1x4<T> m1x4(T x0, T y0, T z0, T w0) {
    mat1x4<T> ret;
    ret[0][0] = x0; ret[0][1] = y0; ret[0][2] = z0; ret[0][3] = w0;
    return ret;
}
template<typename T>
mat1x4<T> m1x4(T v) {
    mat1x4<T> ret;
    ret[0][0] = ret[0][1] = ret[0][2] = ret[0][3] = v;
    return ret;
}

template<typename T>
mat2x1<T> m2x1(const vec<1, T>& v0, const vec<1, T>& v1) {
    mat2x1<T> ret; ret[0] = v0; ret[1] = v1;
    return ret;
}
template<typename T>
mat2x1<T> m2x1(T x0, T x1) {
    mat2x1<T> ret;
    ret[0][0] = x0;
    ret[1][0] = x1;
    return ret;
}
template<typename T>
mat2x1<T> m2x1(T v) {
    mat2x1<T> ret;
    ret[0][0] = ret[1][0] = v;
    return ret;
}

template<typename T>
mat2x2<T> m2x2(const vec2<T>& v0, const vec2<T>& v1) {
    mat2x2<T> ret; ret[0] = v0; ret[1] = v1;
    return ret;
}
template<typename T>
mat2x2<T> m2x2(T x0, T y0, T x1, T y1) {
    mat2x2<T> ret;
    ret[0][0] = x0; ret[0][1] = y0;
    ret[1][0] = x1; ret[1][1] = y1;
    return ret;
}
template<typename T>
mat2x2<T> m2x2(T v) {
    mat2x2<T> ret;
    ret[0][0] = ret[0][1] = ret[1][0] = ret[1][1] = v;
    return ret;
}

template<typename T>
mat2x3<T> m2x3(const vec3<T>& v0, const vec3<T>& v1) {
    mat2x3 <T> ret; ret[0] = v0; ret[1] = v1;
    return ret;
}
template<typename T>
mat2x3<T> m2x3(T x0, T y0, T z0, T x1, T y1, T z1) {
    mat2x3<T> ret;
    ret[0][0] = x0; ret[0][1] = y0; ret[0][2] = z0;
    ret[1][0] = x1; ret[1][1] = y1; ret[1][2] = z1;
    return ret;
}
template<typename T>
mat2x3<T> m2x3(T v) {
    mat2x3<T> ret;
    ret[0][0] = ret[0][1] = ret[0][2] = ret[1][0] = ret[1][1] = ret[1][2] = v;
    return ret;
}

template<typename T>
mat2x4<T> m2x4(const vec4<T>& v0, const vec4<T>& v1) {
    mat2x4 <T> ret; ret[0] = v0; ret[1] = v1;
    return ret;
}
template<typename T>
mat2x4<T> m2x4(T x0, T y0, T z0, T w0, T x1, T y1, T z1, T w1) {
    mat2x4<T> ret;
    ret[0][0] = x0; ret[0][1] = y0; ret[0][2] = z0; ret[0][3] = w0;
    ret[1][0] = x1; ret[1][1] = y1; ret[1][2] = z1; ret[1][3] = w1;
    return ret;
}
template<typename T>
mat2x4<T> m2x4(T v) {
    mat2x4<T> ret;
    ret[0][0] = ret[0][1] = ret[0][2] = ret[0][3] = ret[1][0] = ret[1][1] = ret[1][2] = ret[1][3] = v;
    return ret;
}

template<typename T>
mat3x1<T> m3x1(const vec<1, T>& v0, const vec<1, T>& v1, const vec<1, T>& v2) {
    mat3x1<T> ret; ret[0] = v0; ret[1] = v1; ret[2] = v2;
    return ret;
}
template<typename T>
mat3x1<T> m3x1(T x0, T x1, T x2) {
    mat3x1<T> ret;
    ret[0][0] = x0;
    ret[1][0] = x1;
    ret[2][0] = x2;
    return ret;
}
template<typename T>
mat3x1<T> m3x1(T v) {
    mat3x1<T> ret;
    ret[0][0] = ret[1][0] = ret[2][0] = v;
    return ret;
}

template<typename T>
mat3x2<T> m3x2(const vec2<T>& v0, const vec2<T>& v1, const vec2<T>& v2) {
    mat3x2 <T> ret; ret[0] = v0; ret[1] = v1; ret[2] = v2;
    return ret;
}
template<typename T>
mat3x2<T> m3x2(T x0, T y0, T x1, T y1, T x2, T y2) {
    mat3x2<T> ret;
    ret[0][0] = x0; ret[0][1] = y0;
    ret[1][0] = x1; ret[1][1] = y1;
    ret[2][0] = x2; ret[2][1] = y2;
    return ret;
}
template<typename T>
mat3x2<T> m3x2(T v) {
    mat3x2<T> ret;
    ret[0][0] = ret[0][1] = ret[1][0] = ret[1][1] = ret[2][0] = ret[2][1] = v;
    return ret;
}

template<typename T>
mat3x3<T> m3x3(const vec3<T>& v0, const vec3<T>& v1, const vec3<T>& v2) {
    mat3x3 <T> ret; ret[0] = v0; ret[1] = v1; ret[2] = v2;
    return ret;
}
template<typename T>
mat3x3<T> m3x3(T x0, T y0, T z0, T x1, T y1, T z1, T x2, T y2, T z2) {
    mat3x3<T> ret;
    ret[0][0] = x0; ret[0][1] = y0; ret[0][2] = z0;
    ret[1][0] = x1; ret[1][1] = y1; ret[1][2] = z1;
    ret[2][0] = x2; ret[2][1] = y2; ret[2][2] = z2;
    return ret;
}
template<typename T>
mat3x3<T> m3x3(T v) {
    mat3x3<T> ret;
    ret[0][0] = ret[0][1] = ret[0][2] = ret[1][0] = ret[1][1] = ret[1][2] = ret[2][0] = ret[2][1] = ret[2][2] = v;
    return ret;
}

template<typename T>
mat3x4<T> m3x4(const vec4<T>& v0, const vec4<T>& v1, const vec4<T>& v2) {
    mat3x4 <T> ret; ret[0] = v0; ret[1] = v1; ret[2] = v2;
    return ret;
}
template<typename T>
mat3x4<T> m3x4(T x0, T y0, T z0, T w0, T x1, T y1, T z1, T w1, T x2, T y2, T z2, T w2) {
    mat3x4<T> ret;
    ret[0][0] = x0; ret[0][1] = y0; ret[0][2] = z0; ret[0][3] = w0;
    ret[1][0] = x1; ret[1][1] = y1; ret[1][2] = z1; ret[1][3] = w1;
    ret[2][0] = x2; ret[2][1] = y2; ret[2][2] = z2; ret[2][3] = w2;
    return ret;
}
template<typename T>
mat3x4<T> m3x4(T v) {
    mat3x4<T> ret;
    ret[0][0] = ret[0][1] = ret[0][2] = ret[0][3] = ret[1][0] = ret[1][1] = ret[1][2] = ret[1][3] = ret[2][0] = ret[2][1] = ret[2][2] = ret[2][3] = v;
    return ret;
}

template<typename T>
mat4x1<T> m4x1(const vec<1, T>& v0, const vec<1, T>& v1, const vec<1, T>& v2, const vec<1, T>& v3) {
    mat4x1 <T> ret; ret[0] = v0; ret[1] = v1; ret[2] = v2; ret[3] = v3;
    return ret;
}
template<typename T>
mat4x1<T> m4x1(T x0, T x1, T x2, T x3) {
    mat4x1<T> ret;
    ret[0][0] = x0;
    ret[1][0] = x1;
    ret[2][0] = x2;
    ret[3][0] = x3;
    return ret;
}
template<typename T>
mat4x1<T> m4x1(T v) {
    mat4x1<T> ret;
    ret[0][0] = ret[1][0] = ret[2][0] = ret[3][0] = v;
    return ret;
}

template<typename T>
mat4x2<T> m4x2(const vec2<T>& v0, const vec2<T>& v1, const vec2<T>& v2, const vec2<T>& v3) {
    mat4x2 <T> ret; ret[0] = v0; ret[1] = v1; ret[2] = v2; ret[3] = v3;
    return ret;
}
template<typename T>
mat4x2<T> m4x2(T x0, T y0, T x1, T y1, T x2, T y2, T x3, T y3) {
    mat4x2<T> ret;
    ret[0][0] = x0; ret[0][1] = y0;
    ret[1][0] = x1; ret[1][1] = y1;
    ret[2][0] = x2; ret[2][1] = y2;
    ret[3][0] = x3; ret[3][1] = y3;
    return ret;
}
template<typename T>
mat4x2<T> m4x2(T v) {
    mat4x2<T> ret;
    ret[0][0] = ret[0][1] = ret[1][0] = ret[1][1] = ret[2][0] = ret[2][1] = ret[3][0] = ret[3][1] = v;
    return ret;
}

template<typename T>
mat4x3<T> m4x3(const vec3<T>& v0, const vec3<T>& v1, const vec3<T>& v2, const vec3<T>& v3) {
    mat4x3 <T> ret; ret[0] = v0; ret[1] = v1; ret[2] = v2; ret[3] = v3;
    return ret;
}
template<typename T>
mat4x3<T> m4x3(T x0, T y0, T z0, T x1, T y1, T z1, T x2, T y2, T z2, T x3, T y3, T z3) {
    mat4x3<T> ret;
    ret[0][0] = x0; ret[0][1] = y0; ret[0][2] = z0;
    ret[1][0] = x1; ret[1][1] = y1; ret[1][2] = z1;
    ret[2][0] = x2; ret[2][1] = y2; ret[2][2] = z2;
    ret[3][0] = x3; ret[3][1] = y3; ret[3][2] = z3;
    return ret;
}
template<typename T>
mat4x3<T> m4x3(T v) {
    mat4x3<T> ret;
    ret[0][0] = ret[0][1] = ret[0][2] = ret[1][0] = ret[1][1] = ret[1][2] = ret[2][0] = ret[2][1] = ret[2][2] = ret[3][0] = ret[3][1] = ret[3][2] = v;
    return ret;
}

template<typename T>
mat4x4<T> m4x4(const vec4<T>& v0, const vec4<T>& v1, const vec4<T>& v2, const vec4<T>& v3) {
    mat4x4 <T> ret; ret[0] = v0; ret[1] = v1; ret[2] = v2; ret[3] = v3;
    return ret;
}
template<typename T>
mat4x4<T> m4x4(T x0, T y0, T z0, T w0, T x1, T y1, T z1, T w1, T x2, T y2, T z2, T w2, T x3, T y3, T z3, T w3) {
    mat4x4<T> ret;
    ret[0][0] = x0; ret[0][1] = y0; ret[0][2] = z0; ret[0][3] = w0;
    ret[1][0] = x1; ret[1][1] = y1; ret[1][2] = z1; ret[1][3] = w1;
    ret[2][0] = x2; ret[2][1] = y2; ret[2][2] = z2; ret[2][3] = w2;
    ret[3][0] = x3; ret[3][1] = y3; ret[3][2] = z3; ret[3][3] = w3;
    return ret;
}
template<typename T>
mat4x4<T> m4x4(T v) {
    mat4x4<T> ret;
    ret[0][0] = ret[0][1] = ret[0][2] = ret[0][3] = ret[1][0] = ret[1][1] = ret[1][2] = ret[1][3] = ret[2][0] = ret[2][1] = ret[2][2] = ret[2][3] = ret[3][0] = ret[3][1] = ret[3][2] = ret[3][3] = v;
    return ret;
}

using mat2x2b = mat2x2<bool>;
using mat2x2i = mat2x2<i32>;
using mat2x2u = mat2x2<u32>;
using mat2x2f = mat2x2<f32>;
using mat2x2d = mat2x2<f64>;

using mat2x3b = mat2x3<bool>;
using mat2x3i = mat2x3<i32>;
using mat2x3u = mat2x3<u32>;
using mat2x3f = mat2x3<f32>;
using mat2x3d = mat2x3<f64>;

using mat2x4b = mat2x4<bool>;
using mat2x4i = mat2x4<i32>;
using mat2x4u = mat2x4<u32>;
using mat2x4f = mat2x4<f32>;
using mat2x4d = mat2x4<f64>;

using mat3x2b = mat3x2<bool>;
using mat3x2i = mat3x2<i32>;
using mat3x2u = mat3x2<u32>;
using mat3x2f = mat3x2<f32>;
using mat3x2d = mat3x2<f64>;

using mat3x3b = mat3x3<bool>;
using mat3x3i = mat3x3<i32>;
using mat3x3u = mat3x3<u32>;
using mat3x3f = mat3x3<f32>;
using mat3x3d = mat3x3<f64>;

using mat3x4b = mat3x4<bool>;
using mat3x4i = mat3x4<i32>;
using mat3x4u = mat3x4<u32>;
using mat3x4f = mat3x4<f32>;
using mat3x4d = mat3x4<f64>;

using mat4x2b = mat4x2<bool>;
using mat4x2i = mat4x2<i32>;
using mat4x2u = mat4x2<u32>;
using mat4x2f = mat4x2<f32>;
using mat4x2d = mat4x2<f64>;

using mat4x3b = mat4x3<bool>;
using mat4x3i = mat4x3<i32>;
using mat4x3u = mat4x3<u32>;
using mat4x3f = mat4x3<f32>;
using mat4x3d = mat4x3<f64>;

using mat4x4b = mat4x4<bool>;
using mat4x4i = mat4x4<i32>;
using mat4x4u = mat4x4<u32>;
using mat4x4f = mat4x4<f32>;
using mat4x4d = mat4x4<f64>;

} // namespace core
