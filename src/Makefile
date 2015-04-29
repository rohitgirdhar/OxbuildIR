BOOST_ROOT=/home/rgirdhar/data/Software/cpp/boost_1_57_0/
BOOST_LIBS=/home/rgirdhar/data/Software/cpp/boost_1_57_0/stage/libs/
CXXFLAGS+=-std=c++0x -O2 `pkg-config --cflags opencv`
BOOST_LIBS=-L/usr/local/lib
INC_BOOST=-I${BOOST_ROOT}
LIBS=$(BOOST_LIBS) `pkg-config --libs opencv`
LDFLAGS=-lboost_program_options -lboost_filesystem -lboost_system -lopencv_core -lopencv_imgproc -lopencv_highgui -lopencv_ml -lopencv_video -lopencv_features2d -lopencv_calib3d -lopencv_objdetect -lopencv_stitching

SRCS=buildIndex.cpp getClosest.cpp
OBJS=$(patsubst %.cpp,%.o,$(SRCS))

all: getClosest buildIndex

depend: .depend

.depend: $(SRCS)
	rm -rf ./.depend
	$(CXX) $(CXXFLAGS) -MM $^ -MF ./.depend

include .depend

%: %.cpp
	$(CXX) $(CXXFLAGS) $(INC_BOOST) -o $@ $< $(LIBS) $(LDFLAGS)

clean:
	rm -rf buildIndex getClosest $(OBJS)
