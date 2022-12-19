#include <variant>

// IMPORTANT: constexpr makes a huge difference in the generated code!

struct A {
    int a;
};

struct B {
    int b;
    int c;
};

struct AnonUnion {
    constexpr AnonUnion()    : a({}), type(0) {}
    constexpr AnonUnion(A a) : a(a),  type(1) {}
    constexpr AnonUnion(B b) : b(b),  type(2) {}

    union {
        A a;
        B b;
    };
    int type;
};

constexpr int UnionUsage(int a, int b, int c) {
    int res = 0;
    AnonUnion arr[2] = {};
    arr[0] = AnonUnion(A{ a });
    arr[1] = AnonUnion(B{ b, c });
    for (int i = 0; i < 2; i++) {
        if (arr[i].type == 1) {
            res += arr[i].a.a;
        } else if (arr[i].type == 2) {
            res += arr[i].b.b + arr[i].b.c;
        }
    }
    return res;
}

struct NoUnionStruct {
    int a;
    int b;
    int c;
    int type = 0;
};

constexpr int NoUnionUsage(int a, int b, int c) {
    NoUnionStruct arr[2] = {};
    int res = 0;
    arr[0].a = a;
    arr[0].type = 1;
    arr[1].b = b;
    arr[1].c = c;
    arr[1].type = 2;
    for (int i = 0; i < 2; i++) {
        if (arr[i].type == 1) {
            res += arr[i].a;
        } else if (arr[i].type == 2) {
            res += arr[i].b + arr[i].c;
        }
    }
    return res;
}

int main() {
    int a = UnionUsage(5, 6, 4);
    int b = NoUnionUsage(5, 6, 4);
    return a + b;
}
