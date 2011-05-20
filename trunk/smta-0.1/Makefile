SMTA=smta
PREFIX=../local
CPPFLAGS=-I$(PREFIX)/include -I$(PREFIX)/include/cvc3
LDFLAGS=-static -L$(PREFIX)/lib -L.
LIBS=-lcvc3 -lgmp


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
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@ $< lib$(SMTA).a $(LIBS) 


clean:
	rm -rf *.o *.a *.so test

