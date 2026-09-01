# usage:
# 	insert this makefile into the end of target makefile
# 	and then make xxx all_s can show all variable infomation

all_var := $(foreach each,$(.VARIABLES),$(addsuffix $(if $(filter all_var .VARIABLES,$(each)),,={$($(each))})<$(origin $(each))>,$(each)))

targets = all_s

.PHONY : $(targets)

$(targets) :
	@echo '$(strip $($(@:_s=_var)))' | sed 's#> #>\n#g'
