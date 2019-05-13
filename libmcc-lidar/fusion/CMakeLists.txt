file (GLOB SRC_FILES *.cpp)
file (GLOB HDR_FILES *.h)

add_library (fusion ${SRC_FILES} ${HDR_FILES})
