# Makefile build
SRCS = $(foreach dir,$(DIRS),$(wildcard $(dir)/*.cpp))
OBJS := $(patsubst %.cpp, %.o, $(filter %.cpp, $(SRCS)))

PHONY := all 
all:: $(LIB-build) $(APP-build)
	$(Q)$(STRIP) $^
	$(ECHO) -e "\033[36mDone $^\033[0m"

$(LIB-build):: $(OBJS)
	$(Q)$(CXX) -shared -Wl,-soname,$@ $(LDFLAGS) -o $@ $^ $>
	 
$(APP-build):: $(OBJS)
	$(Q)$(CXX) -o $@ $^ $(LDFLAGS)

PHONY += clean
clean: 
	$(Q)rm -rf $(OBJS) $(APP-build) $(LIB-build)
	
PHONY += distclean
distclean: clean
	$(Q)rm -f $(shell find $(CURDIR) -name "*.d")

.PHONY: $(PHONY)

# 
%.d: %.cpp
	@$(CXX) $(CXXFLAGS) -MM $< -MT $(basename $@).o -o $(basename $@).d

%.o: %.cpp
	@echo "CXX " $@;
	@$(CXX) $(CXXFLAGS) -c $< -o $@

%.d: %.c
	@$(CC) $(CFLAGS) -MM $< -MT $(basename $@).o -o $(basename $@).d

%.o: %.c
	@echo "CC " $@;
	@$(CC) $(CFLAGS) -c $< -o $@

-include $(OBJS:.o=.d)
