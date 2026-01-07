# Use compiler provided by Yocto
CXX ?= $(CXX)
CXXFLAGS ?= $(CXXFLAGS)
LDFLAGS ?= $(LDFLAGS)

TARGET = game_console
SRCS = game_console.cpp
OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)
