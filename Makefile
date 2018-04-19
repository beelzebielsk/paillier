# The following all worked on my machine:
#g++ -g $< -o $@ -lntl -lgmp
#g++ -g $< -o $@ -lntl
#g++ -g $< -o $@
#

# - ntl: Number theory library
# - gmp: Arbitrary precision arithmetic.
# - ssl + crytpo: For openssl. Installed on most linux machines, and
#   has hashing algorithms. In fact, I think we can get away with just
#   'crypto' for hashing, since ssl ought to be for communication
#   (which we're not doing).
# - m: From what I can tell, libm is an implementation of C math
#   functions. Why would we need this?
LIBS:=ntl gmp m ssl crypto
LIBFLAGS:=$(addprefix -l, $(LIBS))
INCLUDES:=include
INCLUDEFLAGS:=$(addprefix -I, $(INCLUDES))
TARGETS:= main hello_world parser-example parser rms-parser

.PHONY : all clean
all : $(TARGETS)

main : main.cpp paillier.cpp
	g++ -g -Wall -Wpedantic $^ -o $@ $(LIBFLAGS)

hello_world : hello_world.cpp
	g++ -g -Wall -Wpedantic $^ -o $@ $(LIBFLAGS) $(INCLUDEFLAGS)

clean :
	rm $(TARGETS)
