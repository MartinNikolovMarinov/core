# This folder contains implementations of allocator algorithms.

Every concreate allocator implementation follows the interface defined in `alloc.h`.

It's **IMPORTANT** to note that all implementations are **NOT** allowed to import `alloc.h`!

That also means that they can't use `core.h`, and obviously any data structure that uses a `default allocator macro` can **NOT** be used, because it will make it impossible for the user code to define it's own `default allocator macro`.

In general the implementations should remain as self contained as possible. That is why allocator implementations are in a folder of their own.
