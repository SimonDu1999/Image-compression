all:
	g++ main.cpp encoder.cpp huffman.cpp decoder.cpp -o cmpimage -lSDL2
clean:
	rm cmpimage