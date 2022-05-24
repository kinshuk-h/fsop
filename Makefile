FIND := F:/msys64/usr/bin/find.exe

TARGET		:=	fsop.out
SOURCES		:=	$(shell $(FIND) -type f -name "*.cpp")
INCLUDEDIRS	:=	./include
LIBDIRS		:=	./lib
OBJECTS		:=	$(SOURCES:src/%.cpp=lib/%.o)
CXXFLAGS	:=	-std=c++17 -Wall -Wextra -g
CC          :=	g++

$(TARGET): $(OBJECTS)
	@echo -n "[>] "
	$(CC) $(LIBDIRS:%=-L%) $(LDFLAGS) $^ -o $@

lib/%.o: src/%.cpp $(INCLUDEDIRS)
	@echo -n "[>] "
	$(CC) $(CXXFLAGS) $(INCLUDEDIRS:%=-I%) -c $< -o $@

.PHONY: run
run: $(TARGET)
	@$(TARGET) $(args)

.PHONY: clean
clean:
	@echo -n "[>] Removing $(TARGET) & intermediate object files ($(OBJECTS)) ... "
	@rm -rf $(TARGET) $(OBJECTS)
	@echo "done"