CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

ASSIGN: main.cpp
	g++ $(CFLAGS) -Iheaders -o bin main.cpp $(LDFLAGS)

ASSIGN1: copy.cpp
	g++ $(CFLAGS) -Iheaders -o cbin copy.cpp $(LDFLAGS)


.PHONY: test clean copy shaders

test: ASSIGN
	./bin

copy: ASSIGN1
	./cbin

clean:
	rm -f bin
	rm -f cbin

shaders:
	glslc shaders/Mesh.frag -o shaders/MeshFrag.spv
	glslc shaders/Mesh.vert -o shaders/MeshVert.spv
	glslc shaders/Overlay.frag -o shaders/OverlayFrag.spv
	glslc shaders/Overlay.vert -o shaders/OverlayVert.spv
	glslc shaders/VColor.frag -o shaders/VColorFrag.spv
	glslc shaders/VColor.vert -o shaders/VColorVert.spv