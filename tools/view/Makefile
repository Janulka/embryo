TARGET = embryo-view

SRC_FILES = \
./src/ChemicalAspect.cpp \
./src/ColourAspect.cpp \
./src/EmbryoAspect.cpp \
./src/EmbryoView.cpp \
./src/Main.cpp \
./src/StateAspect.cpp \
./src/Widget.cpp \
./src/WidgetContainer.cpp

OBJECTS = $(patsubst %.cpp,%.o,$(SRC_FILES))

.PHONY: all clean cleaner install

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@  $(OBJECTS) $(LDFLAGS) -L$(LIB_EMBRYO_PATH) -lembryo

CAL_DIRECTORY = $(TOP_DIRECTORY)/tools/view

%.o:%.cpp
	$(CXX) -c -fPIC $(INCLUDES) -I$(TOP_DIRECTORY)/libembryo/include $(CXXFLAGS) $(OPTIM_FLAGS) -o $@ $<

clean:
	rm -f ./src/*.o

cleaner:
	rm -f $(TARGET)

dist_files:
	ls -1 $(LOCAL_DIRECTORY)/include/*.hpp $(LOCAL_DIRECTORY)/src/*.cpp $(LOCAL_DIRECTORY)/Makefile >> $(DIST_FILES_LIST)
