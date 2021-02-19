# Thanks to Job Vranish (https://spin.atomicobject.com/2016/08/26/makefile-c-projects/)
TARGET_EXEC := nbody.out

BUILD_DIR := ./build
SRC_DIRS := ./src ./lib

CC = mpicc

# Find all the C and C++ files we want to compile
SRCS := $(shell find $(SRC_DIRS) -name *.cpp -or -name *.c)

# String substitution for every C/C++ file.
# As an example, hello.cpp turns into ./build/hello.cpp.o
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)

# String substitution (suffix version without %).
# As an example, ./build/hello.cpp.o turns into ./build/hello.cpp.d
DEPS := $(OBJS:.o=.d)

# Every folder in ./src will need to be passed to GCC so that it can find header files
INC_DIRS := $(shell find $(SRC_DIRS) -type d)
# Add a prefix to INC_DIRS. So moduleA would become -ImoduleA. GCC understands this -I flag
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

use_omp := true

# If we want to use OpenMP, add in the appropriate directives.
ifeq ($(use_omp),true)
OMP_FLAG := -DUSE_OMP -fopenmp
else ifeq ($(use_omp),false)
OMP_FLAG := 
endif

ifeq ($(debug),true)
DEBUG := -g -DDEBUG_MODE
else ifeq ($(debug),mpi)
DEBUG := -g -DDEBUG_MODE_MPI
else
DEBUG := -O2
endif

# The -MMD and -MP flags together generate Makefiles for us!
# These files will have .d instead of .o as the output.
CPPFLAGS := $(INC_FLAGS) -std=c99 -D_POSIX_C_SOURCE=199309L -Wall -MMD -MP $(DEBUG) $(OMP_FLAG)

LDFLAGS := -lm -fopenmp

# The final build step.
$(TARGET_EXEC): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo Type ./$@ to execute the program.

# Build step for C source
$(BUILD_DIR)/%.c.o: %.c
	mkdir -p $(dir $@)
	$(CC) -c $< -o $@ $(CPPFLAGS) $(CFLAGS)

# Build step for C++ source
$(BUILD_DIR)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@


.PHONY: clean
clean:
	-rm -r $(BUILD_DIR)
	-rm $(TARGET_EXEC)

help:
	@echo -----------------------------------------------------------------
	@echo make: Make into nbody.out
	@echo make debug=true: Enable debug \(-g\) support.
	@echo make debug=mpi: Enable debug \(-g\) support for mpi parallel programs.
	@echo make use_omp=true: Make into nbody.out with OpenMP functionality.
	@echo -----------------------------------------------------------------

# Include the .d makefiles. The - at the front suppresses the errors of missing
# Makefiles. Initially, all the .d files will be missing, and we don't want those
# errors to show up.
-include $(DEPS)
