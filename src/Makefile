CXX=g++ -std=c++0x
CXXFLAGS=-O2
BOOST_LIBS=-L/usr/local/lib
INC_BOOST=-I${BOOST_ROOT}
LIBS=$(BOOST_LIBS)
LDFLAGS=-lboost_program_options -lboost_filesystem -lboost_system

all: buildIndex getClosest

%: %.cpp
	$(CXX) $(CXXFLAGS) $(INC_BOOST) -o $@ $< $(LIBS) $(LDFLAGS) 

clean: 
	rm -rf buildIndex getClosest
