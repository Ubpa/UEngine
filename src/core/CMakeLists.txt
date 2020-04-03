Ubpa_GlobGroupSrcs(RST sources PATHS
	${CMAKE_CURRENT_SOURCE_DIR}
	"${PROJECT_SOURCE_DIR}/include/UEngine"
)

Ubpa_GetTargetName(imgui "${PROJECT_SOURCE_DIR}/src/_deps/imgui")

Ubpa_AddTarget_GDR(MODE "LIB"
	SOURCES ${sources}
	INTERFACE_INC ON
	PUBLIC_GLIBS "Ubpa::URTR_core;${imgui}"
)
