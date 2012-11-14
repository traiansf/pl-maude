SMTA=smta
PREFIX=../local
CPPFLAGS=-I$(PREFIX)/include \
  -I$(PREFIX)/include/cvc3 \
  -I$(PREFIX)/z3/src/api -I$(PREFIX)/z3/src/api/c++
LDFLAGS=-static -L. -L$(PREFIX)/lib -L$(PREFIX)/z3/build
LIBS=-lz3 -lcvc3 -lgmp


all: libraries

install: libraries
	cp lib$(SMTA).a $(PREFIX)/lib
	cp $(SMTA).h $(PREFIX)/include

libraries: lib$(SMTA).a

lib$(SMTA).a: $(SMTA).o
	ar rc $@ $<
	
$(SMTA).o: $(SMTA).cpp $(SMTA).h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -fPIC -c -o $@ $(SMTA).cpp 


check: test
	./test

test: test.cpp libraries
	$(CXX) -fopenmp $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ $< lib$(SMTA).a $(LIBS) 


clean:
	rm -rf *.o *.a *.so test

