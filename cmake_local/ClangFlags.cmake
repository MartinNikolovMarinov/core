include(CompilerOptions)

macro(target_set_default_flags
    target
    is_debug
    save_temporary_files)

    set(common_flags "-std=c++20" "-pthread")
    set(debug_flags "")
    set(release_flags "")

    if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)")
        # For x86 assembly use Intel syntax
        set(common_flags, "-masm=intel")
    endif()

    if(${save_temporary_files})
        set(common_flags "${common_flags}" "-g" "-save-temps")
    endif()

    generate_common_flags(
        common_flags "${common_flags}"
        debug_flags "${debug_flags}"
        release_flags "${release_flags}"
    )

    if(${is_debug})
        target_compile_options(${target} PRIVATE ${common_flags} ${debug_flags})
    else()
        target_compile_options(${target} PRIVATE ${common_flags} ${release_flags})
    endif()

endmacro()

