# Compiler is G++
CXX = g++



LIBDIR = .
SOCKETLIBDIR = $(LIBDIR)/SocketLib
BASICLIBDIR  = $(LIBDIR)/BasicLib

# libraries
LIBS = -lpthread -ldl -lutil

# flags
CFLAGS = -I$(LIBDIR)

link: $(wildcard *.cpp)
	$(CXX) $(CFLAGS) $(THREADLIBDIR)/*.cpp -c;
	$(CXX) $(CFLAGS) $(SOCKETLIBDIR)/*.cpp -c;
	$(CXX) $(CFLAGS) $(BASICLIBDIR)/*.cpp -c;
	$(CXX) $(CFLAGS) *.cpp *.o
