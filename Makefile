TARGET=		fsop.out
OBJECTS=	$(wildcard *.cpp *.cxx */*.cpp */*.cxx)
CXXFLAGS=	-std=c++11 -WAll -Wextra -g

.PHONY: clean

${TARGET}: ${OBJECTS}
	${CC} ${LDFLAGS} $^ -o $@

%.o: %.cpp
	${CC} ${CXXFLAGS} -c $^ -o $@

clean:
	@echo -n "[>] Removing ${TARGET} & intermediate object files ($(OBJECTS)) ... "
	@rm -rf ${TARGET} ${OBJECTS}
	@echo "done"