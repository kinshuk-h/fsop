FIND	:=	F:/msys64/usr/bin/find.exe
MKDIR	:=	F:/msys64/usr/bin/mkdir.exe

# Directory containing source files.
SRCDIR		:=	src
# Directory containing header files.
INCLUDEDIR	:=	include
# Directory containing dependency files.
DEPDIR      :=	deps
# Directory containing static libraries and object files.
LIBDIR		:=	lib

# Target executable to generate.
TARGET		:=	fsop.out
# Source files to be compiled.
SOURCES		:=	$(shell $(FIND) $(SRCDIR) -type f -name "*.cpp")
# Compiled object files corresponding to source files.
OBJECTS		:=	$(SOURCES:$(SRCDIR)/%.cpp=$(LIBDIR)/%.o)
# Dependency files containing header dependencies (compiler generated).
DEPS		:=	$(SOURCES:$(SRCDIR)/%.cpp=$(DEPDIR)/%.d)
# C++ flags to use during compilation.
CXXFLAGS	:=	-std=c++17 -Wall -Wextra -g
# Preprocessor flags to use during compilation.
CPPFLAGS    = -MMD -MP -MF $(@:$(LIBDIR)/%.o=$(DEPDIR)/%.d)
# C++ compiler to use.
CXX			:=	g++

# Base rule: Generate target using object files.
$(TARGET): $(OBJECTS)
	@echo -n "[>] "
	$(CXX) $(LIBDIR:%=-L%) $(LDFLAGS) $^ -o $@

.SECONDEXPANSION:
# Substitution rule: generate object files using sources.
$(LIBDIR)/%.o: $(SRCDIR)/%.cpp
	@$(MKDIR) -p $(@D)
	@$(MKDIR) -p $(@D:$(LIBDIR)%=$(DEPDIR)%)
	@echo -n "[>] "
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDEDIR:%=-I%) -c $< -o $@

-include $(DEPS)

# Phony rule: Run the target executable.
.PHONY: run
run: $(TARGET)
	@$(TARGET) $(args)

# Phony rule: Clean build files for a fresh build.
.PHONY: clean
clean:
	@echo -n "[>] Removing $(TARGET) ... "
	@rm -f $(TARGET)
	@echo "done"
	@echo -n "[>] Removing intermediate object files in ($(LIBDIR)) ... "
	@rm -rf $(LIBDIR)
	@echo "done"
	@echo -n "[>] Removing old dependency files in ($(DEPDIR)) ... "
	@rm -rf $(DEPDIR)
	@echo "done"