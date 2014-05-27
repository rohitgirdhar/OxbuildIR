CXX=g++ -std=c++0x
CXXFLAGS=-O2 `pkg-config --cflags opencv`
BOOST_LIBS=-L/usr/local/lib
INC_BOOST=-I${BOOST_ROOT}
LIBS=$(BOOST_LIBS) `pkg-config --libs opencv`
LDFLAGS=-lboost_program_options -lboost_filesystem -lboost_system

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
