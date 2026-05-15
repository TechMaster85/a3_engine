get_filename_component(APP "${APP}" REALPATH)
include(BundleUtilities)
# Apple Silicon Homebrew lives in /opt/homebrew; Intel Homebrew in /usr/local.
# Both are provided so transitive deps (libjxl, libbrotli, etc.) can be found
# regardless of which machine performs the build.
fixup_bundle("${APP}" "" "/opt/homebrew/lib;/usr/local/lib")

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
