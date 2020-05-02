Ubpa_GetTargetName(core "${PROJECT_SOURCE_DIR}/src/core")
Ubpa_AddTarget(
  MODE STATIC
  SOURCE
    "${PROJECT_SOURCE_DIR}/include/_deps/imgui"
  INC
    "include/_deps/imgui"
  LIB
    ${core}
)
