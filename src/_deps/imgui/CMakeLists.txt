Ubpa_GlobGroupSrcs(RST sources PATHS
	${CMAKE_CURRENT_SOURCE_DIR}
	"${PROJECT_SOURCE_DIR}/include/_deps/imgui"
)

Ubpa_AddTarget_GDR(MODE "LIB"
	SOURCES ${sources}
	INC_DIRS "_deps/imgui"
)
