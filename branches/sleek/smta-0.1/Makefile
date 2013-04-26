SMTA=smta
PREFIX=..
CPPFLAGS=-I$(PREFIX)/include
LDFLAGS=-L. -L$(PREFIX)/lib
#LIBS=-lz3 -lcvc3 -lgmp
#LIBS=$(PREFIX)/lib/libz3.a
LIBS=-lz3 #-lgmp


all: libraries

install: libraries
	cp lib$(SMTA).a $(PREFIX)/lib
	cp $(SMTA).h $(PREFIX)/include

libraries: lib$(SMTA).a

lib$(SMTA).a: $(SMTA).o
	#ar rc $@ $< $(LIBS)
	ar rc $@ $<
	
$(SMTA).o: $(SMTA).cpp $(SMTA).h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -fPIC -c -o $@ $(SMTA).cpp 


check: test
	./test

test: test.cpp libraries
	$(CXX) -fopenmp $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ $< lib$(SMTA).a $(LIBS)

clean:
	rm -rf *.o *.a *.so test

