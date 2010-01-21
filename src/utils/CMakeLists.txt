#INCPATH  = -I../ptex
#CXXFLAGS = -Wall -W -pedantic -std=c++98 $(DEBUG) $(INCPATH) -fPIC
#PTEX     = ../ptex/libPtex.a
#LIBS     = -lm -lz -lpthread

#INSTALL = \
#	bin/ptxinfo

add_executable(ptxinfo ptxinfo.cpp)
target_link_libraries(ptxinfo Ptex)


