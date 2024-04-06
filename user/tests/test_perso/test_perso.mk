$(eval $(call clear-module-vars))
LOCAL_MODULE_PATH := $(call my-dir)

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := test_perso
LOCAL_PROCESS_SRC := test_perso.c
$(eval $(call build-test-process))

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := simple
LOCAL_PROCESS_SRC := simple.c
$(eval $(call build-test-process))

$(eval $(call clear-process-vars))
LOCAL_PROCESS_NAME := test_shell
LOCAL_PROCESS_SRC := test_shell.c
$(eval $(call build-test-process))

$(eval $(call build-test-module))
