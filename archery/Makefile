INC = -L/usr/X11R6/lib
LDLIBS = -lglut -lGL -lGLU -lX11 -lm -lpng

CXX = g++
CXXFLAGS = -Wall -g -O3
LDFLAGS = $(CPPFLAGS) $(INC)

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
