# This one here allows automatic directory creation
# All that is required is the secondary expansion of the prerequisities.
# And all the object files shall also be dependant, 'order only', on their
# containting directories + '/.' suffix.
.PRECIOUS: %/.
%/.:
	@$(ECHO_MD) $*
	$(NOECHO)mkdir -p $@

