CXXFLAGS += -std=c++11
INCS = `pkg-config --cflags opencv`
LIBS = `pkg-config --libs opencv` -lboost_filesystem -lboost_system


annotator: main.o
	$(CXX) $(CXXFLAGS) -o $@ $< $(LIBS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $< $(INCS)
