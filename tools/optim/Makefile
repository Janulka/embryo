TARGET = embryo-optim

SRC_FILES = \
./src/cmaes.cpp \
./src/exception.cpp \
./src/Main.cpp \
./src/random.cpp \
./src/readpara.cpp \
./src/timings.cpp \
../view/src/ChemicalAspect.cpp \
../view/src/ColourAspect.cpp \
../view/src/EmbryoAspect.cpp \
../view/src/EmbryoView.cpp \
../view/src/StateAspect.cpp \
../view/src/Widget.cpp \
../view/src/WidgetContainer.cpp
#./src/MainNoGui.cpp

OBJECTS = $(patsubst %.cpp,%.o,$(SRC_FILES))

.PHONY: all clean cleaner install

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@  $(OBJECTS) $(LDFLAGS) -L$(LIB_EMBRYO_PATH) -lembryo
CAL_DIRECTORY = $(TOP_DIRECTORY)/tools/optim


%.o:%.cpp
	$(CXX) -c -fPIC $(INCLUDES) -I$(TOP_DIRECTORY)/libembryo/include -I$(TOP_DIRECTORY)/tools/view/include $(CXXFLAGS) $(OPTIM_FLAGS) -o $@ $<

clean:
	rm -f ./src/*.o

cleaner:
	rm -f $(TARGET)

# compile: $(OBJS)
#libr2e.a : $(OBJS) ar -cru libr2e.a $(OBJS)

dist_files:
	ls -1 $(LOCAL_DIRECTORY)/include/*.hpp $(LOCAL_DIRECTORY)/src/*.cpp $(LOCAL_DIRECTORY)/Makefile >> $(DIST_FILES_LIST)
