#include "mat.h"
#include "mat_example.h"

#include <fmt/format.h>
#include <glm/matrix.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <tuple>

template<typename TMat>
void mat_print_row_major(TMat m) {
    for (int i = 0; i < m.NRow; ++i) {
        for (int j = 0; j < m.NCol; ++j) {
            fmt::print("{} ", m[i][j]);
        }
        fmt::print("\n");
    }
}

template<typename TMat>
void mat_print_col_major(TMat m) {
    for (int i = 0; i < m.NCol; ++i) {
        for (int j = 0; j < m.NRow; ++j) {
            fmt::print("{} ", m[j][i]);
        }
        fmt::print("\n");
    }
}

template<typename TMat>
void mat_print_glm(TMat m) {
    fmt::print("{}\n", glm::to_string(m));
}

template<typename TMat, typename TGlmMat>
void compare_to_glm(TMat m, TGlmMat glmM) {
    for (int i = 0; i < m.NCol; ++i) {
        for (int j = 0; j < m.NRow; ++j) {
            Assert(m[i][j] == glmM[i][j], fmt::format("m[{}][{}] != glmM[{}][{}]", i, j, i, j).data());
        }
    }
}

constexpr i32 fuzzCount = 1000;

std::tuple<f32, f32, f32, f32> get_f32_fuzz() {
    f32 x = (f32)core::rnd_i32(-5, 5);
    f32 y = (f32)core::rnd_i32(-5, 5);
    f32 z = (f32)core::rnd_i32(-5, 5);
    f32 w = (f32)core::rnd_i32(-5, 5);
    return {x, y, z, w};
}

void check_determinant() {
    i32 c = fuzzCount;
    while (c-- > 0) {
        auto [n0, n1, n2, n3] = get_f32_fuzz();
        auto [n4, n5, n6, n7] = get_f32_fuzz();
        auto [n8, n9, n10, n11] = get_f32_fuzz();
        auto [n12, n13, n14, n15] = get_f32_fuzz();
        {
            mat2f m1(n0, n1, n2, n3);
            glm::mat2 glmM1(n0, n1, n2, n3);
            auto v = det(m1);
            auto vglm = glm::determinant(glmM1);
            Assert(v == vglm, fmt::format("det(m1) != det(glmM1) {} != {}", v, vglm).data());
        }
        {
            mat3f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8);
            glm::mat3 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8);
            auto v = det(m1);
            auto vglm = glm::determinant(glmM1);
            Assert(v == vglm, fmt::format("det(m1) != det(glmM1) {} != {}", v, vglm).data());
        }
        {
            mat4f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15);
            glm::mat4 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15);
            auto v = det(m1);
            auto vglm = glm::determinant(glmM1);
            Assert(v == vglm, fmt::format("det(m1) != det(glmM1) {} != {}", v, vglm).data());
        }
    }
}

void check_ctors() {
    i32 c = fuzzCount;
    while (c-- > 0) {
        auto [n0, n1, n2, n3] = get_f32_fuzz();
        auto [n4, n5, n6, n7] = get_f32_fuzz();
        auto [n8, n9, n10, n11] = get_f32_fuzz();
        auto [n12, n13, n14, n15] = get_f32_fuzz();
        {
            mat2f m1(n0, n1, n2, n3);
            glm::mat2 glmM1(n0, n1, n2, n3);
            compare_to_glm(m1, glmM1);
        }
        {
            mat2x3f m1(n0, n1, n2, n3, n4, n5);
            glm::mat2x3 glmM1(n0, n1, n2, n3, n4, n5);
            compare_to_glm(m1, glmM1);
        }
        {
            mat2x4f m1(n0, n1, n2, n3, n4, n5, n6, n7);
            glm::mat2x4 glmM1(n0, n1, n2, n3, n4, n5, n6, n7);
            compare_to_glm(m1, glmM1);
        }
        {
            mat3x2f m1(n0, n1, n2, n3, n4, n5);
            glm::mat3x2 glmM1(n0, n1, n2, n3, n4, n5);
            compare_to_glm(m1, glmM1);
        }
        {
            mat3f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8);
            glm::mat3 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8);
            compare_to_glm(m1, glmM1);
        }
        {
            mat3x4f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            glm::mat3x4 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            compare_to_glm(m1, glmM1);
        }
        {
            mat4x2f m1(n0, n1, n2, n3, n4, n5, n6, n7);
            glm::mat4x2 glmM1(n0, n1, n2, n3, n4, n5, n6, n7);
            compare_to_glm(m1, glmM1);
        }
        {
            mat4x3f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            glm::mat4x3 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            compare_to_glm(m1, glmM1);
        }
        {
            mat4f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15);
            glm::mat4 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15);
            compare_to_glm(m1, glmM1);
        }
    }
}

void check_identity() {
    i32 c = fuzzCount;
    while (c-- > 0) {
        auto [n0, n1, n2, n3] = get_f32_fuzz();
        auto [n4, n5, n6, n7] = get_f32_fuzz();
        auto [n8, n9, n10, n11] = get_f32_fuzz();
        auto [n12, n13, n14, n15] = get_f32_fuzz();
        {
            mat2f m1(n0, n1, n2, n3);
            glm::mat2 glmM1(n0, n1, n2, n3);
            compare_to_glm(m1.identity(), glm::identity<glm::mat2>());
            compare_to_glm(mat2f::identity(), glm::identity<glm::mat2>());
        }
        {
            mat3f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8);
            glm::mat3 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8);
            compare_to_glm(m1.identity(), glm::identity<glm::mat3>());
            compare_to_glm(mat3f::identity(), glm::identity<glm::mat3>());
        }
        {
            mat4f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15);
            glm::mat4 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15);
            compare_to_glm(m1.identity(), glm::identity<glm::mat4>());
            compare_to_glm(mat4f::identity(), glm::identity<glm::mat4>());
        }
    }
}

void check_transpose() {
    i32 c = fuzzCount;
    while (c-- > 0) {
        auto [n0, n1, n2, n3] = get_f32_fuzz();
        auto [n4, n5, n6, n7] = get_f32_fuzz();
        auto [n8, n9, n10, n11] = get_f32_fuzz();
        auto [n12, n13, n14, n15] = get_f32_fuzz();
        {
            mat2f m1(n0, n1, n2, n3);
            glm::mat2 glmM1(n0, n1, n2, n3);
            compare_to_glm(mtranspose(m1), glm::transpose(glmM1));
        }
        {
            mat2x3f m1(n0, n1, n2, n3, n4, n5);
            glm::mat2x3 glmM1(n0, n1, n2, n3, n4, n5);
            compare_to_glm(mtranspose(m1), glm::transpose(glmM1));
        }
        {
            mat2x4f m1(n0, n1, n2, n3, n4, n5, n6, n7);
            glm::mat2x4 glmM1(n0, n1, n2, n3, n4, n5, n6, n7);
            compare_to_glm(mtranspose(m1), glm::transpose(glmM1));
        }
        {
            mat3x2f m1(n0, n1, n2, n3, n4, n5);
            glm::mat3x2 glmM1(n0, n1, n2, n3, n4, n5);
            compare_to_glm(mtranspose(m1), glm::transpose(glmM1));
        }
        {
            mat3f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8);
            glm::mat3 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8);
            compare_to_glm(mtranspose(m1), glm::transpose(glmM1));
        }
        {
            mat3x4f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            glm::mat3x4 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            compare_to_glm(mtranspose(m1), glm::transpose(glmM1));
        }
        {
            mat4x2f m1(n0, n1, n2, n3, n4, n5, n6, n7);
            glm::mat4x2 glmM1(n0, n1, n2, n3, n4, n5, n6, n7);
            compare_to_glm(mtranspose(m1), glm::transpose(glmM1));
        }
        {
            mat4x3f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            glm::mat4x3 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            compare_to_glm(mtranspose(m1), glm::transpose(glmM1));
        }
        {
            mat4f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15);
            glm::mat4 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15);
            compare_to_glm(mtranspose(m1), glm::transpose(glmM1));
        }
    }
}

void check_addition_and_subtraction() {
    i32 c = fuzzCount;
    while (c-- > 0) {
        auto [n0, n1, n2, n3] = get_f32_fuzz();
        auto [n4, n5, n6, n7] = get_f32_fuzz();
        auto [n8, n9, n10, n11] = get_f32_fuzz();
        auto [n12, n13, n14, n15] = get_f32_fuzz();
        auto [n16, n17, n18, n19] = get_f32_fuzz();
        auto [n20, n21, n22, n23] = get_f32_fuzz();
        auto [n24, n25, n26, n27] = get_f32_fuzz();
        auto [n28, n29, n30, n31] = get_f32_fuzz();
        {
            mat2f m1(n0, n1, n2, n3);
            glm::mat2 glmM1(n0, n1, n2, n3);
            mat2f m2(n4, n5, n6, n7);
            glm::mat2 glmM2(n4, n5, n6, n7);
            compare_to_glm(m1 + m2, glmM1 + glmM2);
            compare_to_glm(m1 - m2, glmM1 - glmM2);
        }
        {
            mat2x3f m1(n0, n1, n2, n3, n4, n5);
            glm::mat2x3 glmM1(n0, n1, n2, n3, n4, n5);
            mat2x3f m2(n6, n7, n8, n9, n10, n11);
            glm::mat2x3 glmM2(n6, n7, n8, n9, n10, n11);
            compare_to_glm(m1 + m2, glmM1 + glmM2);
            compare_to_glm(m1 - m2, glmM1 - glmM2);
        }
        {
            mat2x4f m1(n0, n1, n2, n3, n4, n5, n6, n7);
            glm::mat2x4 glmM1(n0, n1, n2, n3, n4, n5, n6, n7);
            mat2x4f m2(n8, n9, n10, n11, n12, n13, n14, n15);
            glm::mat2x4 glmM2(n8, n9, n10, n11, n12, n13, n14, n15);
            compare_to_glm(m1 + m2, glmM1 + glmM2);
            compare_to_glm(m1 - m2, glmM1 - glmM2);
        }
        {
            mat3x2f m1(n0, n1, n2, n3, n4, n5);
            glm::mat3x2 glmM1(n0, n1, n2, n3, n4, n5);
            mat3x2f m2(n6, n7, n8, n9, n10, n11);
            glm::mat3x2 glmM2(n6, n7, n8, n9, n10, n11);
            compare_to_glm(m1 + m2, glmM1 + glmM2);
            compare_to_glm(m1 - m2, glmM1 - glmM2);
        }
        {
            mat3f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8);
            glm::mat3 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8);
            mat3f m2(n9, n10, n11, n12, n13, n14, n15, n16, n17);
            glm::mat3 glmM2(n9, n10, n11, n12, n13, n14, n15, n16, n17);
            compare_to_glm(m1 + m2, glmM1 + glmM2);
            compare_to_glm(m1 - m2, glmM1 - glmM2);
        }
        {
            mat3x4f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            glm::mat3x4 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            mat3x4f m2(n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23);
            glm::mat3x4 glmM2(n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23);
            compare_to_glm(m1 + m2, glmM1 + glmM2);
            compare_to_glm(m1 - m2, glmM1 - glmM2);
        }
        {
            mat4x2f m1(n0, n1, n2, n3, n4, n5, n6, n7);
            glm::mat4x2 glmM1(n0, n1, n2, n3, n4, n5, n6, n7);
            mat4x2f m2(n8, n9, n10, n11, n12, n13, n14, n15);
            glm::mat4x2 glmM2(n8, n9, n10, n11, n12, n13, n14, n15);
            compare_to_glm(m1 + m2, glmM1 + glmM2);
            compare_to_glm(m1 - m2, glmM1 - glmM2);
        }
        {
            mat4x3f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            glm::mat4x3 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            mat4x3f m2(n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23);
            glm::mat4x3 glmM2(n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23);
            compare_to_glm(m1 + m2, glmM1 + glmM2);
            compare_to_glm(m1 - m2, glmM1 - glmM2);
        }
        {
            mat4f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15);
            glm::mat4 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15);
            mat4f m2(n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31);
            glm::mat4 glmM2(n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31);
            compare_to_glm(m1 + m2, glmM1 + glmM2);
            compare_to_glm(m1 - m2, glmM1 - glmM2);
        }
    }
}

void check_multiplication() {
    i32 c = fuzzCount;
    while (c-- > 0) {
        auto [n0, n1, n2, n3] = get_f32_fuzz();
        auto [n4, n5, n6, n7] = get_f32_fuzz();
        auto [n8, n9, n10, n11] = get_f32_fuzz();
        auto [n12, n13, n14, n15] = get_f32_fuzz();
        auto [n16, n17, n18, n19] = get_f32_fuzz();
        auto [n20, n21, n22, n23] = get_f32_fuzz();
        auto [n24, n25, n26, n27] = get_f32_fuzz();
        auto [n28, n29, n30, n31] = get_f32_fuzz();
        {
            // 2x2 * 2x2 = 2x2
            mat2f m1(n0, n1, n2, n3);
            glm::mat2 glmM1(n0, n1, n2, n3);
            mat2f m2(n4, n5, n6, n7);
            glm::mat2 glmM2(n4, n5, n6, n7);
            auto res = m1 * m2;
            glm::mat2 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 2x2 * 3x2 = 3x2
            mat2f m1(n0, n1, n2, n3);
            glm::mat2 glmM1(n0, n1, n2, n3);
            mat3x2f m2(n4, n5, n6, n7, n8, n9);
            glm::mat3x2 glmM2(n4, n5, n6, n7, n8, n9);
            auto res = m1 * m2;
            glm::mat3x2 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 2x2 * 4x2 = 4x2
            mat2f m1(n0, n1, n2, n3);
            glm::mat2 glmM1(n0, n1, n2, n3);
            mat4x2f m2(n4, n5, n6, n7, n8, n9, n10, n11);
            glm::mat4x2 glmM2(n4, n5, n6, n7, n8, n9, n10, n11);
            auto res = m1 * m2;
            glm::mat4x2 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 2x3 * 2x2 = 2x3
            mat2x3f m1(n0, n1, n2, n3, n4, n5);
            glm::mat2x3 glmM1(n0, n1, n2, n3, n4, n5);
            mat2f m2(n6, n7, n8, n9);
            glm::mat2 glmM2(n6, n7, n8, n9);
            auto res = m1 * m2;
            glm::mat2x3 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 2x3 * 3x2 = 3x3
            mat2x3f m1(n0, n1, n2, n3, n4, n5);
            glm::mat2x3 glmM1(n0, n1, n2, n3, n4, n5);
            mat3x2f m2(n6, n7, n8, n9, n10, n11);
            glm::mat3x2 glmM2(n6, n7, n8, n9, n10, n11);
            auto res = m1 * m2;
            glm::mat3 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 2x3 * 4x2 = 4x3
            mat2x3f m1(n0, n1, n2, n3, n4, n5);
            glm::mat2x3 glmM1(n0, n1, n2, n3, n4, n5);
            mat4x2f m2(n6, n7, n8, n9, n10, n11, n12, n13);
            glm::mat4x2 glmM2(n6, n7, n8, n9, n10, n11, n12, n13);
            auto res = m1 * m2;
            glm::mat4x3 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 2x4 * 2x2 = 2x4
            mat2x4f m1(n0, n1, n2, n3, n4, n5, n6, n7);
            glm::mat2x4 glmM1(n0, n1, n2, n3, n4, n5, n6, n7);
            mat2f m2(n8, n9, n10, n11);
            glm::mat2 glmM2(n8, n9, n10, n11);
            auto res = m1 * m2;
            glm::mat2x4 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 2x4 * 3x2 = 3x4
            mat2x4f m1(n0, n1, n2, n3, n4, n5, n6, n7);
            glm::mat2x4 glmM1(n0, n1, n2, n3, n4, n5, n6, n7);
            mat3x2f m2(n8, n9, n10, n11, n12, n13);
            glm::mat3x2 glmM2(n8, n9, n10, n11, n12, n13);
            auto res = m1 * m2;
            glm::mat3x4 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 2x4 * 4x2 = 4x4
            mat2x4f m1(n0, n1, n2, n3, n4, n5, n6, n7);
            glm::mat2x4 glmM1(n0, n1, n2, n3, n4, n5, n6, n7);
            mat4x2f m2(n8, n9, n10, n11, n12, n13, n14, n15);
            glm::mat4x2 glmM2(n8, n9, n10, n11, n12, n13, n14, n15);
            auto res = m1 * m2;
            glm::mat4x4 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 3x2 * 2x3 = 2x2
            mat3x2f m1(n0, n1, n2, n3, n4, n5);
            glm::mat3x2 glmM1(n0, n1, n2, n3, n4, n5);
            mat2x3f m2(n6, n7, n8, n9, n10, n11);
            glm::mat2x3 glmM2(n6, n7, n8, n9, n10, n11);
            auto res = m1 * m2;
            glm::mat2x2 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 3x2 * 3x3 = 3x2
            mat3x2f m1(n0, n1, n2, n3, n4, n5);
            glm::mat3x2 glmM1(n0, n1, n2, n3, n4, n5);
            mat3x3f m2(n6, n7, n8, n9, n10, n11, n12, n13, n14);
            glm::mat3x3 glmM2(n6, n7, n8, n9, n10, n11, n12, n13, n14);
            auto res = m1 * m2;
            glm::mat3x2 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 3x2 * 4x3 = 4x2
            mat3x2f m1(n0, n1, n2, n3, n4, n5);
            glm::mat3x2 glmM1(n0, n1, n2, n3, n4, n5);
            mat4x3f m2(n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16, n17);
            glm::mat4x3 glmM2(n6, n7, n8, n9, n10, n11, n12, n13, n14, n15, n16, n17);
            auto res = m1 * m2;
            glm::mat4x2 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 3x3 * 2x3 = 2x3
            mat3x3f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8);
            glm::mat3x3 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8);
            mat2x3f m2(n9, n10, n11, n12, n13, n14);
            glm::mat2x3 glmM2(n9, n10, n11, n12, n13, n14);
            auto res = m1 * m2;
            glm::mat2x3 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 3x3 * 3x3 = 3x3
            mat3x3f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8);
            glm::mat3x3 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8);
            mat3x3f m2(n9, n10, n11, n12, n13, n14, n15, n16, n17);
            glm::mat3x3 glmM2(n9, n10, n11, n12, n13, n14, n15, n16, n17);
            auto res = m1 * m2;
            glm::mat3x3 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 3x3 * 4x3 = 4x3
            mat3x3f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8);
            glm::mat3x3 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8);
            mat4x3f m2(n9, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20);
            glm::mat4x3 glmM2(n9, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20);
            auto res = m1 * m2;
            glm::mat4x3 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 3x4 * 2x3 = 2x4
            mat3x4f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            glm::mat3x4 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            mat2x3f m2(n12, n13, n14, n15, n16, n17);
            glm::mat2x3 glmM2(n12, n13, n14, n15, n16, n17);
            auto res = m1 * m2;
            glm::mat2x4 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 3x4 * 3x3 = 3x4
            mat3x4f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            glm::mat3x4 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            mat3x3f m2(n12, n13, n14, n15, n16, n17, n18, n19, n20);
            glm::mat3x3 glmM2(n12, n13, n14, n15, n16, n17, n18, n19, n20);
            auto res = m1 * m2;
            glm::mat3x4 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 3x4 * 4x3 = 4x4
            mat3x4f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            glm::mat3x4 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            mat4x3f m2(n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23);
            glm::mat4x3 glmM2(n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23);
            auto res = m1 * m2;
            glm::mat4x4 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 4x2 * 2x4 = 2x2
            mat4x2f m1(n0, n1, n2, n3, n4, n5, n6, n7);
            glm::mat4x2 glmM1(n0, n1, n2, n3, n4, n5, n6, n7);
            mat2x4f m2(n8, n9, n10, n11, n12, n13, n14, n15);
            glm::mat2x4 glmM2(n8, n9, n10, n11, n12, n13, n14, n15);
            auto res = m1 * m2;
            glm::mat2x2 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 4x2 * 3x4 = 3x2
            mat4x2f m1(n0, n1, n2, n3, n4, n5, n6, n7);
            glm::mat4x2 glmM1(n0, n1, n2, n3, n4, n5, n6, n7);
            mat3x4f m2(n8, n9, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19);
            glm::mat3x4 glmM2(n8, n9, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19);
            auto res = m1 * m2;
            glm::mat3x2 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 4x2 * 4x4 = 4x2
            mat4x2f m1(n0, n1, n2, n3, n4, n5, n6, n7);
            glm::mat4x2 glmM1(n0, n1, n2, n3, n4, n5, n6, n7);
            mat4x4f m2(n8, n9, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23);
            glm::mat4x4 glmM2(n8, n9, n10, n11, n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23);
            auto res = m1 * m2;
            glm::mat4x2 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 4x3 * 2x4 = 2x3
            mat4x3f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            glm::mat4x3 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            mat2x4f m2(n12, n13, n14, n15, n16, n17, n18, n19);
            glm::mat2x4 glmM2(n12, n13, n14, n15, n16, n17, n18, n19);
            auto res = m1 * m2;
            glm::mat2x3 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 4x3 * 3x4 = 3x3
            mat4x3f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            glm::mat4x3 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            mat3x4f m2(n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23);
            glm::mat3x4 glmM2(n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23);
            auto res = m1 * m2;
            glm::mat3x3 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 4x3 * 4x4 = 4x3
            mat4x3f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            glm::mat4x3 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11);
            mat4x4f m2(n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27);
            glm::mat4x4 glmM2(n12, n13, n14, n15, n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27);
            auto res = m1 * m2;
            glm::mat4x3 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 4x4 * 2x4 = 2x4
            mat4x4f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15);
            glm::mat4x4 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15);
            mat2x4f m2(n16, n17, n18, n19, n20, n21, n22, n23);
            glm::mat2x4 glmM2(n16, n17, n18, n19, n20, n21, n22, n23);
            auto res = m1 * m2;
            glm::mat2x4 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 4x4 * 3x4 = 3x4
            mat4x4f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15);
            glm::mat4x4 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15);
            mat3x4f m2(n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27);
            glm::mat3x4 glmM2(n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27);
            auto res = m1 * m2;
            glm::mat3x4 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
        {
            // 4x4 * 4x4 = 4
            mat4x4f m1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15);
            glm::mat4x4 glmM1(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15);
            mat4x4f m2(n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31);
            glm::mat4x4 glmM2(n16, n17, n18, n19, n20, n21, n22, n23, n24, n25, n26, n27, n28, n29, n30, n31);
            auto res = m1 * m2;
            glm::mat4x4 glmRes = glmM1 * glmM2;
            compare_to_glm(res, glmRes);
        }
    }
}

void check_inverse() {
    i32 c = fuzzCount;
    while (c-- > 0) {
        auto [n0, n1, n2, n3] = get_f32_fuzz();
        auto [n4, n5, n6, n7] = get_f32_fuzz();
        auto [n8, n9, n10, n11] = get_f32_fuzz();
        auto [n12, n13, n14, n15] = get_f32_fuzz();
        {
            // 2x2
            mat2x2f m(n0, n1, n2, n3);
            glm::mat2 glmM(n0, n1, n2, n3);
            auto res = minverse(m);
            if (res[0][0] == INFINITY) continue;
            glm::mat2 glmRes = glm::inverse(glmM);
            compare_to_glm(res, glmRes);
        }
        {
            // 3x3
            mat3x3f m(n0, n1, n2, n3, n4, n5, n6, n7, n8);
            glm::mat3 glmM(n0, n1, n2, n3, n4, n5, n6, n7, n8);
            auto res = minverse(m);
            if (res[0][0] == INFINITY) continue;
            glm::mat3 glmRes = glm::inverse(glmM);
            compare_to_glm(res, glmRes);
        }
        {
            // 4x4
            mat4x4f m(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15);
            glm::mat4 glmM(n0, n1, n2, n3, n4, n5, n6, n7, n8, n9, n10, n11, n12, n13, n14, n15);
            auto res = minverse(m);
            if (res[0][0] == INFINITY) continue;
            glm::mat4 glmRes = glm::inverse(glmM);
            compare_to_glm(res, glmRes);
        }
    }
}

void refactorRunMatExample() {
    check_ctors();
    check_determinant();
    check_identity();
    check_addition_and_subtraction();
    check_multiplication();
    check_inverse();
}
