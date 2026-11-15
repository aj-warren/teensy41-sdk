cmake_minimum_required(VERSION 3.20)

project(teensy_loader_cli LANGUAGES CXX)

add_executable(teensy_loader_cli teensy_loader_cli.cpp)
target_compile_features(teensy_loader_cli PRIVATE cxx_std_17)

if(WIN32)
    target_compile_definitions(teensy_loader_cli PRIVATE USE_WIN32 _CRT_SECURE_NO_WARNINGS)
    target_link_libraries(teensy_loader_cli PRIVATE hid setupapi winmm)
elseif(APPLE)
    target_compile_definitions(teensy_loader_cli PRIVATE USE_APPLE_IOKIT)
    target_link_libraries(teensy_loader_cli PRIVATE "-framework IOKit" "-framework CoreFoundation")
elseif(UNIX)
    find_library(USB_LIBRARY usb REQUIRED)
    target_compile_definitions(teensy_loader_cli PRIVATE USE_LIBUSB)
    target_link_libraries(teensy_loader_cli PRIVATE ${USB_LIBRARY})
endif()
