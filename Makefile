CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

ASSIGN: main.cpp
	g++ $(CFLAGS) -Iheaders -o bin main.cpp $(LDFLAGS)

LOADER: MeshLoader.cpp	
	g++ $(CFLAGS) -Iheaders -o mbin MeshLoader.cpp $(LDFLAGS)

.PHONY: test clean mesh

test: ASSIGN
	./bin

mesh: LOADER
	./mbin

clean:
	rm -f bin
	rm -f mbin
