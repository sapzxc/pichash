
CPPFLAGS =  -O2 -I/usr/local/include -Dcimg_use_jpeg -Dcimg_use_png
CXX = g++
LDFLAGS =  -O2 -L/usr/local/lib -ljpeg -lpng

all: pichash

pichash: pichash.cpp
	$(CXX) $(CPPFLAGS) $(LDFLAGS) pichash.cpp -o pichash
