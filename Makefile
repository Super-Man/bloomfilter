#
# Open Bloom Filter Master MakeFile
# By Arash Partow - 2000
#
# URL: http://www.partow.net/programming/hashfunctions/index.html
#
# Note: You will need to download the C++ General Hash Functions library
#
# Copyright Notice:
# Free use of this library is permitted under the guidelines 
# and in accordance with the most current version of the 
# Common Public License.
# http://www.opensource.org/licenses/cpl1.0.php
#

COMPILER         = -c++
OPTIMIZATION_OPT = -O3
OPTIONS          = -ansi -pedantic -Wall -Werror $(OPTIMIZATION_OPT) -o
OPTIONS_LIBS     = -ansi -pedantic -Wall -Werror $(OPTIMIZATION_OPT) -c

CPP_SRC =

%.o: %.h %.cpp
	$(COMPILER) $(OPTIONS_LIBS) $*.cpp -o $@

OBJECTS = $(CPP_SRC:.cpp=.o)

all: $(OBJECTS) BloomFilterTest

BloomFilterTest: $(OBJECTS) bloom_filter.hpp BloomFilterTest.cpp
	$(COMPILER) $(OPTIONS) BloomFilterTest BloomFilterTest.cpp $(OBJECTS)

clean:
	rm -f core *.o *.bak *stackdump *#

#
# The End !
#
