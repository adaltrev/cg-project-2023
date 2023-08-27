CFLAGS = -std=c++17 -O2
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

ASSIGN: main.cpp
	g++ $(CFLAGS) -o bin main.cpp $(LDFLAGS)

.PHONY: test clean

test: ASSIGN
	./bin

clean:
	rm -f bin
