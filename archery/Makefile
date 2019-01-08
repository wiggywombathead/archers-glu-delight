GLEW =/modules/cs324/glew-1.11.0

INCDIRS = -I$(GLEW)/include
LIBDIRS = -L/usr/X11R6/lib -L$(GLEW)/lib
LDLIBS = -lglut -lGL -lGLU -lX11 -lm -lpng -lGLEW

CXX = g++
CXXFLAGS = -g -std=gnu++11 -O3 $(INCDIRS)
LDFLAGS = $(CXXFLAGS) $(LIBDIRS) -Wl,-rpath,$(GLEW)/lib

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:%.cpp=%.o)

%: %.o
	$(CXX) $(LDFLAGS) $(LDLIBS) $< -o $@

%.o: %.cpp
	$(CXX) $(LDFLAGS) $(LDLIBS) -c $<

all: $(OBJS)
	$(CXX) $(LDFLAGS) $(LDLIBS) -o archery $(OBJS)

clean:
	@rm *.o
