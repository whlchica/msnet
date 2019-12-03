# 遍历指定目录执行make

# make dirs
src-y += common-include/
src-y += libg726/
src-y += libnet/
src-y += c++11/

all-dirs := $(src-y)
PHONY := all
all: $(all-dirs)
	@echo all done
	
PHONY+= $(all-dirs)
$(all-dirs):
	@cd $@; make || exit $? ;

# make clean
#
clean-dirs := $(addprefix _clean_, $(all-dirs))
PHONY += clean
clean: $(clean-dirs)
	@echo clean done
$(clean-dirs):
	@make -C $(patsubst _clean_%, %, $@) clean || exit $? ;

PHONY += distclean
distclean: clean
	@rm -f $(shell find ./ -name "*.d")
#	$(Q)@rm -r $(prefix)

.PHONY: $(PHONY)
