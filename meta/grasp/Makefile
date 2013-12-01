all: g_test

g_test: main.cc ImageSeg.hpp SolGen.hpp
	$(CXX) -o $@ $^ -lopencv_core -lopencv_imgproc -lopencv_highgui

relase: main.cc ImageSeg.hpp SolGen.hpp
	$(CXX) -o $@ $^ -lopencv_core -lopencv_imgproc -lopencv_highgui -O3

clean:
	@rm -rf g_test relase