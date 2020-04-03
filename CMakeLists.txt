# 设置 CMake 的最低版本要求
cmake_minimum_required(VERSION 3.14 FATAL_ERROR)

# 设定项目
project(UEngine VERSION 0.0.1)
message(STATUS "[Project] ${PROJECT_NAME}")

# 在 Debug 模式下自动对 dll 和 lib 文件名后边加 d，但 exe 仍然需要手动设置
set(CMAKE_DEBUG_POSTFIX d)

# 确定 C++ 版本
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

if(MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /permissive-")
endif()

# 将生成的 exe, dll, lib 等放置在 bin, lib 目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${PROJECT_SOURCE_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${PROJECT_SOURCE_DIR}/lib")

# 开启 VS 的文件夹功能
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(FetchContent)

message(STATUS "find package: UCMake v0.4.0")
find_package(UCMake 0.4.0 QUIET)
if(${UCMake_FOUND})
	message(STATUS "UCMake v${UCMake_VERSION} found")
else()
	set(_address "https://github.com/Ubpa/UCMake")
	message(STATUS "UCMake v0.4.0 not found, so fetch it ...\n"
	"fetch: ${_address} with tag v0.4.0")
	FetchContent_Declare(
	  UCMake
	  GIT_REPOSITORY ${_address}
	  GIT_TAG "v0.4.0"
	)
	message(STATUS "UCMake v0.4.0 fetch done, building ...")
	FetchContent_MakeAvailable(UCMake)
	message(STATUS "UCMake v0.4.0 build done")
endif()

# 修改默认的安装路径前缀
Ubpa_InitInstallPrefix()

message(STATUS "----------")
if(EXISTS "${PROJECT_SOURCE_DIR}/include/_deps/detail/glad/glad.h"
	AND EXISTS "${PROJECT_SOURCE_DIR}/src/_deps/glad/glad.c"
	AND EXISTS "${PROJECT_SOURCE_DIR}/include/_deps/detail/KHR/khrplatform.h")
	message(STATUS "found glad")
else()
	message(FATAL_ERROR "not found glad\n"
		"read README.md for more detail")
endif()

message(STATUS "----------")
find_package(glfw3 REQUIRED)
if(TARGET glfw)
	message(STATUS "glfw3_FOUND: ${glfw3_FOUND}")
	message(STATUS "glfw3_DIR: ${glfw3_DIR}")
endif()

set(UBPA_UGL_GLAD TRUE)

Ubpa_AddDep(URTR 0.0.1)

Ubpa_AddSubDirsRec(src)

set(initTail "find_package(glfw3 REQUIRED)
if(TARGET glfw)
	message(STATUS \"glfw3_FOUND: \${glfw3_FOUND}\")
	message(STATUS \"glfw3_DIR: \${glfw3_DIR}\")
endif()
")

Ubpa_Export(DIRECTORIES "include" INIT_TAIL "${initTail}")
