#include <alloc.h>

#include <string>
#include <string_view>

using namespace core;
using namespace alloc;

void StaticBumpAllcatorTest() {
    StaticBumpAllocator<32> salloc;
    ptr_size curExpectedAllocSize = 0;
    Assert(UsedMem(salloc) == curExpectedAllocSize);

    u32* a = reinterpret_cast<u32*>(Alloc(salloc, sizeof(u32)));
    curExpectedAllocSize += Align(sizeof(u32));
    Assert(a != nullptr);
    Assert(UsedMem(salloc) == curExpectedAllocSize);
    *a = 1;

    // Static Allocator free does nothing but complies to the alloc interface !
    Free(salloc, a);
    Assert(UsedMem(salloc) == Align(sizeof(u32)));

    u8* b = reinterpret_cast<u8*>(Alloc(salloc, sizeof(u8)));
    curExpectedAllocSize += Align(sizeof(u8));
    Assert(b != nullptr);
    Assert(UsedMem(salloc) == curExpectedAllocSize);
    *b = 2;

    u16* c = reinterpret_cast<u16*>(Alloc(salloc, sizeof(u16) * 3));
    curExpectedAllocSize += Align(sizeof(u16) * 3);
    Assert(c != nullptr);
    Assert(UsedMem(salloc) == curExpectedAllocSize);
    c[0] = 3;
    c[1] = 4;
    c[2] = 5;

    struct Example {
        Example(u8 e1, u16 e2) : e1(e1), e2(e2) {}
        u8 e1;
        u16 e2;
    };

    Example* e = Construct(salloc, (Example*)(nullptr), u8(6), u16(7));
    curExpectedAllocSize += Align(sizeof(Example));
    Assert(e != nullptr);
    Assert(UsedMem(salloc) == curExpectedAllocSize);

    u8* d = reinterpret_cast<u8*>(Alloc(salloc, sizeof(u8) * 4));
    Assert(d == nullptr, "Out of memory should be reached here!");

    // Check that non of the allocated memory has been unintentionally overwritten.
    Assert(*a == 1);
    Assert(*b == 2);
    Assert(c[0] == 3);
    Assert(c[1] == 4);
    Assert(c[2] == 5);
    Assert(e->e1 == 6);
    Assert(e->e2 == 7);

    Clear(salloc);
    Assert(UsedMem(salloc) == 0);
}

void StdAllocatorTest() {
    StdAllocator stdAlloc;
    void* bytes = Alloc(stdAlloc, 100);
    Assert(bytes != nullptr);

    // OOM test
    static i32 oomCount = 0;
    stdAlloc.OnOutOfMemory([](void*) { oomCount++; });
    bytes = Alloc(stdAlloc, 10000000000000);
    Assert(bytes == nullptr);
    Assert(oomCount == 1);

    // Everything should still work OOM:
    bytes = stdAlloc.Alloc(30);
    Assert(bytes != nullptr);

    struct Person {
        std::string name;
        i32 age;
        Person(std::string_view name, i32 age) : name(name), age(age) {}
    };
    Person* person = nullptr;
    person = Construct(stdAlloc, person, "Pesho", 20);
    Assert(person != nullptr);
    Assert(person->name == "Pesho");
    Assert(person->age == 20);

    ptr_size used = stdAlloc.UsedMem();
    Assert(used == 100 + 30 + sizeof(Person));
    stdAlloc.Free(person);
    used = stdAlloc.UsedMem();
    Assert(used == 100 + 30);

    stdAlloc.Clear();
    used = stdAlloc.UsedMem();
    Assert(used == 0);
}

void AllocTestSuite() {
    RunTest(StaticBumpAllcatorTest);
    RunTest(StdAllocatorTest);
}
