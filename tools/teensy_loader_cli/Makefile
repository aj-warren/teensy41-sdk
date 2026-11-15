#OS ?= LINUX
OS ?= WINDOWS
#OS ?= MACOSX
#OS ?= BSD

# uncomment this to use libusb on Macintosh, instead of Apple's HID manager via IOKit
# this is technically not the "correct" way to support Macs, but it's been reported to
# work.
#USE_LIBUSB ?= YES

ifeq ($(OS), LINUX)  # also works on FreeBSD
CXX ?= c++
CXXFLAGS ?= -O2 -Wall -std=c++17
teensy_loader_cli: teensy_loader_cli.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -s -DUSE_LIBUSB -o teensy_loader_cli teensy_loader_cli.cpp -lusb $(LDFLAGS)


else ifeq ($(OS), WINDOWS)
CXX ?= clang-cl
CXXFLAGS ?= /O2 /W4 /EHsc /std:c++17 /D_CRT_SECURE_NO_WARNINGS
teensy_loader_cli.exe: teensy_loader_cli.cpp
	$(CXX) $(CXXFLAGS) /DUSE_WIN32 /Fe:$@ teensy_loader_cli.cpp /link hid.lib setupapi.lib winmm.lib


else ifeq ($(OS), MACOSX)
ifeq ($(USE_LIBUSB), YES)
CXX ?= c++
CXXFLAGS ?= -O2 -Wall -std=c++17
teensy_loader_cli: teensy_loader_cli.cpp
	$(CXX) $(CXXFLAGS) -s -DUSE_LIBUSB -DMACOSX -o teensy_loader_cli teensy_loader_cli.cpp -lusb -I /usr/local/include -L/usr/local/lib
	 
else
CXX ?= c++
SDK ?= $(shell xcrun --show-sdk-path)
#SDK ?= /Developer/SDKs/MacOSX10.6.sdk  # the old way...
#SDK = /Developer_xcode32/SDKs/MacOSX10.5.sdk  # the very old way!
#CC = /Developer_xcode32/usr/bin/gcc-4.0
#CFLAGS = -O2 -Wall -arch i386 -arch ppc
CXXFLAGS ?= -O2 -Wall -std=c++17
teensy_loader_cli: teensy_loader_cli.cpp
ifeq ($(SDK),)
	$(error SDK was not found. To use this type of compilation please install Xcode)
endif
	$(CXX) $(CXXFLAGS) -DUSE_APPLE_IOKIT -isysroot $(SDK) -o teensy_loader_cli teensy_loader_cli.cpp -Wl,-syslibroot,$(SDK) -framework IOKit -framework CoreFoundation

endif

else ifeq ($(OS), BSD)  # works on NetBSD and OpenBSD
CXX ?= c++
CXXFLAGS ?= -O2 -Wall -std=c++17
teensy_loader_cli: teensy_loader_cli.cpp
	$(CXX) $(CXXFLAGS) -s -DUSE_UHID -o teensy_loader_cli teensy_loader_cli.cpp


endif


clean:
	rm -f teensy_loader_cli teensy_loader_cli.exe*
