#!/usr/bin/env bash

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
SOURCES=( )
while IFS= read -r -d $'\0'; do
    SOURCES+=("$REPLY")
done < <(find $SRCDIR -type f -name "*.cpp" -print0)
# C++ flags to use during compilation.
CXXFLAGS="-std=c++17 -Wall -Wextra -g"
# Preprocessor flags to use during compilation.
CPPFLAGS="-MMD -MP -MF <depfile>"
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
    OBJECTS=()
    for source in "${SOURCES[@]}"; do
        eval "object=\${source/$SRCDIR/$LIBDIR}"
        object="${object/%.cpp/.o}"
        eval "depend=\${source/$SRCDIR/$DEPDIR}"
        depend="${depend/%.cpp/.d}"
        eval "cppflags=\${CPPFLAGS/<depfile>/$depend}"

        mkdir -p "$(dirname $object)"
        mkdir -p "$(dirname $depend)"

        OBJECTS+=("$object")
        cmd="$CXX $CXXFLAGS $cppflags ${INCLUDEDIR//include/-Iinclude} -c $source -o $object"
        echo "[>] $cmd"
        eval "\$($cmd)"
    done

    cmd="$CXX ${LIBDIR//lib/-Llib} $LDFLAGS ${OBJECTS[@]} -o $TARGET"
    echo "[>] $cmd"
    eval "\$($cmd)"
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