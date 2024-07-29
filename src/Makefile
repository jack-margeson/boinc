BOINC_DIR = /home/boincadm/boinc/
BOINC_SOURCE_API_DIR = $(BOINC_DIR)/api
BOINC_SOURCE_LIB_DIR = $(BOINC_DIR)/lib
BOINC_SOURCE_ZIP_DIR = $(BOINC_DIR)/zip
FREETYPE_DIR = /usr/include/freetype2

BOINC_API_DIR = $(BOINC_SOURCE_API_DIR)
BOINC_LIB_DIR = $(BOINC_SOURCE_LIB_DIR)
BOINC_ZIP_DIR = $(BOINC_SOURCE_ZIP_DIR)

MAKEFILE_LDFLAGS = -lpthread libstdc++.a
MAKEFILE_STDLIB  = libstdc++.a

ARGPARSE_DIR = /usr/local/include/argparse

CXXFLAGS += -g \
	-Wall -W -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -fno-common \
    -DAPP_GRAPHICS \
    -I$(BOINC_DIR) \
    -I$(BOINC_SOURCE_API_DIR) \
    -I$(BOINC_SOURCE_LIB_DIR) \
    -I$(BOINC_SOURCE_ZIP_DIR) \
    -I$(FREETYPE_DIR) \
    -L$(BOINC_API_DIR) \
    -L$(BOINC_LIB_DIR) \
    -L$(ARGPARSE_DIR) \
    -L/usr/X11R6/lib \
    -L.

LIBGLUT = -lglut
LIBGLU = -lGLU
LIBGL= -lGL
LIBUI = -lX11 -lXmu
LIBFTGL = -lftgl
LIBJPEG = -ljpeg


CFLAGS = -g -Wall

LIBS = $(BOINC_DIR)/sched/libsched.a $(BOINC_DIR)/lib/libboinc.a \
    -lmysqlclient

INC = -I $(BOINC_DIR) -I $(BOINC_DIR)/lib -I $(BOINC_DIR)/sched -I $(BOINC_DIR)/db \
      -I /usr/include/mysql

CXX = g++ $(CFLAGS) $(INC)

VALIDATOR_OBJS = $(BOINC_DIR)/sched/validator.o \
    $(BOINC_DIR)/sched/validate_util.o \
    $(BOINC_DIR)/sched/validate_util2.o

ASSIMILATOR_OBJS = $(BOINC_DIR)/sched/assimilator.o \
    $(BOINC_DIR)/sched/validate_util.o

PROGS = work_generator generate_candidates_boinc generate_candidates_assimilator

$(info All PROGS=$(PROGS))

all: $(PROGS)

libstdc++.a:
	ln -s `$(CXX) $(CXXFLAGS) -print-file-name=libstdc++.a`

ttfont.cpp:
	ln -s ../../api/ttfont.cpp .

clean: distclean

distclean:
	/bin/rm -f $(PROGS) $(GUI_PROGS) $(addsuffix .exe, $(PROGS)) $(addsuffix .exe, $(GUI_PROGS)) *.o libstdc++.a ttfont.cpp

install: generate_candidates_boinc 

work_generator: work_generator.cpp $(MAKEFILE_STDLIB) $(BOINC_API_DIR)/libboinc_api.a $(BOINC_LIB_DIR)/libboinc.a
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o ../out/work_generator work_generator.cpp \
	-lboinc_api -lboinc $(MAKEFILE_LDFLAGS) \
	$(STDCPPTC)

generate_candidates_boinc: generate_candidates_boinc.cpp $(MAKEFILE_STDLIB) $(BOINC_API_DIR)/libboinc_api.a $(BOINC_LIB_DIR)/libboinc.a
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o ../out/generate_candidates generate_candidates_boinc.cpp \
	-lboinc_api -lboinc $(MAKEFILE_LDFLAGS) \
	$(STDCPPTC)

generate_candidates_assimilator: generate_candidates_assimilator.cpp
	$(CXX) generate_candidates_assimilator.cpp $(ASSIMILATOR_OBJS) $(LIBS) -o ../out/generate_candidates_assimilator