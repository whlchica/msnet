include ../scripts/common.mk

LDFLAGS += 
INCLUDES += -I$(CURDIR)/include
CXXFLAGS += $(INCLUDES)
# 由于built-in_build.Makefile编译需要， 必须声明 CFLAGS CXXFLAGS LDFLAGS 
export CFLAGS CXXFLAGS LDFLAGS 

DIR := example/

include ../scripts/built-in.mk