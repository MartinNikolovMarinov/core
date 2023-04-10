#include <types.h>
#include <std/vec.h>

#include <type_traits>

using namespace coretypes;

template<i32 NCol, i32 NRow, typename T> struct mat;

template<typename T>
struct mat<2, 2, T> {
    static constexpr i32 NCol = 2;
    static constexpr i32 NRow = 2;
    using DataType = T;
    using MatrixType = mat<NCol, NRow, T>;

    static_assert(std::is_trivial_v<DataType>, "DataType must be a trivial");
    static_assert(std::is_arithmetic_v<DataType>, "DataType must be an arithmetic");

    static constexpr MatrixType identity() { return MatrixType(1, 0, 0, 1); }

    DataType data[NCol][NRow] = {};

    constexpr explicit mat() = default;
    constexpr explicit mat(DataType uniform) : data{{uniform, uniform}, {uniform, uniform}} {}
    constexpr explicit mat(DataType x0, DataType y0, DataType x1, DataType y1)
        : data{{x0, y0}, {x1, y1}} {}
    constexpr explicit mat(core::vec2<DataType> col1, core::vec2<DataType> col2)
        : data{{col1.x, col1.y}, {col2.x, col2.y}} {}

    constexpr DataType* operator[](i32 i) { return data[i]; }
    constexpr const DataType* operator[](i32 i) const { return data[i]; }
};

template<typename T>
struct mat<2, 3, T> {
    static constexpr i32 NCol = 2;
    static constexpr i32 NRow = 3;
    using DataType = T;
    using MatrixType = mat<NCol, NRow, T>;

    static_assert(std::is_trivial_v<DataType>, "DataType must be a trivial");
    static_assert(std::is_arithmetic_v<DataType>, "DataType must be an arithmetic");

    DataType data[NCol][NRow] = {};

    constexpr explicit mat() = default;
    constexpr explicit mat(DataType uniform) : data{{uniform, uniform, uniform}, {uniform, uniform, uniform}} {}
    constexpr explicit mat(DataType x0, DataType y0, DataType z0, DataType x1, DataType y1, DataType z1)
        : data{{x0, y0, z0}, {x1, y1, z1}} {}
    constexpr explicit mat(core::vec2<DataType> col1, core::vec2<DataType> col2, core::vec2<DataType> col3)
        : data{{col1.x, col1.y, col1.z}, {col2.x, col2.y, col2.z}, {col3.x, col3.y, col3.z}} {}

    constexpr DataType* operator[](i32 i) { return data[i]; }
    constexpr const DataType* operator[](i32 i) const { return data[i]; }
};

template<typename T>
struct mat<2, 4, T> {
    static constexpr i32 NCol = 2;
    static constexpr i32 NRow = 4;
    using DataType = T;
    using MatrixType = mat<NCol, NRow, T>;

    static_assert(std::is_trivial_v<DataType>, "DataType must be a trivial");
    static_assert(std::is_arithmetic_v<DataType>, "DataType must be an arithmetic");

    DataType data[NCol][NRow] = {};

    constexpr explicit mat() = default;
    constexpr explicit mat(DataType uniform) : data{{uniform, uniform, uniform, uniform}, {uniform, uniform, uniform, uniform}} {}
    constexpr explicit mat(DataType x0, DataType y0, DataType z0, DataType w0, DataType x1, DataType y1, DataType z1, DataType w1)
        : data{{x0, y0, z0, w0}, {x1, y1, z1, w1}} {}
    constexpr explicit mat(core::vec2<DataType> col1, core::vec2<DataType> col2, core::vec2<DataType> col3, core::vec2<DataType> col4)
        : data{{col1.x, col1.y, col1.z, col1.w}, {col2.x, col2.y, col2.z, col2.w}, {col3.x, col3.y, col3.z, col3.w}, {col4.x, col4.y, col4.z, col4.w}} {}

    constexpr DataType* operator[](i32 i) { return data[i]; }
    constexpr const DataType* operator[](i32 i) const { return data[i]; }
};

template<typename T>
struct mat<3, 2, T> {
    static constexpr i32 NCol = 3;
    static constexpr i32 NRow = 2;
    using DataType = T;
    using MatrixType = mat<NCol, NRow, T>;

    static_assert(std::is_trivial_v<DataType>, "DataType must be a trivial");
    static_assert(std::is_arithmetic_v<DataType>, "DataType must be an arithmetic");

    DataType data[NCol][NRow] = {};

    constexpr explicit mat() = default;
    constexpr explicit mat(DataType uniform) : data{{uniform, uniform}, {uniform, uniform}, {uniform, uniform}} {}
    constexpr explicit mat(DataType x0, DataType y0, DataType x1, DataType y1, DataType x2, DataType y2)
        : data{{x0, y0}, {x1, y1}, {x2, y2}} {}
    constexpr explicit mat(core::vec3<DataType> col1, core::vec3<DataType> col2, core::vec3<DataType> col3)
        : data{{col1.x, col1.y}, {col2.x, col2.y}, {col3.x, col3.y}} {}

    constexpr DataType* operator[](i32 i) { return data[i]; }
    constexpr const DataType* operator[](i32 i) const { return data[i]; }
};

template<typename T>
struct mat<3, 3, T> {
    static constexpr i32 NCol = 3;
    static constexpr i32 NRow = 3;
    using DataType = T;
    using MatrixType = mat<NCol, NRow, T>;

    static_assert(std::is_trivial_v<DataType>, "DataType must be a trivial");
    static_assert(std::is_arithmetic_v<DataType>, "DataType must be an arithmetic");

    static constexpr MatrixType identity() { return MatrixType(1, 0, 0, 0, 1, 0, 0, 0, 1); }

    DataType data[NCol][NRow] = {};

    constexpr explicit mat() = default;
    constexpr explicit mat(DataType uniform) : data{{uniform, uniform, uniform}, {uniform, uniform, uniform}, {uniform, uniform, uniform}} {}
    constexpr explicit mat(DataType x0, DataType y0, DataType z0, DataType x1, DataType y1, DataType z1, DataType x2, DataType y2, DataType z2)
        : data{{x0, y0, z0}, {x1, y1, z1}, {x2, y2, z2}} {}
    constexpr explicit mat(core::vec3<DataType> col1, core::vec3<DataType> col2, core::vec3<DataType> col3)
        : data{{col1.x, col1.y, col1.z}, {col2.x, col2.y, col2.z}, {col3.x, col3.y, col3.z}} {}

    constexpr DataType* operator[](i32 i) { return data[i]; }
    constexpr const DataType* operator[](i32 i) const { return data[i]; }
};

template<typename T>
struct mat<3, 4, T> {
    static constexpr i32 NCol = 3;
    static constexpr i32 NRow = 4;
    using DataType = T;
    using MatrixType = mat<NCol, NRow, T>;

    static_assert(std::is_trivial_v<DataType>, "DataType must be a trivial");
    static_assert(std::is_arithmetic_v<DataType>, "DataType must be an arithmetic");

    DataType data[NCol][NRow] = {};

    constexpr explicit mat() = default;
    constexpr explicit mat(DataType uniform) : data{{uniform, uniform, uniform, uniform}, {uniform, uniform, uniform, uniform}, {uniform, uniform, uniform, uniform}} {}
    constexpr explicit mat(DataType x0, DataType y0, DataType z0, DataType w0, DataType x1, DataType y1, DataType z1, DataType w1, DataType x2, DataType y2, DataType z2, DataType w2)
        : data{{x0, y0, z0, w0}, {x1, y1, z1, w1}, {x2, y2, z2, w2}} {}
    constexpr explicit mat(core::vec3<DataType> col1, core::vec3<DataType> col2, core::vec3<DataType> col3, core::vec3<DataType> col4)
        : data{{col1.x, col1.y, col1.z, col1.w}, {col2.x, col2.y, col2.z, col2.w}, {col3.x, col3.y, col3.z, col3.w}, {col4.x, col4.y, col4.z, col4.w}} {}

    constexpr DataType* operator[](i32 i) { return data[i]; }
    constexpr const DataType* operator[](i32 i) const { return data[i]; }
};

template<typename T>
struct mat<4, 2, T> {
    static constexpr i32 NCol = 4;
    static constexpr i32 NRow = 2;
    using DataType = T;
    using MatrixType = mat<NCol, NRow, T>;

    static_assert(std::is_trivial_v<DataType>, "DataType must be a trivial");
    static_assert(std::is_arithmetic_v<DataType>, "DataType must be an arithmetic");

    DataType data[NCol][NRow] = {};

    constexpr explicit mat() = default;
    constexpr explicit mat(DataType uniform) : data{{uniform, uniform}, {uniform, uniform}, {uniform, uniform}, {uniform, uniform}} {}
    constexpr explicit mat(DataType x0, DataType y0, DataType x1, DataType y1, DataType x2, DataType y2, DataType x3, DataType y3)
        : data{{x0, y0}, {x1, y1}, {x2, y2}, {x3, y3}} {}
    constexpr explicit mat(core::vec4<DataType> col1, core::vec4<DataType> col2, core::vec4<DataType> col3, core::vec4<DataType> col4)
        : data{{col1.x, col1.y}, {col2.x, col2.y}, {col3.x, col3.y}, {col4.x, col4.y}} {}

    constexpr DataType* operator[](i32 i) { return data[i]; }
    constexpr const DataType* operator[](i32 i) const { return data[i]; }
};

template<typename T>
struct mat<4, 3, T> {
    static constexpr i32 NCol = 4;
    static constexpr i32 NRow = 3;
    using DataType = T;
    using MatrixType = mat<NCol, NRow, T>;

    static_assert(std::is_trivial_v<DataType>, "DataType must be a trivial");
    static_assert(std::is_arithmetic_v<DataType>, "DataType must be an arithmetic");

    DataType data[NCol][NRow] = {};

    constexpr explicit mat() = default;
    constexpr explicit mat(DataType uniform) : data{{uniform, uniform, uniform}, {uniform, uniform, uniform}, {uniform, uniform, uniform}, {uniform, uniform, uniform}} {}
    constexpr explicit mat(DataType x0, DataType y0, DataType z0, DataType x1, DataType y1, DataType z1, DataType x2, DataType y2, DataType z2, DataType x3, DataType y3, DataType z3)
        : data{{x0, y0, z0}, {x1, y1, z1}, {x2, y2, z2}, {x3, y3, z3}} {}
    constexpr explicit mat(core::vec4<DataType> col1, core::vec4<DataType> col2, core::vec4<DataType> col3, core::vec4<DataType> col4)
        : data{{col1.x, col1.y, col1.z}, {col2.x, col2.y, col2.z}, {col3.x, col3.y, col3.z}, {col4.x, col4.y, col4.z}} {}

    constexpr DataType* operator[](i32 i) { return data[i]; }
    constexpr const DataType* operator[](i32 i) const { return data[i]; }
};

template<typename T>
struct mat<4, 4, T> {
    static constexpr i32 NCol = 4;
    static constexpr i32 NRow = 4;
    using DataType = T;
    using MatrixType = mat<NCol, NRow, T>;

    static_assert(std::is_trivial_v<DataType>, "DataType must be a trivial");
    static_assert(std::is_arithmetic_v<DataType>, "DataType must be an arithmetic");

    static constexpr MatrixType identity() { return MatrixType(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1); }

    DataType data[NCol][NRow] = {};

    constexpr explicit mat() = default;
    constexpr explicit mat(DataType uniform) : data{{uniform, uniform, uniform, uniform}, {uniform, uniform, uniform, uniform}, {uniform, uniform, uniform, uniform}, {uniform, uniform, uniform, uniform}} {}
    constexpr explicit mat(DataType x0, DataType y0, DataType z0, DataType w0, DataType x1, DataType y1, DataType z1, DataType w1, DataType x2, DataType y2, DataType z2, DataType w2, DataType x3, DataType y3, DataType z3, DataType w3)
        : data{{x0, y0, z0, w0}, {x1, y1, z1, w1}, {x2, y2, z2, w2}, {x3, y3, z3, w3}} {}
    constexpr explicit mat(core::vec4<DataType> col1, core::vec4<DataType> col2, core::vec4<DataType> col3, core::vec4<DataType> col4)
        : data{{col1.x, col1.y, col1.z, col1.w}, {col2.x, col2.y, col2.z, col2.w}, {col3.x, col3.y, col3.z, col3.w}, {col4.x, col4.y, col4.z, col4.w}} {}

    constexpr DataType* operator[](i32 i) { return data[i]; }
    constexpr const DataType* operator[](i32 i) const { return data[i]; }
};

#pragma region Static Functions

// Equality

template<i32 NCol, i32 NRow, typename T>
constexpr bool mequals(const mat<NCol, NRow, T>& m1, const mat<NCol, NCol, T>& m2) {
    for (i32 i = 0; i < m1.NCol; ++i) {
        for (i32 j = 0; j < m1.NRow; ++j) {
            if (m1[i][j] != m2[i][j]) return false;
        }
    }
    return true;
}
template<i32 NCol, i32 NRow, typename T>
constexpr bool msafeequals(const mat<NCol, NRow, T>& m1, const mat<NCol, NRow, T>& m2, T epsilon) {
    for (i32 i = 0; i < m1.NCol; ++i) {
        for (i32 j = 0; j < m1.NRow; ++j) {
            if (!core::safe_eq(m1[i][j], m2[i][j], epsilon)) return false;
        }
    }
    return true;
}

// Negation

template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T> mnegate(const mat<NCol, NRow, T>& m) {
    mat<NCol, NRow, T> res;
    for (i32 i = 0; i < m.NCol; ++i) {
        for (i32 j = 0; j < m.NRow; ++j) {
            res[i][j] = -m[i][j];
        }
    }
    return res;
}

// Addition

template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T> madd(const mat<NCol, NRow, T>& m1, const mat<NCol, NRow, T>& m2) {
    mat<NCol, NRow, T> res;
    for (i32 i = 0; i < m1.NCol; ++i) {
        for (i32 j = 0; j < m1.NRow; ++j) {
            res[i][j] = m1[i][j] + m2[i][j];
        }
    }
    return res;
}
template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T> madd(const mat<NCol, NRow, T>& m, T value) {
    mat<NCol, NRow, T> res;
    for (i32 i = 0; i < m.NCol; ++i) {
        for (i32 j = 0; j < m.NRow; ++j) {
            res[i][j] = m[i][j] + value;
        }
    }
    return res;
}

// Subtraction

template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T> msub(const mat<NCol, NRow, T>& m1, const mat<NCol, NRow, T>& m2) {
    mat<NCol, NRow, T> res;
    for (i32 i = 0; i < m1.NCol; ++i) {
        for (i32 j = 0; j < m1.NRow; ++j) {
            res[i][j] = m1[i][j] - m2[i][j];
        }
    }
    return res;
}
template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T> msub(const mat<NCol, NRow, T>& m, T value) {
    mat<NCol, NRow, T> res;
    for (i32 i = 0; i < m.NCol; ++i) {
        for (i32 j = 0; j < m.NRow; ++j) {
            res[i][j] = m[i][j] - value;
        }
    }
    return res;
}

// Multiplication

template<i32 NCol1, i32 NRow1, i32 NCol2, i32 NRow2, typename T>
constexpr mat<NCol2, NRow1, T> mmul(const mat<NCol1, NRow1, T>& m1, const mat<NCol2, NRow2, T>& m2) {
    static_assert(NCol1 == NRow2, "Matrices must be compatible");
    mat<NCol2, NRow1, T> res;
    for (i32 i = 0; i < res.NCol; ++i) {
        for (i32 j = 0; j < res.NRow; ++j) {
            T sum = 0;
            for (i32 k = 0; k < NCol1; ++k) {
                sum += m1[k][j] * m2[i][k];
            }
            res[i][j] = sum;
        }
    }
    return res;
}
template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T> mmul(const mat<NCol, NRow, T>& m, T value) {
    mat<NCol, NRow, T> res;
    for (i32 i = 0; i < m.NCol; ++i) {
        for (i32 j = 0; j < m.NRow; ++j) {
            res[i][j] = m[i][j] * value;
        }
    }
    return res;
}

// Determinant

template<i32 NDim, typename T>
constexpr T det(const mat<NDim, NDim, T>& m) {
    if constexpr (NDim == 2) {
        return m[0][0] * m[1][1] - m[1][0] * m[0][1];
    }
    else if constexpr (NDim == 3) {
        return + m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2])
               - m[1][0] * (m[0][1] * m[2][2] - m[2][1] * m[0][2])
               + m[2][0] * (m[0][1] * m[1][2] - m[1][1] * m[0][2]);
    }
    else if constexpr (NDim == 4) {
        return + m[0][0] * (m[1][1] * (m[2][2] * m[3][3] - m[3][2] * m[2][3]) - m[2][1] * (m[1][2] * m[3][3] - m[3][2] * m[1][3]) + m[3][1] * (m[1][2] * m[2][3] - m[2][2] * m[1][3]))
               - m[1][0] * (m[0][1] * (m[2][2] * m[3][3] - m[3][2] * m[2][3]) - m[2][1] * (m[0][2] * m[3][3] - m[3][2] * m[0][3]) + m[3][1] * (m[0][2] * m[2][3] - m[2][2] * m[0][3]))
               + m[2][0] * (m[0][1] * (m[1][2] * m[3][3] - m[3][2] * m[1][3]) - m[1][1] * (m[0][2] * m[3][3] - m[3][2] * m[0][3]) + m[3][1] * (m[0][2] * m[1][3] - m[1][2] * m[0][3]))
               - m[3][0] * (m[0][1] * (m[1][2] * m[2][3] - m[2][2] * m[1][3]) - m[1][1] * (m[0][2] * m[2][3] - m[2][2] * m[0][3]) + m[2][1] * (m[0][2] * m[1][3] - m[1][2] * m[0][3]));
    }
    else {
        static_assert(core::AlwaysFalse<T>, "Matrix must be 2x2, 3x3 or 4x4");
    }
}

// Transpose

template<i32 NCol, i32 NRow, typename T>
constexpr mat<NRow, NCol, T> mtranspose(const mat<NCol, NRow, T>& m) {
    mat<NRow, NCol, T> res;
    for (i32 i = 0; i < m.NCol; ++i) {
        for (i32 j = 0; j < m.NRow; ++j) {
            res[j][i] = m[i][j];
        }
    }
    return res;
}

// Inverse

template<i32 NDim, typename T>
constexpr mat<NDim, NDim, T> minverse(const mat<NDim, NDim, T>& m) {
    mat<NDim, NDim, T> ret { T(INFINITY) };
    T d = det(m);
    if (d == 0) return ret;
    T idet = 1 / d;
    if constexpr (NDim == 2) {
        ret[0][0] = m[1][1] * idet;
        ret[0][1] = -m[0][1] * idet;
        ret[1][0] = -m[1][0] * idet;
        ret[1][1] = m[0][0] * idet;
    }
    else if constexpr (NDim == 3) {
        ret[0][0] = (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * idet;
        ret[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * idet;
        ret[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * idet;
        ret[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * idet;
        ret[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * idet;
        ret[1][2] = (m[1][0] * m[0][2] - m[0][0] * m[1][2]) * idet;
        ret[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * idet;
        ret[2][1] = (m[2][0] * m[0][1] - m[0][0] * m[2][1]) * idet;
        ret[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * idet;
    }
    else if constexpr (NDim == 4) {
        ret[0][0] = m[1][1] * m[2][2] * m[3][3] - m[1][1] * m[2][3] * m[3][2] - m[2][1] * m[1][2] * m[3][3]
                    + m[2][1] * m[1][3] * m[3][2] + m[3][1] * m[1][2] * m[2][3] - m[3][1] * m[1][3] * m[2][2];
        ret[0][1] = -m[0][1] * m[2][2] * m[3][3] + m[0][1] * m[2][3] * m[3][2] + m[2][1] * m[0][2] * m[3][3]
                    - m[2][1] * m[0][3] * m[3][2] - m[3][1] * m[0][2] * m[2][3] + m[3][1] * m[0][3] * m[2][2];
        ret[0][2] = m[0][1] * m[1][2] * m[3][3] - m[0][1] * m[1][3] * m[3][2] - m[1][1] * m[0][2] * m[3][3]
                    + m[1][1] * m[0][3] * m[3][2] + m[3][1] * m[0][2] * m[1][3] - m[3][1] * m[0][3] * m[1][2];
        ret[0][3] = -m[0][1] * m[1][2] * m[2][3] + m[0][1] * m[1][3] * m[2][2] + m[1][1] * m[0][2] * m[2][3]
                    - m[1][1] * m[0][3] * m[2][2] - m[2][1] * m[0][2] * m[1][3] + m[2][1] * m[0][3] * m[1][2];
        ret[1][0] = -m[1][0] * m[2][2] * m[3][3] + m[1][0] * m[2][3] * m[3][2] + m[2][0] * m[1][2] * m[3][3]
                    - m[2][0] * m[1][3] * m[3][2] - m[3][0] * m[1][2] * m[2][3] + m[3][0] * m[1][3] * m[2][2];
        ret[1][1] = m[0][0] * m[2][2] * m[3][3] - m[0][0] * m[2][3] * m[3][2] - m[2][0] * m[0][2] * m[3][3]
                    + m[2][0] * m[0][3] * m[3][2] + m[3][0] * m[0][2] * m[2][3] - m[3][0] * m[0][3] * m[2][2];
        ret[1][2] = -m[0][0] * m[1][2] * m[3][3] + m[0][0] * m[1][3] * m[3][2] + m[1][0] * m[0][2] * m[3][3]
                    - m[1][0] * m[0][3] * m[3][2] - m[3][0] * m[0][2] * m[1][3] + m[3][0] * m[0][3] * m[1][2];
        ret[1][3] = m[0][0] * m[1][2] * m[2][3] - m[0][0] * m[1][3] * m[2][2] - m[1][0] * m[0][2] * m[2][3]
                    + m[1][0] * m[0][3] * m[2][2] + m[2][0] * m[0][2] * m[1][3] - m[2][0] * m[0][3] * m[1][2];
        ret[2][0] = m[1][0] * m[2][1] * m[3][3] - m[1][0] * m[2][3] * m[3][1] - m[2][0] * m[1][1] * m[3][3]
                    + m[2][0] * m[1][3] * m[3][1] + m[3][0] * m[1][1] * m[2][3] - m[3][0] * m[1][3] * m[2][1];
        ret[2][1] = -m[0][0] * m[2][1] * m[3][3] + m[0][0] * m[2][3] * m[3][1] + m[2][0] * m[0][1] * m[3][3]
                    - m[2][0] * m[0][3] * m[3][1] - m[3][0] * m[0][1] * m[2][3] + m[3][0] * m[0][3] * m[2][1];
        ret[2][2] = m[0][0] * m[1][1] * m[3][3] - m[0][0] * m[1][3] * m[3][1] - m[1][0] * m[0][1] * m[3][3]
                    + m[1][0] * m[0][3] * m[3][1] + m[3][0] * m[0][1] * m[1][3] - m[3][0] * m[0][3] * m[1][1];
        ret[2][3] = -m[0][0] * m[1][1] * m[2][3] + m[0][0] * m[1][3] * m[2][1] + m[1][0] * m[0][1] * m[2][3]
                    - m[1][0] * m[0][3] * m[2][1] - m[2][0] * m[0][1] * m[1][3] + m[2][0] * m[0][3] * m[1][1];
        ret[3][0] = -m[1][0] * m[2][1] * m[3][2] + m[1][0] * m[2][2] * m[3][1] + m[2][0] * m[1][1] * m[3][2]
                    - m[2][0] * m[1][2] * m[3][1] - m[3][0] * m[1][1] * m[2][2] + m[3][0] * m[1][2] * m[2][1];
        ret[3][1] = m[0][0] * m[2][1] * m[3][2] - m[0][0] * m[2][2] * m[3][1] - m[2][0] * m[0][1] * m[3][2]
                    + m[2][0] * m[0][2] * m[3][1] + m[3][0] * m[0][1] * m[2][2] - m[3][0] * m[0][2] * m[2][1];
        ret[3][2] = -m[0][0] * m[1][1] * m[3][2] + m[0][0] * m[1][2] * m[3][1] + m[1][0] * m[0][1] * m[3][2]
                    - m[1][0] * m[0][2] * m[3][1] - m[3][0] * m[0][1] * m[1][2] + m[3][0] * m[0][2] * m[1][1];
        ret[3][3] = m[0][0] * m[1][1] * m[2][2] - m[0][0] * m[1][2] * m[2][1] - m[1][0] * m[0][1] * m[2][2]
                    + m[1][0] * m[0][2] * m[2][1] + m[2][0] * m[0][1] * m[1][2] - m[2][0] * m[0][2] * m[1][1];
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                ret[i][j] *= idet;
            }
        }
    }
    else {
        static_assert(core::AlwaysFalse<T>, "Matrix must be 2x2, 3x3 or 4x4");
    }
    return ret;
}

#pragma endregion

#pragma region Operators

// Negation

template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T> operator-(const mat<NCol, NRow, T>& m) {
    return mnegate(m);
}

// Equality

template<i32 NCol, i32 NRow, typename T>
constexpr bool operator==(const mat<NCol, NRow, T>& m1, const mat<NCol, NRow, T>& m2) {
    return mequals(m1, m2);
}
template<i32 NCol, i32 NRow, typename T>
constexpr bool operator!=(const mat<NCol, NRow, T>& m1, const mat<NCol, NRow, T>& m2) {
    return !mequals(m1, m2);
}

// Addition

template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T> operator+(const mat<NCol, NRow, T>& m1, const mat<NCol, NRow, T>& m2) {
    return madd(m1, m2);
}
template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T> operator+(const mat<NCol, NRow, T>& m, T value) {
    return madd(m, value);
}
template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T> operator+(T value, const mat<NCol, NRow, T>& m) {
    return madd(m, value);
}

// Subtraction

template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T> operator-(const mat<NCol, NRow, T>& m1, const mat<NCol, NRow, T>& m2) {
    return msub(m1, m2);
}
template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T> operator-(const mat<NCol, NRow, T>& m, T value) {
    return msub(m, value);
}
template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T> operator-(T value, const mat<NCol, NRow, T>& m) {
    return msub(m, value);
}

// Multiplication

// 2x2 * 2x2 = 2x2
template<typename T>
constexpr mat<2, 2, T> operator*(const mat<2, 2, T>& m1, const mat<2, 2, T>& m2) { return mmul(m1, m2); }
// 2x2 * 3x2 = 3x2
template<typename T>
constexpr mat<3, 2, T> operator*(const mat<2, 2, T>& m1, const mat<3, 2, T>& m2) { return mmul(m1, m2); }
// 2x2 * 4x2 = 4x2

template<typename T>
constexpr mat<4, 2, T> operator*(const mat<2, 2, T>& m1, const mat<4, 2, T>& m2) { return mmul(m1, m2); }
// 2x3 * 2x2 = 2x3
template<typename T>
constexpr mat<2, 3, T> operator*(const mat<2, 3, T>& m1, const mat<2, 2, T>& m2) { return mmul(m1, m2); }
// 2x3 * 3x2 = 3x3
template<typename T>
constexpr mat<3, 3, T> operator*(const mat<2, 3, T>& m1, const mat<3, 2, T>& m2) { return mmul(m1, m2); }
// 2x3 * 4x2 = 4x3
template<typename T>
constexpr mat<4, 3, T> operator*(const mat<2, 3, T>& m1, const mat<4, 2, T>& m2) { return mmul(m1, m2); }

// 2x4 * 2x2 = 2x4
template<typename T>
constexpr mat<2, 4, T> operator*(const mat<2, 4, T>& m1, const mat<2, 2, T>& m2) { return mmul(m1, m2); }
// 2x4 * 3x2 = 3x4
template<typename T>
constexpr mat<3, 4, T> operator*(const mat<2, 4, T>& m1, const mat<3, 2, T>& m2) { return mmul(m1, m2); }
// 2x4 * 4x2 = 4x4
template<typename T>
constexpr mat<4, 4, T> operator*(const mat<2, 4, T>& m1, const mat<4, 2, T>& m2) { return mmul(m1, m2); }

// 3x2 * 2x3 = 2x2
template<typename T>
constexpr mat<2, 2, T> operator*(const mat<3, 2, T>& m1, const mat<2, 3, T>& m2) { return mmul(m1, m2); }
// 3x2 * 3x3 = 3x2
template<typename T>
constexpr mat<3, 2, T> operator*(const mat<3, 2, T>& m1, const mat<3, 3, T>& m2) { return mmul(m1, m2); }
// 3x2 * 4x3 = 4x2
template<typename T>
constexpr mat<4, 2, T> operator*(const mat<3, 2, T>& m1, const mat<4, 3, T>& m2) { return mmul(m1, m2); }

// 3x3 * 2x3 = 2x3
template<typename T>
constexpr mat<2, 3, T> operator*(const mat<3, 3, T>& m1, const mat<2, 3, T>& m2) { return mmul(m1, m2); }
// 3x3 * 3x3 = 3x3
template<typename T>
constexpr mat<3, 3, T> operator*(const mat<3, 3, T>& m1, const mat<3, 3, T>& m2) { return mmul(m1, m2); }
// 3x3 * 4x3 = 4x3
template<typename T>
constexpr mat<4, 3, T> operator*(const mat<3, 3, T>& m1, const mat<4, 3, T>& m2) { return mmul(m1, m2); }

// 3x4 * 2x3 = 2x4
template<typename T>
constexpr mat<2, 4, T> operator*(const mat<3, 4, T>& m1, const mat<2, 3, T>& m2) { return mmul(m1, m2); }
// 3x4 * 3x3 = 3x4
template<typename T>
constexpr mat<3, 4, T> operator*(const mat<3, 4, T>& m1, const mat<3, 3, T>& m2) { return mmul(m1, m2); }
// 3x4 * 4x3 = 4x4
template<typename T>
constexpr mat<4, 4, T> operator*(const mat<3, 4, T>& m1, const mat<4, 3, T>& m2) { return mmul(m1, m2); }

// 4x2 * 2x4 = 2x2
template<typename T>
constexpr mat<2, 2, T> operator*(const mat<4, 2, T>& m1, const mat<2, 4, T>& m2) { return mmul(m1, m2); }
// 4x2 * 3x4 = 3x2
template<typename T>
constexpr mat<3, 2, T> operator*(const mat<4, 2, T>& m1, const mat<3, 4, T>& m2) { return mmul(m1, m2); }
// 4x2 * 4x4 = 4x2
template<typename T>
constexpr mat<4, 2, T> operator*(const mat<4, 2, T>& m1, const mat<4, 4, T>& m2) { return mmul(m1, m2); }

// 4x3 * 2x4 = 2x3
template<typename T>
constexpr mat<2, 3, T> operator*(const mat<4, 3, T>& m1, const mat<2, 4, T>& m2) { return mmul(m1, m2); }
// 4x3 * 3x4 = 3x3
template<typename T>
constexpr mat<3, 3, T> operator*(const mat<4, 3, T>& m1, const mat<3, 4, T>& m2) { return mmul(m1, m2); }
// 4x3 * 4x4 = 4x3
template<typename T>
constexpr mat<4, 3, T> operator*(const mat<4, 3, T>& m1, const mat<4, 4, T>& m2) { return mmul(m1, m2); }

// 4x4 * 2x4 = 2x4
template<typename T>
constexpr mat<2, 4, T> operator*(const mat<4, 4, T>& m1, const mat<2, 4, T>& m2) { return mmul(m1, m2); }
// 4x4 * 3x4 = 3x4
template<typename T>
constexpr mat<3, 4, T> operator*(const mat<4, 4, T>& m1, const mat<3, 4, T>& m2) { return mmul(m1, m2); }
// 4x4 * 4x4 = 4x4
template<typename T>
constexpr mat<4, 4, T> operator*(const mat<4, 4, T>& m1, const mat<4, 4, T>& m2) { return mmul(m1, m2); }

template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T> operator*(const mat<NCol, NRow, T>& m, typename mat<NCol, NRow, T>::DataType value) {
    return mmul(m, value);
}
template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T> operator*(typename mat<NCol, NRow, T>::DataType value, const mat<NCol, NRow, T>& m) {
    return mmul(m, value);
}

// Increment and Decrement

template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T>& operator++(mat<NCol, NRow, T>& m) {
    m = madd(m, 1);
    return m;
}
template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T> operator++(mat<NCol, NRow, T>& m, int) {
    mat<NCol, NRow, T> res = m;
    m = madd(m, 1);
    return res;
}
template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T>& operator--(mat<NCol, NRow, T>& m) {
    m = msub(m, 1);
    return m;
}
template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T> operator--(mat<NCol, NRow, T>& m, int) {
    mat<NCol, NRow, T> res = m;
    m = msub(m, 1);
    return res;
}

// Assignment

template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T>& operator+=(mat<NCol, NRow, T>& m1, const mat<NCol, NRow, T>& m2) {
    m1 = madd(m1, m2);
    return m1;
}
template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T>& operator+=(mat<NCol, NRow, T>& m, T value) {
    m = madd(m, value);
    return m;
}

template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T>& operator-=(mat<NCol, NRow, T>& m1, const mat<NCol, NRow, T>& m2) {
    m1 = msub(m1, m2);
    return m1;
}
template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T>& operator-=(mat<NCol, NRow, T>& m, T value) {
    m = msub(m, value);
    return m;
}

template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T>& operator*=(mat<NCol, NRow, T>& m, T value) {
    m = mmul(m, value);
    return m;
}

template<i32 NCol, i32 NRow, typename T>
constexpr mat<NCol, NRow, T>& operator*=(mat<NCol, NRow, T>& m1, const mat<NCol, NRow, T>& m2) {
    m1 = mmul(m1, m2);
    return m1;
}

#pragma endregion

#pragma region Type Aliasing

template<typename T> using mat2 = mat<2, 2, T>;
template<typename T> using mat3 = mat<3, 3, T>;
template<typename T> using mat4 = mat<4, 4, T>;
template<typename T> using mat2x2 = mat<2, 2, T>;
template<typename T> using mat2x3 = mat<2, 3, T>;
template<typename T> using mat2x4 = mat<2, 4, T>;
template<typename T> using mat3x2 = mat<3, 2, T>;
template<typename T> using mat3x3 = mat<3, 3, T>;
template<typename T> using mat3x4 = mat<3, 4, T>;
template<typename T> using mat4x2 = mat<4, 2, T>;
template<typename T> using mat4x3 = mat<4, 3, T>;
template<typename T> using mat4x4 = mat<4, 4, T>;

using mat2b = mat2<bool>;
using mat2i = mat2<i32>;
using mat2u = mat2<u32>;
using mat2f = mat2<f32>;
using mat2d = mat2<f64>;
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

using mat3b = mat3<bool>;
using mat3i = mat3<i32>;
using mat3u = mat3<u32>;
using mat3f = mat3<f32>;
using mat3d = mat3<f64>;
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

using mat4b = mat4<bool>;
using mat4i = mat4<i32>;
using mat4u = mat4<u32>;
using mat4f = mat4<f32>;
using mat4d = mat4<f64>;
using mat4x4b = mat4x4<bool>;
using mat4x4i = mat4x4<i32>;
using mat4x4u = mat4x4<u32>;
using mat4x4f = mat4x4<f32>;
using mat4x4d = mat4x4<f64>;

#pragma endregion
