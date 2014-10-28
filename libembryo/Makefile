TARGET = libembryo.so

SRC_FILES = \
./src/ArrayOps.cpp \
./src/Controller.cpp \
./src/Cell.cpp \
./src/Embryo.cpp \
./src/EmbryoListener.cpp \
./src/EnergyMonitor.cpp \
./src/Exception.cpp \
./src/FixedMonitor.cpp \
./src/GaussianDistribution.cpp \
./src/Lexer.cpp \
./src/MapTree.cpp \
./src/MapTreeParser.cpp \
./src/MLPController.cpp \
./src/Monitor.cpp \
./src/Object.cpp \
./src/ObjectFactory.cpp \
./src/ObjectFactoryDealer.cpp \
./src/PGM.cpp \
./src/Picture.cpp \
./src/Randomizer.cpp \
./src/Symbol.cpp \
./src/ESNController.cpp \
./src/AspectCell.cpp
#./src/Evaluator.cpp \

OBJECTS = $(patsubst %.cpp,%.o,$(SRC_FILES))

.PHONY: all clean cleaner install

all: $(TARGET) embryo.pc

$(TARGET): $(OBJECTS)
	$(CXX) -shared -Wl,-soname,$@ -o $@  $(OBJECTS) $(LDFLAGS)

%.o:%.cpp
	$(CXX) -c -fPIC $(INCLUDES) $(CXXFLAGS) $(OPTIM_FLAGS) -o $@ $<

embryo.pc:embryo.pc.in
	sed -e 's#@version@#$(VERSION)#g' \
            -e 's#@prefix@#$(INSTALL_DIRECTORY)#g' \
            $< > $@

clean:
	rm -f ./src/*.o ./src/*.*~

cleaner:
	rm -f $(TARGET) embryo.pc

dist_files:
	ls -1 $(LOCAL_DIRECTORY)/embryo.pc.in $(LOCAL_DIRECTORY)/include/*.hpp $(LOCAL_DIRECTORY)/include/embryo/*.hpp $(LOCAL_DIRECTORY)/src/*.cpp $(LOCAL_DIRECTORY)/Makefile >> $(DIST_FILES_LIST)

install:
	cp $(TARGET) $(INSTALL_DIRECTORY)/lib
	cp embryo.pc $(INSTALL_DIRECTORY)/lib/pkgconfig

uninstall:
	rm -f $(INSTALL_DIRECTORY)/lib/$(TARGET)
	rm -f $(INSTALL_DIRECTORY)/lib/pkgconfig/embryo.pc
