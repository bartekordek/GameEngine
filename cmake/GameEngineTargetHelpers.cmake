# GameEngine helpers for third-party warning silence (Windows + Linux).
# Reuses cul_disable_third_party_warnings from deps/CUL/cmake/CULTargetHelpers.cmake.

if(GE_TARGET_HELPERS_INCLUDED)
    return()
endif()
set(GE_TARGET_HELPERS_INCLUDED TRUE)

# Silence every buildsystem target whose SOURCE_DIR lives under deps_root,
# except CUL first-party libraries (libcul / libculTests) so their warnings
# stay visible when developing CUL through a GameEngine build.
function(ge_silence_warnings_for_deps_dir deps_root)
    set(_ge_all_targets "")
    cul_get_all_targets_recursive(_ge_all_targets "${CMAKE_CURRENT_SOURCE_DIR}")

    file(TO_CMAKE_PATH "${deps_root}" _ge_deps_root)

    foreach(_ge_t IN LISTS _ge_all_targets)
        if(NOT TARGET ${_ge_t})
            # skip
        else()
            get_target_property(_ge_src ${_ge_t} SOURCE_DIR)
            if(_ge_src)
                file(TO_CMAKE_PATH "${_ge_src}" _ge_src_norm)
                string(FIND "${_ge_src_norm}" "${_ge_deps_root}" _ge_pos)
                if(_ge_pos EQUAL 0)
                    # Keep CUL first-party warning settings (nested CUL deps are
                    # already silenced by CUL's own cul_silence_warnings_for_deps_dir).
                    string(FIND "${_ge_src_norm}" "/libcul" _ge_libcul_pos)
                    string(FIND "${_ge_src_norm}" "/libculTests" _ge_libcultests_pos)
                    if(_ge_libcul_pos EQUAL -1 AND _ge_libcultests_pos EQUAL -1)
                        cul_disable_third_party_warnings(${_ge_t})
                    endif()
                endif()
            endif()
        endif()
    endforeach()
endfunction()
