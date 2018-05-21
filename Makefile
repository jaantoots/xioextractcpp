CC = g++
CFLAGS = --std=c++11 -O2 -Wall
DEPS = slip.h osc_bundle.h osc_message.h osc_types.h
OBJ = xio2csv.o slip.o osc_bundle.o osc_message.o

xio2csv.out: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm *.o *~
