all: grasp_seg

grasp_seg: main.cc ImageSeg.hpp SolGen.hpp
	$(CXX) -o $@ $^ -lopencv_core -lopencv_imgproc -lopencv_highgui

relase: main.cc ImageSeg.hpp SolGen.hpp
	$(CXX) -o grasp_seg $^ -lopencv_core -lopencv_imgproc -lopencv_highgui -O3

clean:
	@rm -rf grasp_seg