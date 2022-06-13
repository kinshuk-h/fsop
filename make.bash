# Directory containing source files.
SRCDIR=src
# Directory containing header files.
INCLUDEDIR=include
# Directory containing dependency files.
DEPDIR=deps
# Directory containing static libraries and object files.
LIBDIR=lib

# Target executable to generate.
TARGET=fsop.out
# Source files to be compiled.
SOURCES=$(find $SRCDIR -type f -name "*.cpp")
# Compiled object files corresponding to source files.
#OBJECTS="${SOURCES/$(SRCDIR)/%.cpp/$(LIBDIR)/%.o}"
# Dependency files containing header dependencies (compiler generated).
#DEPS="$(SOURCES:$(SRCDIR)/%.cpp=$(DEPDIR)/%.d)"
# C++ flags to use during compilation.
#CXXFLAGS="-std=c++17 -Wall -Wextra -g"
# Preprocessor flags to use during compilation.
CPPFLAGS="-MMD -MP -MF" #$(@:$(LIBDIR)/%.o=$(DEPDIR)/%.d)
# C++ compiler to use.
CXX=g++

show_help()
{
    echo "usage: $0 [{all,clean,docs}]\n"
    echo "  all         build the executable, along with "
    echo "              intermediate files"
    echo "  clean       clean the objects files and "
    echo "              target generated in previous builds"
    echo "  docs        create docs using doxygen"
    echo ""
}

make_docs()
{
    doxygen Doxyfile
}

make_clean()
{
    echo -n "[>] Removing $TARGET ... "
    rm -f $TARGET
    echo "done"
    echo -n "[>] Removing intermediate object files in ($LIBDIR) ... "
    rm -rf $LIBDIR
    echo "done"
    echo -n "[>] Removing old dependency files in ($DEPDIR) ... "
    rm -rf $DEPDIR
    echo "done"
}

make_target()
{
    echo $SOURCES
    # Base rule: Generate target using object files.
#    echo -n "[>] "
#    $(CXX) $(LIBDIR:%=-L%) $(LDFLAGS) $^ -o $@

# Substitution rule: generate object files using sources.
#   $(LIBDIR)/%.o: $(SRCDIR)/%.cpp
#    @$(MKDIR) -p $(@D)
#    @$(MKDIR) -p $(@D:$(LIBDIR)%=$(DEPDIR)%)
#    @echo -n "[>] "
#    $(CXX) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDEDIR:%=-I%) -c $< -o $@
}

case "${1:-all}" in
    "-h"|"--help")
        show_help
        ;;
    docs)
        make_docs
        ;;
    clean)
        make_clean
        ;;
    all|fsop.out)
        make_clean
        make_target
        ;;
    *)
        echo "$0: *** No rule to make target '$1'.  Stop."
        ;;
esac