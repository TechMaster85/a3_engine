get_filename_component(APP "${APP}" REALPATH)
include(BundleUtilities)
# Apple Silicon Homebrew lives in /opt/homebrew; Intel Homebrew in /usr/local.
# Both are provided so transitive deps (libjxl, libbrotli, etc.) can be found
# regardless of which machine performs the build.
fixup_bundle("${APP}" "" "/opt/homebrew/lib;/usr/local/lib")

# Homebrew replaced sdl2 with sdl2-compat, a shim that loads SDL3 at runtime
# via dlopen("@loader_path/libSDL3.dylib"). fixup_bundle uses otool -L and
# cannot detect dlopen deps, so SDL3 never gets bundled automatically.
# Copy it manually, rewrite its install name to be bundle-relative, and create
# an unversioned symlink (sdl2-compat dlopens the name without version suffix).
set(_fw_dir "${APP}/Contents/Frameworks")
if(NOT EXISTS "${_fw_dir}/libSDL3.dylib")
  foreach(_candidate "/opt/homebrew/lib/libSDL3.dylib" "/usr/local/lib/libSDL3.dylib")
    if(EXISTS "${_candidate}")
      get_filename_component(_sdl3_real "${_candidate}" REALPATH)
      get_filename_component(_sdl3_name "${_sdl3_real}" NAME)
      file(COPY "${_sdl3_real}" DESTINATION "${_fw_dir}")
      execute_process(COMMAND install_name_tool -id "@loader_path/${_sdl3_name}" "${_fw_dir}/${_sdl3_name}"
          COMMAND_ERROR_IS_FATAL ANY)
      file(CREATE_LINK "${_sdl3_name}" "${_fw_dir}/libSDL3.dylib" SYMBOLIC)
      break()
    endif()
  endforeach()
endif()

# fixup_bundle rewrites dylibs with install_name_tool, invalidating their
# original signatures. Re-sign everything with an ad-hoc identity so macOS
# doesn't kill the process on launch (required on macOS 13+).
file(GLOB_RECURSE EMBEDDED_LIBS "${APP}/Contents/Frameworks/*.dylib")
foreach(LIB ${EMBEDDED_LIBS})
    execute_process(COMMAND codesign --force --sign - "${LIB}"
        COMMAND_ERROR_IS_FATAL ANY)
endforeach()
execute_process(COMMAND codesign --force --sign - "${APP}"
    COMMAND_ERROR_IS_FATAL ANY)
