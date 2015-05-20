ECHO_DEP        := echo "       DEP     "
ECHO_CC         := echo "       CC      "
ECHO_AS         := echo "       AS      "
ECHO_NASM       := echo "     NASM      "
ECHO_CXX        := echo "       CXX     "
ECHO_LD         := echo "       LD      "
ECHO_MD         := echo "       MD      "
ECHO_ISO        := echo "       ISO     "
ECHO_STRIP      := echo "       STRIP   "
ECHO_CP         := echo "       CP      "
ECHO_RM         := echo "       RM      "

ifeq ($(V),1)
NOECHO  :=
CP      :=cp -v
else
NOECHO  :=@
CP      :=cp
endif

