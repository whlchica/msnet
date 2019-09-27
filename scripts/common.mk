ifeq ($(V),1)
Q :=
ECHO := @true
else
Q := @
ECHO := @echo
endif
TOPDIR := $(CURDIR)/..
export Q ECHO TOPDIR
#-------------------------------------------------------------------------------

# Compilation tools
CC = $(CROSS_COMPILE)gcc
CXX = $(CROSS_COMPILE)g++
AR = $(CROSS_COMPILE)ar
LD = $(CROSS_COMPILE)ld
STRIP = $(CROSS_COMPILE)strip
NM = $(CROSS_COMPILE)nm
# export CC CXX AR LD NM STRIP 

#-------------------------------------------------------------------------------
build ?= debug
CFLAGS_OPT ?= -Os 

ifeq ($(build), debug)
CFLAGS_OPT += -Wall -g
else
CFLAGS_OPT += -DNDEBUG 
endif

#-------------------------------------------------------------------------------
# 这里定义通用的编译参数，不同项目在对应Makefile中配置
# c 编译参数
CFLAGS := -O2 -fPIC -Wdeprecated-declarations $(CFLAGS_OPT)
# c++ 编译参数
CXXFLAGS := $(CFLAGS) -std=c++11
# 链接so, 在子层Makefile中配置
LDFLAGS := -pthread
# 链接头文件路径
INCLUDES :=
