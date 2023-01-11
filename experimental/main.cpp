#include "better_tuple.cpp"

template <typename T, typename TLenType = u64>
struct slice {

using data_type = T;
using len_type = TLenType;

slice(data_type* data, len_type len) : data(data), len(len) {}

private:
    data_type* data;
    len_type len;
};

i32 main(i32, char**) {
    tuple_usage();
    return 0;
}
