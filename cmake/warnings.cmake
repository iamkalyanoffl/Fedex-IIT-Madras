include (CheckCXXCompilerFlag)
include (CheckCCompilerFlag)

# Try to add -Wflag if compiler supports it
macro (add_warning flag)
    string(REPLACE "-" "_" underscored_flag ${flag})
    string(REPLACE "+" "x" underscored_flag ${underscored_flag})

    check_cxx_compiler_flag("-W${flag}" SUPPORTS_CXXFLAG_${underscored_flag})
    check_c_compiler_flag("-W${flag}" SUPPORTS_CFLAG_${underscored_flag})

    if (SUPPORTS_CXXFLAG_${underscored_flag})
        set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W${flag}")
    else()
        message (STATUS "Flag -W${flag} is unsupported")
    endif()

    if (SUPPORTS_CFLAG_${underscored_flag})
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -W${flag}")
    else()
        message(STATUS "Flag -W${flag} is unsupported")
    endif()
endmacro()

# Try to add -Wno flag if compiler supports it
macro (no_warning flag)
    add_warning(no-${flag})
endmacro ()


# The same but only for specified target.
macro (target_add_warning target flag)
    string (REPLACE "-" "_" underscored_flag ${flag})
    string (REPLACE "+" "x" underscored_flag ${underscored_flag})

    check_cxx_compiler_flag("-W${flag}" SUPPORTS_CXXFLAG_${underscored_flag})

    if (SUPPORTS_CXXFLAG_${underscored_flag})
        target_compile_options (${target} PRIVATE "-W${flag}")
    else ()
        message (STATUS "Flag -W${flag} is unsupported")
    endif ()
endmacro ()

macro (target_no_warning target flag)
    target_add_warning(${target} no-${flag})
endmacro ()

add_warning(all)
add_warning(extra)
add_warning(pedantic)
add_warning(error) # treat all warnings as errors
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_warning(strict-overflow=1)
endif()
add_warning(suggest-override)
add_warning(suggest-destructor-override)
add_warning(unused)
add_warning(unreachable-code)
add_warning(delete-incomplete)
add_warning(duplicated-cond)
add_warning(disabled-optimization)
add_warning(init-self)
add_warning(bool-compare)
add_warning(logical-not-parentheses)
add_warning(logical-op)
add_warning(maybe-uninitialized)
add_warning(misleading-indentation)
# `no-` prefix is part of warning name(i.e. doesn't mean we are disabling it)
add_warning(no-return-local-addr)
add_warning(odr)
add_warning(pointer-arith)
add_warning(redundant-decls)
add_warning(reorder)
add_warning(shift-negative-value)
add_warning(sizeof-array-argument)
add_warning(switch-bool)
add_warning(tautological-compare)
add_warning(trampolines)
# these warnings are not enabled by default
# no_warning(name-of-warning)
