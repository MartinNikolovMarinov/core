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

template<i32 DimRow, i32 DimCol, typename TDst, typename TSrc>
constexpr void madd(mat<DimRow, DimCol, TDst>& dst, const TSrc& src) {
    if constexpr (std::is_arithmetic_v<TSrc>) {
        for (i32 i = 0; i < dst.dimensionsRows(); ++i) {
            vadd(dst[i], src);
        }
    } else {
        for (i32 i = 0; i < dst.dimensionsRows(); ++i) {
            vadd(dst[i], src[i]);
        }
    }
}

template<i32 DimRow, i32 DimCol, typename TDst, typename TSrc>
constexpr void msub(mat<DimRow, DimCol, TDst>& dst, const TSrc& src) {
    if constexpr (std::is_arithmetic_v<TSrc>) {
        for (i32 i = 0; i < dst.dimensionsRows(); ++i) {
            vsub(dst[i], src);
        }
    } else {
        for (i32 i = 0; i < dst.dimensionsRows(); ++i) {
            vsub(dst[i], src[i]);
        }
    }
}

template<i32 DimRow, i32 DimCol, typename T>
struct mat {
    using Type = T;
    static_assert(std::is_trivial_v<Type>, "Type must be a trivial");
    static_assert(std::is_arithmetic_v<Type>, "Type must be an arithmetic");
    static_assert(DimRow > 0, "DimRow must be greater than 0");
    static_assert(DimCol > 0, "DimCol must be greater than 0");

    static constexpr i32 dimensionsRows() { return DimRow; }
    static constexpr i32 dimensionsCols() { return DimCol; }

    using ColType = vec<DimCol, Type>;
    // using RowType = vec<DimRow, Type>; // TODO: do I need this ?
    using MatrixType = mat<DimRow, DimCol, Type>;
    using TransposeType = mat<DimCol, DimRow, Type>;

    ColType data[DimRow] = {};

    constexpr mat() = default;

#pragma region constructors

    // mat constructor for 1xN matrix:
    constexpr explicit mat(Type x0) {
        static_assert(DimRow == 1 && DimCol == 1, "Invalid dimensions for constructor");
        data[0] = ColType(x0);
    }
    constexpr mat(ColType v0) {
        static_assert(DimRow == 1, "Invalid dimensions for constructor");
        data[0] = v0;
    }

    // mat constructor for 2xN matrix:
    constexpr explicit mat(Type x0, Type y0, Type x1, Type y1) {
        static_assert(DimRow == 2 && DimCol == 2, "Invalid dimensions for constructor");
        data[0] = ColType(x0, y0);
        data[1] = ColType(x1, y1);
    }
    constexpr mat(ColType v0, ColType v1) {
        static_assert(DimRow == 2, "Invalid dimensions for constructor");
        data[0] = v0;
        data[1] = v1;
    }

    // mat constructor for 3xN matrix:
    constexpr explicit mat(Type x0, Type y0, Type z0,
                           Type x1, Type y1, Type z1,
                           Type x2, Type y2, Type z2) {
        static_assert(DimRow == 3 && DimCol == 3, "Invalid dimensions for constructor");
        data[0] = ColType(x0, y0, z0);
        data[1] = ColType(x1, y1, z1);
        data[2] = ColType(x2, y2, z2);
    }
    constexpr mat(ColType v0, ColType v1, ColType v2) {
        static_assert(DimRow == 3, "Invalid dimensions for constructor");
        data[0] = v0;
        data[1] = v1;
        data[2] = v2;
    }

    // mat constructor for 4xN matrix:
    constexpr explicit mat(Type x0, Type y0, Type z0, Type w0,
                           Type x1, Type y1, Type z1, Type w1,
                           Type x2, Type y2, Type z2, Type w2,
                           Type x3, Type y3, Type z3, Type w3) {
        static_assert(DimRow == 4 && DimCol == 4, "Invalid dimensions for constructor");
        data[0] = ColType(x0, y0, z0, w0);
        data[1] = ColType(x1, y1, z1, w1);
        data[2] = ColType(x2, y2, z2, w2);
        data[3] = ColType(x3, y3, z3, w3);
    }

    constexpr mat(core::vec<DimCol, Type> v0,
                  core::vec<DimCol, Type> v1,
                  core::vec<DimCol, Type> v2,
                  core::vec<DimCol, Type> v3) {
        static_assert(DimRow == 4, "Invalid dimensions for constructor");
        data[0] = v0;
        data[1] = v1;
        data[2] = v2;
        data[3] = v3;
    }

#pragma endregion

    constexpr ColType& operator[](i32 i) {
        Assert(i >= 0 && i < DimRow, "Index out of bounds");
        return data[i];
    }
    constexpr const ColType& operator[](i32 i) const {
        Assert(i >= 0 && i < DimRow, "Index out of bounds");
        return data[i];
    }

    constexpr bool equals(const MatrixType& other) const               { return mequals(*this, other); }
    constexpr bool equals(const MatrixType& other, Type epsilon) const { return msafeequals(*this, other, epsilon); }

    constexpr void negate() { mnegate(*this); }

    template<typename U>  constexpr void add(const mat<DimRow, DimCol, U>& other) { madd(*this, other); }
    template<typename TA> constexpr void add(TA v)                                { madd(*this, v); }
                          constexpr void inc()                                    { add(1); }

    template<typename U>  constexpr void sub(const mat<DimRow, DimCol, U>& other) { msub(*this, other); }
    template<typename TA> constexpr void sub(TA v)                                { msub(*this, v); }
                          constexpr void dec()                                    { sub(1); }

    // TODO: Write mul and div here...

#pragma region overloads "=="

    template<typename U>
    constexpr bool operator==(const MatrixType& other) const { return equals(other); }
    template<typename U>
    constexpr bool operator==(U v) const { return equals(MatrixType(v)); }

    template<typename U>
    constexpr bool operator!=(const MatrixType& other) const { return !equals(other); }
    template<typename U>
    constexpr bool operator!=(U v) const { return !equals(MatrixType(v)); }

#pragma endregion

#pragma region overloads negation

    constexpr MatrixType operator-() const {
        MatrixType ret = *this;
        ret.negate();
        return ret;
    }

#pragma endregion

#pragma region overloads increment and decrement

    constexpr MatrixType& operator++() { inc(); return *this; }
    constexpr MatrixType& operator--() { dec(); return *this; }

    constexpr MatrixType operator++(int) { MatrixType tmp(*this); inc(); return tmp; }
    constexpr MatrixType operator--(int) { MatrixType tmp(*this); dec(); return tmp; }

#pragma endregion

#pragma region overloads assignment operators

    constexpr MatrixType& operator+=(const MatrixType& other) { add(other); return *this; }
    constexpr MatrixType& operator+=(Type v)                  { add(v); return *this; }
    constexpr MatrixType& operator-=(const MatrixType& other) { sub(other); return *this; }
    constexpr MatrixType& operator-=(Type v)                  { sub(v); return *this; }

#pragma endregion

#pragma region overloads "+"

    friend constexpr MatrixType operator+(const MatrixType& v1, const MatrixType& v2) {
        MatrixType ret = v1;
        ret.add(v2);
        return ret;
    }
    template<typename U>
    friend constexpr MatrixType operator+(const MatrixType& v1, U v2) {
        MatrixType ret = v1;
        ret.add(v2);
        return ret;
    }
    template<typename U>
    friend constexpr MatrixType operator+(U v1, const MatrixType& v2) {
        MatrixType ret = v2;
        ret.add(v1);
        return ret;
    }

#pragma endregion

#pragma region overloads "-"

    friend constexpr MatrixType operator-(const MatrixType& v1, const MatrixType& v2) {
        MatrixType ret = v1;
        ret.sub(v2);
        return ret;
    }
    template<typename U>
    friend constexpr MatrixType operator-(const MatrixType& v1, U v2) {
        MatrixType ret = v1;
        ret.sub(v2);
        return ret;
    }
    template<typename U>
    friend constexpr MatrixType operator-(U v1, const MatrixType& v2) {
        MatrixType ret = v2;
        for (i32 i = 0; i < ret.dimensions(); ++i) {
            ret[i] = -(ret[i] - static_cast<T>(v1));
        }
        return ret;
    }

#pragma endregion

};

template<typename T> using mat2x2 = mat<2, 2, T>;
template<typename T> using mat2x3 = mat<2, 3, T>;
template<typename T> using mat2x4 = mat<2, 4, T>;
template<typename T> using mat3x2 = mat<3, 2, T>;
template<typename T> using mat3x3 = mat<3, 3, T>;
template<typename T> using mat3x4 = mat<3, 4, T>;
template<typename T> using mat4x2 = mat<4, 2, T>;
template<typename T> using mat4x3 = mat<4, 3, T>;
template<typename T> using mat4x4 = mat<4, 4, T>;

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
