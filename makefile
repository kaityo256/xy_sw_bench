TARGET=xy3d
all: $(TARGET)
CC=mpicxx
CPPFLAGS=-O3 
SRC=$(shell ls *.cpp)
OBJ=$(SRC:.cpp=.o)
.SUFFIXES: .cpp .h. .o

main.o: util.h

$(TARGET): $(OBJ)
	$(CC) $(CPPFLAGS) $(LDFLAGS) -o $(TARGET) $(OBJ)

%.o : %.cpp
	$(CC) $(CPPFLAGS) -c $< 

clean:
	rm -f $(TARGET) $(OBJ)

tar:
	rm -rf xy_sw_bench xy_sw_bench.tar.gz
	mkdir xy_sw_bench
	cp *.cpp *.h makefile README.md xy_sw_bench
	tar cvzf xy_sw_bench.tar.gz xy_sw_bench

-include makefile.opt
