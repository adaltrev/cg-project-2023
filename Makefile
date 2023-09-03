CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

ASSIGN: main.cpp
	g++ $(CFLAGS) -Iheaders -o bin main.cpp $(LDFLAGS)

ASSIGN1: MeshLoader.cpp
	g++ $(CFLAGS) -Iheaders -o mbin MeshLoader.cpp $(LDFLAGS)

ASSIGN2: copy.cpp
	g++ $(CFLAGS) -Iheaders -o cbin copy.cpp $(LDFLAGS)

.PHONY: test clean mesh copy

test: ASSIGN
	./bin

mesh: ASSIGN1
	./mbin

copy: ASSIGN2
	./cbin

clean:
	rm -f bin
	rm -f mbin
	rm -f cbin
