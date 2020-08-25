#Name: Antonio Rios
#Email: alr150630@utdallas.edu
#Course Number: CS 3376.001

#Define our variables here.
CXX = g++
CPPFLAGS = -Wall -O -std=c++11 -pthread # -I /home/012/a/al/alr150630/include
#LDLIBS = -L /home/012/a/al/alr150630/lib
#LDFLAGS = -l curses -l cdk -l boost_thread-mt -l boost_system

EXEC = ProjectTwo
CCFILE = $(wildcard *.cpp)
OFILE = $(CCFILE:.cpp=.o)


#Create the executable file.
$(EXEC): $(OFILE)
	$(CXX) $(CCFILE) $(CPPFLAGS) -o $(EXEC)

#Remove unnecessary files.
clean:
	rm -f *.o *~ ProjectTwo
