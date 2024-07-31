BOINC_DIR = /home/boincadm/boinc
BOINC_SOURCE_API_DIR = $(BOINC_DIR)/api
BOINC_SOURCE_LIB_DIR = $(BOINC_DIR)/lib
BOINC_SOURCE_ZIP_DIR = $(BOINC_DIR)/zip
BOINC_SOURCE_SCHED_DIR = $(BOINC_DIR)/sched
BOINC_SOURCE_TOOLS_DIR = $(BOINC_DIR)/tools
BOINC_SOURCE_DB_DIR = $(BOINC_DIR)/db

BOINC_API_DIR = $(BOINC_SOURCE_API_DIR)
BOINC_LIB_DIR = $(BOINC_SOURCE_LIB_DIR)
BOINC_ZIP_DIR = $(BOINC_SOURCE_ZIP_DIR)
BOINC_SCHED_DIR = $(BOINC_SOURCE_SCHED_DIR)
BOINC_TOOLS_DIR = $(BOINC_SOURCE_TOOLS_DIR)
BOINC_DB_DIR = $(BOINC_SOURCE_DB_DIR)

MAKEFILE_LDFLAGS = -lpthread libstdc++.a
MAKEFILE_STDLIB  = libstdc++.a

FREETYPE_DIR = /usr/include/freetype2
ARGPARSE_DIR = /usr/local/include/argparse
MYSQL_DIR = /usr/include/mysql

CXXFLAGS += -g \
	-Wall -W -Wshadow -Wpointer-arith -Wcast-qual -Wcast-align -Wwrite-strings -fno-common \
    -I$(BOINC_DIR) \
    -I$(BOINC_SOURCE_API_DIR) \
    -I$(BOINC_SOURCE_LIB_DIR) \
    -I$(BOINC_SOURCE_ZIP_DIR) \
    -I$(BOINC_SOURCE_SCHED_DIR) \
    -I$(BOINC_SOURCE_DB_DIR) \
    -I$(BOINC_SOURCE_TOOLS_DIR) \
    -I$(FREETYPE_DIR) \
    -I$(MYSQL_DIR) \
    -L$(BOINC_API_DIR) \
    -L$(BOINC_LIB_DIR) \
    -L$(BOINC_SCHED_DIR) \
    -L$(BOINC_TOOLS_DIR) \
    -L$(BOINC_DB_DIR) \
    -L$(ARGPARSE_DIR) \
    -L/usr/X11R6/lib \
    -L.

LIBGLUT = -lglut
LIBGLU = -lGLU
LIBGL= -lGL
LIBUI = -lX11 -lXmu
LIBFTGL = -lftgl
LIBJPEG = -ljpeg
LIBMYSQL = -lmysqlclient

VALIDATOR_OBJS = $(BOINC_DIR)/sched/validator.o \
    $(BOINC_DIR)/sched/validate_util.o \
    $(BOINC_DIR)/sched/validate_util2.o

ASSIMILATOR_OBJS = $(BOINC_DIR)/sched/assimilator.o \
    $(BOINC_DIR)/sched/validate_util.o

PROGS = work_generator generate_candidates_boinc generate_candidates_assimilator generate_candidates_validator calculate_frequency_boinc calculate_frequency_assimilator calculate_frequency_validator

$(info All PROGS=$(PROGS))

all: $(PROGS)

libstdc++.a:
	ln -s `$(CXX) $(CXXFLAGS) -print-file-name=libstdc++.a`

ttfont.cpp:
	ln -s ../../api/ttfont.cpp .

clean: distclean

distclean:
	/bin/rm -f $(PROGS) $(GUI_PROGS) $(addsuffix .exe, $(PROGS)) $(addsuffix .exe, $(GUI_PROGS)) *.o libstdc++.a ttfont.cpp

work_generator: work_generator.cpp $(MAKEFILE_STDLIB) $(BOINC_API_DIR)/libboinc_api.a $(BOINC_LIB_DIR)/libboinc.a
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o ../out/work_generator work_generator.cpp \
	-lboinc_api -lboinc $(MAKEFILE_LDFLAGS) \
	$(STDCPPTC)

# work_generator_test: work_generator_test.cpp $(MAKEFILE_STDLIB) $(BOINC_API_DIR)/libboinc_api.a $(BOINC_LIB_DIR)/libboinc.a
# 	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o ../out/work_generator_test work_generator_test.cpp \
# 	-lboinc_api -lboinc $(MAKEFILE_LDFLAGS) \
# 	$(STDCPPTC) -lz

generate_candidates_boinc: generate_candidates_boinc.cpp $(MAKEFILE_STDLIB) $(BOINC_API_DIR)/libboinc_api.a $(BOINC_LIB_DIR)/libboinc.a
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o ../out/generate_candidates generate_candidates_boinc.cpp \
	-lboinc_api -lboinc $(MAKEFILE_LDFLAGS) \
	$(STDCPPTC)

generate_candidates_assimilator: generate_candidates_assimilator.cpp
	$(CXX) $(CXXFLAGS) generate_candidates_assimilator.cpp $(ASSIMILATOR_OBJS) $(BOINC_DIR)/sched/libsched.a $(BOINC_DIR)/lib/libboinc.a \
    $(LIBMYSQL) -o ../out/generate_candidates_assimilator

generate_candidates_validator: generate_candidates_validator.cpp
	$(CXX) $(CXXFLAGS) generate_candidates_validator.cpp $(VALIDATOR_OBJS) $(BOINC_DIR)/sched/libsched.a $(BOINC_DIR)/lib/libboinc.a \
    $(LIBMYSQL) -o ../out/generate_candidates_validator

calculate_frequency_boinc: calculate_frequency_boinc.cpp $(MAKEFILE_STDLIB) $(BOINC_API_DIR)/libboinc_api.a $(BOINC_LIB_DIR)/libboinc.a
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) $(LDFLAGS) -o ../out/calculate_frequency calculate_frequency_boinc.cpp \
	-lboinc_api -lboinc $(MAKEFILE_LDFLAGS) \
	$(STDCPPTC)

calculate_frequency_assimilator: calculate_frequency_assimilator.cpp
	$(CXX) $(CXXFLAGS) calculate_frequency_assimilator.cpp $(ASSIMILATOR_OBJS) $(BOINC_DIR)/sched/libsched.a $(BOINC_DIR)/lib/libboinc.a \
    $(LIBMYSQL) -o ../out/calculate_frequency_assimilator

calculate_frequency_validator: calculate_frequency_validator.cpp
	$(CXX) $(CXXFLAGS) calculate_frequency_validator.cpp $(VALIDATOR_OBJS) $(BOINC_DIR)/sched/libsched.a $(BOINC_DIR)/lib/libboinc.a \
    $(LIBMYSQL) -o ../out/calculate_frequency_validator