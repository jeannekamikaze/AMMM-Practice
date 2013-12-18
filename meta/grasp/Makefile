OPENCV_FLAGS = -lopencv_core -lopencv_imgproc -lopencv_highgui
OMP_FLAGS = -fopenmp -lpthread
CFLAGS = $(OPENCV_FLAGS) $(OMP_FLAGS)
NAME = grasp_seg
GRASP_CODE = main.cc ImageSeg.hpp
META_CODE = ../ObjFunc.h ../ParseDatFile.hpp ../SolGen.hpp

all: grasp_seg

grasp_seg: $(GRASP_CODE)
	$(CXX) -o $(NAME) $^ $(CFLAGS)

relase: $(GRASP_CODE)
	$(CXX) -o $(NAME) $^ -O3 $(CFLAGS)

clean:
	@rm -rf $(NAME)