TARGET = tsuki
CXX = g++
CXXFLAGS = -Wall -Iinclude -g
LDFLAGS = -lreadline
SRCDIR = src
OBJDIR = obj

SOURCES = $(wildcard $(SRCDIR)/*.cpp) \
					$(wildcard $(SRCDIR)/expressions/*.cpp) \
					$(wildcard $(SRCDIR)/stmt/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR):
	mkdir -p $(OBJDIR)

clean:
	rm -f $(OBJECTS) $(TARGET)
	rm -rf $(OBJDIR)

.PHONY: all clean
