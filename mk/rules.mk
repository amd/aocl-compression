# Copyright (C) 2026, Advanced Micro Devices. All rights reserved.
# 
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its
# contributors may be used to endorse or promote products derived from this
# software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.

# ==============================================================================
# Build Rules and Patterns
# ==============================================================================

# ==============================================================================
# INCLUDE PATHS
# ==============================================================================

INCLUDES = -I$(ROOT_DIR) \
            -I$(GEN_DIR) \
            -I$(ROOT_DIR)/algos/common \
            -I$(ROOT_DIR)/utils \
            -I$(ROOT_DIR)/api

# ==============================================================================
# VPATH - Tell Make where to find source files
# ==============================================================================

vpath %.c $(ROOT_DIR)
vpath %.cpp $(ROOT_DIR)
vpath %.cc $(ROOT_DIR)
vpath %.S $(ROOT_DIR)

# ==============================================================================
# COMPILATION RULES
# ==============================================================================

# Verbose output control
ifeq ($(VERBOSE),1)
    Q :=
    ECHO := @true
else
    Q := @
    ECHO := @echo
endif

# Define compilation rule template for C files
define COMPILE_C_RULE
$(OBJ_DIR)/$(1).o: $(ROOT_DIR)/$(1).c | $(OBJ_DIR) $(DEP_DIR)
	@mkdir -p $$(dir $$@) $$(dir $(DEP_DIR)/$(1).d)
	$$(ECHO) "CC $$<"
	$$(Q)$$(CC) $$(CFLAGS) $$(INCLUDES) -MMD -MP -MF $(DEP_DIR)/$(1).d -c $$< -o $$@
endef

# Define compilation rule template for C++ files (.cpp)
define COMPILE_CPP_RULE
$(OBJ_DIR)/$(1).o: $(ROOT_DIR)/$(1).cpp | $(OBJ_DIR) $(DEP_DIR)
	@mkdir -p $$(dir $$@) $$(dir $(DEP_DIR)/$(1).d)
	$$(ECHO) "CXX $$<"
	$$(Q)$$(CXX) $$(CXXFLAGS) $$(INCLUDES) -MMD -MP -MF $(DEP_DIR)/$(1).d -c $$< -o $$@
endef

# Define compilation rule template for C++ files (.cc)
define COMPILE_CC_RULE
$(OBJ_DIR)/$(1).o: $(ROOT_DIR)/$(1).cc | $(OBJ_DIR) $(DEP_DIR)
	@mkdir -p $$(dir $$@) $$(dir $(DEP_DIR)/$(1).d)
	$$(ECHO) "CXX $$<"
	$$(Q)$$(CXX) $$(CXXFLAGS) $$(INCLUDES) -MMD -MP -MF $(DEP_DIR)/$(1).d -c $$< -o $$@
endef

# Define compilation rule template for assembly files
define COMPILE_ASM_RULE
$(OBJ_DIR)/$(1).o: $(ROOT_DIR)/$(1).S | $(OBJ_DIR) $(DEP_DIR)
	@mkdir -p $$(dir $$@) $$(dir $(DEP_DIR)/$(1).d)
	$$(ECHO) "AS $$<"
	$$(Q)$$(CC) $$(CFLAGS) $$(INCLUDES) -MMD -MP -MF $(DEP_DIR)/$(1).d -c $$< -o $$@
endef

# ==============================================================================
# DEPENDENCY GENERATION
# ==============================================================================

# Dependency files are generated automatically during compilation
# using -MMD -MP flags

# Pattern rule for dependency files
$(DEP_DIR)/%.d: ;

.PRECIOUS: $(DEP_DIR)/%.d

# ==============================================================================
# PHONY TARGETS
# ==============================================================================

.PHONY: all clean install uninstall help templates test

# ==============================================================================
# DEFAULT GOAL
# ==============================================================================

.DEFAULT_GOAL := all
