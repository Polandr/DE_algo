CXX = mpicxx
CXX_FLAGS = -g -I./src/

ALGORITHM = algorithm

SRC_DIR = src
CHROMOSOMES_DIR = species
# Choose one of the next value according to the problem and set to the variable CHROMOSOME
# sphere_model , rosenbrock , rastrigin
CHROMOSOME = sphere_model
HDRS = types.h parameters.h parallel.h factory.h randomizer.h eventer.h operators.h chromosome.h $(CHROMOSOME).h algorithm.h
SRC = experiment.cpp my_test.cpp parameters.cpp parallel.cpp factory.cpp randomizer.cpp eventer.cpp operators.cpp $(CHROMOSOME).cpp $(ALGORITHM).cpp

OBJ_DIR = objs
OBJS = parameters.o parallel.o factory.o randomizer.o eventer.o operators.o $(CHROMOSOME).o algorithm.o

first: experiment

experiment: $(OBJS) experiment.o
	$(CXX) $(CXX_FLAGS) -o experiment $(OBJ_DIR)/*.o

my_test: $(OBJS) my_test.o
	$(CXX) $(CXX_FLAGS) -o my_test $(OBJ_DIR)/*.o

$(SRC): $(SRC_DIR)/$(@F)

$(HDRS): $(SRC_DIR)/$(@F)

experiment.o: parameters.o algorithm.o parallel.h experiment.cpp
	$(CXX) $(CXX_FLAGS) -c $(SRC_DIR)/experiment.cpp -o $(OBJ_DIR)/$@

my_test.o: parameters.o algorithm.o types.h parallel.h factory.h my_test.cpp
	$(CXX) $(CXX_FLAGS) -c $(SRC_DIR)/my_test.cpp -o $(OBJ_DIR)/$@

parameters.o: parallel.h parameters.h parameters.cpp
	$(CXX) $(CXX_FLAGS) -c $(SRC_DIR)/parameters.cpp -o $(OBJ_DIR)/$@

parallel.o: parallel.h parallel.cpp
	$(CXX) $(CXX_FLAGS) -c $(SRC_DIR)/parallel.cpp -o $(OBJ_DIR)/$@

factory.o: parameters.o types.h operators.h chromosome.h factory.h factory.cpp
	$(CXX) $(CXX_FLAGS) -c $(SRC_DIR)/factory.cpp -o $(OBJ_DIR)/$@

randomizer.o: types.h parallel.h randomizer.h randomizer.cpp
	$(CXX) $(CXX_FLAGS) -c $(SRC_DIR)/randomizer.cpp -o $(OBJ_DIR)/$@

eventer.o: eventer.h eventer.cpp
	$(CXX) $(CXX_FLAGS) -c $(SRC_DIR)/eventer.cpp -o $(OBJ_DIR)/$@

operators.o: parameters.o types.h parallel.h randomizer.h operators.h operators.cpp
	$(CXX) $(CXX_FLAGS) -c $(SRC_DIR)/operators.cpp -o $(OBJ_DIR)/$@

$(CHROMOSOME).o: parameters.o types.h randomizer.h parallel.h chromosome.h $(CHROMOSOME).h $(CHROMOSOME).cpp
	$(CXX) $(CXX_FLAGS) -c $(SRC_DIR)/$(CHROMOSOMES_DIR)/$(CHROMOSOME).cpp -o $(OBJ_DIR)/$@

algorithm.o: parameters.o types.h chromosome.h operators.h factory.h randomizer.h parallel.h eventer.h algorithm.h $(ALGORITHM).cpp
	$(CXX) $(CXX_FLAGS) -c $(SRC_DIR)/$(ALGORITHM).cpp -o $(OBJ_DIR)/$@

clean:
	rm -f experiment my_test
	rm -f $(OBJ_DIR)/*.o

.PHONY: first clean $(SRC) $(HDRS)
