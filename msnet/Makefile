# 
APP-build := msnet
CONFIG_HAVE_SRS_LIBRTMP=n
# 头文件路径
INCLUDES += -I. -I./3rd/asio-1.12.2  -I./3rd/x2struct-1.1 -I../libg726/include -I./3rd/faac 
INCLUDES += -I ./utils

ifeq ($(CONFIG_HAVE_SRS_LIBRTMP),y)
INCLUDES +=  -I./3rd/lib-srsrtmp/include
LDFLAGS += ./3rd/lib-srsrtmp/lib/srs_librtmp.a ./3rd/openssl/lib/libssl.a ./3rd/openssl/lib/libcrypto.a
CXXFLAGS += -DSRS_LIBRTMP
else 
INCLUDES +=  -I./3rd/librtmp 
LDFLAGS += ./3rd/librtmp/librtmp.a ./3rd/faac/libfaac/.libs/libfaac.a
endif
# 编译选项
CXXFLAGS += $(INCLUDES) -DASIO_STANDALONE

# 生成可执行程序链接库
LDFLAGS += -ldl -pthread -Wl,-rpath=./

# 源文件
DIRS := utils g7xx src  

-include ../scripts/compile.mk