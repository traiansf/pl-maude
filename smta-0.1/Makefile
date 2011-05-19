SMTA=smta
PREFIX=../local
CXXFLAGS=-O2 -Wall -m32 -mtune=i386 -march=i386
CPPFLAGS=-I$(PREFIX)/include -I$(PREFIX)/include/cvc3
LDFLAGS=-L$(PREFIX)/lib -L.
LIBS=-l$(SMTA) -lcvc3 -lgmp


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

test: test.cpp 
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ $< $(LIBS) 


clean:
	rm -rf *.o *.a *.so test

