projectPath   := ..
INCLUDES      := $(projectPath)
INCLUDE_FLAGS := $(addprefix -I, $(INCLUDES))
LIBS          := ntl gmp m ssl crypto
LIBFLAGS      := $(addprefix -l, $(LIBS))
SOURCES       := $(wildcard *.cpp)
TARGETS       := $(SOURCES:.cpp=)
PAILLIER_SOURCES := $(wildcard $(projectPath)/paillier/*.cpp)
INTERPRETER_SOURCES := $(wildcard $(projectPath)/interpreter/*.cpp)

all : $(TARGETS)

pir_client : pir_client.cpp $(PAILLIER_SOURCES)
	g++ -g $^ -o $@ $(INCLUDE_FLAGS) $(LIBFLAGS)

pir_server : pir_server.cpp $(PAILLIER_SOURCES)
	g++ -g $^ -o $@ $(INCLUDE_FLAGS) $(LIBFLAGS)


.PHONY : all clean test
test :
	echo $(projectPath)
	echo $(INCLUDE_FLAGS)
	echo $(LIBFLAGS)
	echo $(SOURCES)
	echo $(TARGETS)
	echo $(PAILLIER_SOURCES)
clean :
	rm $(TARGETS)
