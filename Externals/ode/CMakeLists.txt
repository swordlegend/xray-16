if (STATIC_BUILD)
    add_library(xrODE STATIC)
else()
    add_library(xrODE SHARED)
endif()

target_sources(xrODE PRIVATE
    ode/src/array.cpp
    ode/src/array.h
    ode/src/Bounder33.cpp
    ode/src/Bounder33.h
    ode/src/collision_kernel.cpp
    ode/src/collision_kernel.h
    ode/src/collision_quadtreespace.cpp
    ode/src/collision_space.cpp
    ode/src/collision_space_internal.h
    ode/src/collision_std.cpp
    ode/src/collision_std.h
    ode/src/collision_transform.cpp
    ode/src/collision_transform.h
    ode/src/collision_trimesh_internal.h
    ode/src/collision_util.cpp
    ode/src/collision_util.h
    ode/src/error.cpp
    ode/src/export-dif.cpp
    ode/src/fastdot.c
    ode/src/fastldlt.c
    ode/src/fastlsolve.c
    ode/src/fastltsolve.c
    ode/src/geom_internal.h
    ode/src/joint.cpp
    ode/src/joint.h
    ode/src/Lcp33.h
    ode/src/lcp.cpp
    ode/src/lcp.h
    ode/src/mass.cpp
    ode/src/mat.cpp
    ode/src/mat.h
    ode/src/matrix.cpp
    ode/src/memory.cpp
    ode/src/misc.cpp
    ode/src/objects.h
    ode/src/obstack.cpp
    ode/src/obstack.h
    ode/src/ode.cpp
    ode/src/odemath.cpp
    ode/src/quickstep.cpp
    ode/src/quickstep.h
    ode/src/rotation.cpp
    ode/src/stack.h
    ode/src/step.cpp
    ode/src/stepfast.cpp
    ode/src/step.h
    ode/src/StepJointInternal.cpp
    ode/src/StepJointInternal.h
    ode/src/testing.cpp
    ode/src/testing.h
    ode/src/timer.cpp
    ode/src/util.cpp
    ode/src/util.h
    include/ode/collision.h
    include/ode/collision_space.h
    include/ode/collision_trimesh.h
    include/ode/common.h
    include/ode/compatibility.h
    include/ode/config.h
    include/ode/contact.h
    include/ode/error.h
    include/ode/export-dif.h
    include/ode/geom.h
    include/ode/mass.h
    include/ode/matrix.h
    include/ode/memory.h
    include/ode/misc.h
    include/ode/objects.h
    include/ode/odecpp_collision.h
    include/ode/odecpp.h
    include/ode/ode.h
    include/ode/odemath.h
    include/ode/rotation.h
    include/ode/space.h
    include/ode/timer.h
    contrib/msvc7/ode_default/de_padf_integration.cpp
    contrib/msvc7/ode_default/de_padf_integration.h
)

target_include_directories(xrODE
    PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

# TODO platform-specific defines
target_compile_definitions(xrODE
    PUBLIC
    dSINGLE
    dNODEBUG

    PRIVATE
    LINUX
    __cdecl=
)

if (MASTER_GOLD)
    target_compile_definitions(xrODE
        PUBLIC
        dNODEBUG
    )
endif()

set_target_properties(xrODE PROPERTIES
    PREFIX ""
    POSITION_INDEPENDENT_CODE ON
    UNITY_BUILD OFF
)

if (NOT STATIC_BUILD)
    install(TARGETS xrODE LIBRARY
        DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    )
endif()
