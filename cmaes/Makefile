TARGET = libcmaes.so

SRC_FILES = \
	boundary_transformation.c \
	cmaes.c 

OBJECTS = $(patsubst %.c,%.o,$(SRC_FILES))

.PHONY: all clean cleaner install

all: $(TARGET) cmaes.pc

$(TARGET): $(OBJECTS)
	$(CXX) -shared -Wl,-soname,$@ -o $@  $(OBJECTS) $(LDFLAGS)

%.o:%.c
	$(CXX) -c -fPIC $(INCLUDES) $(CXXFLAGS) $(OPTIM_FLAGS) -o $@ $<

cmaes.pc:cmaes.pc.in
	sed -e 's#@version@#$(VERSION)#g' \
            -e 's#@prefix@#$(INSTALL_DIRECTORY)#g' \
            $< > $@

clean:
	rm -f ./src/*.o ./src/*.*~

cleaner:
	rm -f $(TARGET) cmaes.pc

dist_files:
	ls -1 $(LOCAL_DIRECTORY)/cmaes.pc.in $(LOCAL_DIRECTORY)/*.h $(LOCAL_DIRECTORY)/*.c $(LOCAL_DIRECTORY)/Makefile >> $(DIST_FILES_LIST)

install:
	cp $(TARGET) $(INSTALL_DIRECTORY)/lib
	cp cmaes.pc $(INSTALL_DIRECTORY)/lib/pkgconfig

uninstall:
	rm -f $(INSTALL_DIRECTORY)/lib/$(TARGET)
	rm -f $(INSTALL_DIRECTORY)/lib/pkgconfig/cmaes.pc

# build examples
 #example_boundary.c  example_restarts.c  example_short.c
