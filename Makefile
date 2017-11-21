CC=clang
CXX=clang++

SRCDIR = src
DEPDIR = .d
OBJDIR = .o
LIBDIR = lib
$(shell mkdir -p $(DEPDIR) $(OBJDIR) >/dev/null)

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
CXXFLAGS = -c -stdlib=libc++ -std=c++1z -W{all,extra,error,no-unused-parameter} -pedantic -isystem ./$(LIBDIR)/include
CFLAGS   = -c -isystem ./$(LIBDIR)/include
LDFLAGS  = -stdlib=libc++ -lglfw -ldl

POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

FILES = main.cpp shader.cpp level.cpp
SRCS  = $(addprefix $(SRCDIR)/, $(FILES))
OBJS  = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRCS))
LIBS  = $(LIBDIR)/src/jsoncpp.o $(LIBDIR)/src/glad.o

all : graphics

debug : CXXFLAGS += -DDEBUG -g
debug : LDFLAGS += -g
debug : graphics

graphics : $(OBJS) $(LIBS)
	$(CXX) $(LDFLAGS) $^ -o $@

$(LIBDIR)/src/glad.o : $(LIBDIR)/src/glad.c
	$(CC) $(CFLAGS) $< -o $@

$(LIBDIR)/src/%.o : $(LIBDIR)/src/%.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp $(DEPDIR)/%.d
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) $< -o $@
	$(POSTCOMPILE)

$(DEPDIR)/%.d : ;
.PRECIOUS : $(DEPDIR)/%.d

.PHONY : clean fullclean
clean :
	rm -f graphics
	rm -rd $(DEPDIR) $(OBJDIR)

fullclean : clean
	find . -name '*.o' -delete

include $(wildcard $(patsubst %, $(DEPDIR)/%.d, $(basename $(SRCS))))
