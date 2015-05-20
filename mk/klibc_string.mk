klibc_string_DIR         :=./klibc/string/
klibc_string_FILES       :=\
	$(notdir $(wildcard $(klibc_string_DIR)*.c))

klibc_string_INCLUDES:=\
 -I./klibc/include/

