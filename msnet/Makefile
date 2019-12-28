# 
APP-build := msnet

# 头文件路径
INCLUDES += -I. -I./3rd -I./3rd/asio-1.12.2  -I./3rd/x2struct-1.1 -I./3rd/faac 
INCLUDES += -I ./utils

INCLUDES +=  -I./3rd/librtmp 
LDFLAGS += ./3rd/librtmp/librtmp.a ./3rd/faac/libfaac/.libs/libfaac.a

# LDFLAGS += -L/usr/local/lib -lavutil -lavformat -lavcodec

# 编译选项
CXXFLAGS += $(INCLUDES) -DASIO_STANDALONE

# 生成可执行程序链接库
LDFLAGS += -ldl -lrt -pthread -Wl,-rpath=./

# 源文件
DIRS := utils srs g7xx src  

-include ../scripts/compile.mk