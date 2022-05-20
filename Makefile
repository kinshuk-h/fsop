TARGET=		fsop.out
SOURCES=	$(wildcard *.cpp *.cxx */*.cpp */*.cxx)
OBJECTS=    $(SOURCES:.cpp=.o)
CXXFLAGS=	-std=c++11 -Wall -Wextra -g

.PHONY: clean

${TARGET}: ${OBJECTS}
	${CC} ${LDFLAGS} $^ -o $@

%.o: %.cpp
	${CC} ${CXXFLAGS} -c $^ -o $@

clean:
	@echo -n "[>] Removing ${TARGET} & intermediate object files ($(OBJECTS)) ... "
	@rm -rf ${TARGET} ${OBJECTS}
	@echo "done"