set(BUILD_SHARED_LIBS OFF)
set(CMAKE_EXE_LINKER_FLAGS "-static")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_FILE_OFFSET_BITS=64")

set(EXENAME "uloader")


set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(${EXENAME} uaploader.cpp)

link_libraries("-static")

target_include_directories(${EXENAME} PRIVATE)

target_link_libraries(${EXENAME} ${LINK_LIBS})

# objcopy  --add-section .uap.data=data.sqsfs --set-section-flags .uap.data=noload,readonly uloader test_x86.uab
# sudo mount -o ro,offset=0 data.sqsfs /mnt/
