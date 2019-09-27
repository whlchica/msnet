# 需要在子层目录中写Makefile, 可以控制编译文件
OBJS := $(patsubst %/, %/built-in.o, $(DIR))

PHONY := all 
all:: $(OBJS) $(APP-build) $(LIB-build)
	$(ECHO) -e "\033[36mDone $^\033[0m"

$(LIB-build):: 
	$(Q)$(CXX) $(CXXFLAGS) -shared -Wl,-soname,$@ $(LDFLAGS) -o $@ $(OBJS) $>

$(APP-build):: 
	$(Q)$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $> $(LDFLAGS) 

PHONY += $(OBJS)
$(OBJS): 
	$(Q)@make -C $(DIR) -f $(TOPDIR)/scripts/built-in_build.Makefile

# make clean distclean
#
clean-dirs := $(addprefix _clean_, . $(DIR))
PHONY += clean distclean
clean: $(clean-dirs)
	$(Q)@rm -f $(APP-build) $(LIB-build)

PHONY += $(clean-dirs)
$(clean-dirs):
	$(Q)@make -C $(patsubst _clean_%,%,$@) -f $(TOPDIR)/scripts/built-in_clean.Makefile

distclean: clean
	$(Q)@rm -f $(shell find $(CURDIR) -name "*.d")

.PHONY: $(PHONY)
