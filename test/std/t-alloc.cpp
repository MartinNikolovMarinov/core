#include <alloc.h>
#include <static_bump_alloc.h>
#include <std_alloc.h>

#include <string>
#include <string_view>

void StaticBumpAllcatorTest() {
    core::alloc::StaticBumpAllocator<32> salloc;
    ptr_size curExpectedAllocSize = 0;
    Assert(core::alloc::UsedMem(salloc) == curExpectedAllocSize);

    u32* a = reinterpret_cast<u32*>(core::alloc::Alloc(salloc, sizeof(u32)));
    curExpectedAllocSize += core::Align(sizeof(u32));
    Assert(a != nullptr);
    Assert(core::alloc::UsedMem(salloc) == curExpectedAllocSize);
    *a = 1;

    // Static Allocator free does nothing but complies to the alloc interface !
    core::alloc::Free(salloc, a);
    Assert(core::alloc::UsedMem(salloc) == core::Align(sizeof(u32)));

    u8* b = reinterpret_cast<u8*>(core::alloc::Alloc(salloc, sizeof(u8)));
    curExpectedAllocSize += core::Align(sizeof(u8));
    Assert(b != nullptr);
    Assert(core::alloc::UsedMem(salloc) == curExpectedAllocSize);
    *b = 2;

    u16* c = reinterpret_cast<u16*>(core::alloc::Alloc(salloc, sizeof(u16) * 3));
    curExpectedAllocSize += core::Align(sizeof(u16) * 3);
    Assert(c != nullptr);
    Assert(core::alloc::UsedMem(salloc) == curExpectedAllocSize);
    c[0] = 3;
    c[1] = 4;
    c[2] = 5;

    struct Example {
        Example(u8 e1, u16 e2) : e1(e1), e2(e2) {}
        u8 e1;
        u16 e2;
    };

    Example* e = core::alloc::Construct(salloc, (Example*)(nullptr), u8(6), u16(7));
    curExpectedAllocSize += core::Align(sizeof(Example));
    Assert(e != nullptr);
    Assert(core::alloc::UsedMem(salloc) == curExpectedAllocSize);

    u8* d = reinterpret_cast<u8*>(core::alloc::Alloc(salloc, sizeof(u8) * 4));
    Assert(d == nullptr, "Out of memory should be reached here!");

    // Check that non of the allocated memory has been unintentionally overwritten.
    Assert(*a == 1);
    Assert(*b == 2);
    Assert(c[0] == 3);
    Assert(c[1] == 4);
    Assert(c[2] == 5);
    Assert(e->e1 == 6);
    Assert(e->e2 == 7);

    core::alloc::Clear(salloc);
    Assert(core::alloc::UsedMem(salloc) == 0);
}

void StdAllocatorTest() {
    core::alloc::StdAllocator stdAlloc;
    void* bytes = stdAlloc.Alloc(100);
    Assert(bytes != nullptr);

    // OOM test
    static i32 oomCount = 0;
    stdAlloc.OnOutOfMemory([](void*) { oomCount++; });
    bytes = stdAlloc.Alloc(10000000000000);
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
    person = stdAlloc.Construct<Person>(person, "Pesho", 20);
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
