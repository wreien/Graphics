CC=clang
CXX=clang++

DEPDIR = dep
OBJDIR = obj
$(shell mkdir -p $(DEPDIR) $(OBJDIR) >/dev/null)

DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td
CXXFLAGS = -c -stdlib=libc++ -std=c++1z -W{all,extra,error,no-unused-parameter} -pedantic -isystem ./lib/include
LDFLAGS = -stdlib=libc++ -lglfw -ldl

POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

SRCS = main.cpp shader.cpp
OBJS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS))

all : graphics

debug : CXXFLAGS += -DDEBUG -g
debug : LDFLAGS += -g
debug : graphics

graphics : $(OBJS) lib/src/glad.o
	$(CXX) $(LDFLAGS) $^ -o $@

lib/src/glad.o : lib/src/glad.c
	$(CC) -c -O2 -isystem ./lib/include $< -o $@

$(OBJDIR)/%.o : %.cpp $(DEPDIR)/%.d
	$(CXX) $(CXXFLAGS) $(DEPFLAGS) $< -o $@
	$(POSTCOMPILE)

$(DEPDIR)/%.d : ;
.PRECIOUS : $(DEPDIR)/%.d

.PHONY : clean
clean :
	rm -f lib/src/glad.o graphics
	rm -rd $(DEPDIR) $(OBJDIR)

include $(wildcard $(patsubst %, $(DEPDIR)/%.d, $(basename $(SRCS))))
