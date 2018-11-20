PROJECT(GUNDOOM)

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(GVERSION	"1.0")

#################################################################################
# 编译平台设定
#################################################################################
option(PLATFORM_HISIV	"build for platform hisiv chips" OFF)


IF(PLATFORM_HISIV)
	MESSAGE("-- build for platform hi3518ev200 chips")
    INCLUDE(${CMAKE_HOME_DIRECTORY}/config/toolchain-cross-hi3518e.cmake)
	SET(OUT_BIN_DIR	"${CMAKE_HOME_DIRECTORY}/output/arm/bin")
	SET(OUT_LIB_DIR	"${CMAKE_HOME_DIRECTORY}/output/arm/lib")
ELSE(PLATFORM_HISIV)
	MESSAGE("-- build for platform x86_64")
	SET(OUT_BIN_DIR	"${CMAKE_HOME_DIRECTORY}/output/x86_64/bin")
	SET(OUT_LIB_DIR	"${CMAKE_HOME_DIRECTORY}/output/x86_64/lib")
ENDIF(PLATFORM_HISIV)

SET(CMAKE_C_FLAGS "-g -pthread")
#################################################################################
# 目标文件设定
#################################################################################
SET(TARGET_FORMAT	"so")

SET(TARGET_NAME	"dtest")
SET(TARGET_TEST_FILE	"main/main.c")








#################################################################################
# 源码目录设定
#################################################################################
FILE(GLOB_RECURSE HEADER_LIST "*.h")
SET (INCLUDE_DIRS "")

FOREACH(_HEADERFILE ${HEADER_LIST})
    GET_FILENAME_COMPONENT(_DIR ${_HEADERFILE} PATH)
    LIST(APPEND INCLUDE_DIRS ${_DIR})
ENDFOREACH()

LIST(REMOVE_DUPLICATES INCLUDE_DIRS)
INCLUDE_DIRECTORIES(${INCLUDE_DIRS})

##逻辑层
FILE(GLOB_RECURSE FUNC_LIST		"function/*.c")
FILE(GLOB_RECURSE PRO_LIST		"profile/*.c")
FILE(GLOB_RECURSE PLAT_LIST		"platform/*.c")
FILE(GLOB_RECURSE STACK_LIST	"stack/*.c")
FILE(GLOB_RECURSE TRANS_LIST	"transport/hci_droid.c" 
								"transport/hw_link.c")
#################################################################################
##工具层
FILE(GLOB_RECURSE UTILS_LIST	"utils/*.c")	






							
#################################################################################						
MESSAGE("=================================================")
#################################################################################
#uran测试
MESSAGE("-- build add server test : [uran_test]")
SET(URAN_DIR "${CMAKE_HOME_DIRECTORY}/server/uran")
SET(URAN_TEST_FILE "${URAN_DIR}/test/uran_test.c")
SET(URAN_LIST
	"${URAN_DIR}/uran.c"
	"${URAN_DIR}/uran_time.c"
	"${URAN_DIR}/uran_tree.c"
	)
ADD_EXECUTABLE(uran_test ${URAN_TEST_FILE} ${URAN_LIST} ${UTILS_LIST})
SET_TARGET_PROPERTIES(uran_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${URAN_DIR}/test)
#################################################################################
IF(PLATFORM_HISIV)
MESSAGE("-- build add server test : [avio_test]")
SET(HI_AVIO_DIR "${CMAKE_HOME_DIRECTORY}/server/avio/hi3518e")
SET(HI_AVIO_TEST_FILE "${HI_AVIO_DIR}/test/avio_test.c")
SET(HI_AVIO_LIST
	"${HI_AVIO_DIR}/hi3518e.c"
	"${HI_AVIO_DIR}/hi3518e_audio.c"
	"${HI_AVIO_DIR}/hi3518e_video.c"
	"${HI_AVIO_DIR}/hi3518e_device.c"
	)
LINK_DIRECTORIES(${LIBS_DIR})
ADD_LIBRARY(hi3518e_avio SHARED ${HI_AVIO_LIST} ${URAN_LIST} ${UTILS_LIST})
TARGET_INCLUDE_DIRECTORIES(hi3518e_avio PUBLIC
							${COMMON_DIR}
							${MPP_PATH}/include
							${MPP_PATH}/component/acodec)

SET_TARGET_PROPERTIES(hi3518e_avio 
						PROPERTIES 
						COMPILE_FLAGS ${CHIP_FLAGS}
						LIBRARY_OUTPUT_DIRECTORY ${HI_AVIO_DIR})
ADD_EXECUTABLE(avio_test ${HI_AVIO_TEST_FILE})
TARGET_LINK_LIBRARIES(avio_test hi3518e_avio ${LIBS_FLAGS})
SET_TARGET_PROPERTIES(avio_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${HI_AVIO_DIR}/test)

ENDIF(PLATFORM_HISIV)
#################################################################################
#底层通讯接口测试
MESSAGE("-- build add server test : [abslay_test]")
SET(ABSL_DIR "${CMAKE_HOME_DIRECTORY}/transport/abslay")
SET(ABSL_TEST_FILE "${ABSL_DIR}/test/abslay_test.c")
SET(ABSL_LIST
	"${ABSL_DIR}/abslay.c"
	"${ABSL_DIR}/hash_fd.c"
	"${ABSL_DIR}/vendor/vendor_lib.c"
	"${ABSL_DIR}/vendor/userial_vendor.c"
	"${ABSL_DIR}/vendor/net_server_vendor.c"
	"${ABSL_DIR}/vendor/net_client_vendor.c"
	"${ABSL_DIR}/vendor/net_monitor_vendor.c"
	)
ADD_EXECUTABLE(abslay_test ${ABSL_TEST_FILE} ${ABSL_LIST} ${URAN_LIST} ${UTILS_LIST})
SET_TARGET_PROPERTIES(abslay_test PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ABSL_DIR}/test)
#################################################################################
MESSAGE("-- build platform output format: ${TARGET_FORMAT}")

SET (LOGIC_CODE		${FUNC_LIST} ${PRO_LIST} ${PLAT_LIST} ${STACK_LIST} ${TRANS_LIST})
SET (SERVER_CODE	${ABSL_LIST} ${URAN_LIST})

IF(TARGET_FORMAT STREQUAL "so")
	SET(TARGET_LIB_NAME	"newfish")
	MESSAGE("-- Compile mode :dynamic library, name :lib${TARGET_LIB_NAME}.so.${GVERSION}")
	ADD_LIBRARY(${TARGET_LIB_NAME} SHARED ${LOGIC_CODE} ${SERVER_CODE} ${UTILS_LIST})
	SET_TARGET_PROPERTIES(${TARGET_LIB_NAME} 
							PROPERTIES 
							COMPILE_FLAGS "-fPIC"
							LIBRARY_OUTPUT_DIRECTORY ${OUT_LIB_DIR}
							)
	ADD_EXECUTABLE(${TARGET_NAME} ${TARGET_TEST_FILE})
ELSEIF(${TARGET_FORMAT} STREQUAL "a")
	MESSAGE("-- Compile mode :static library, name :lib${TARGET_LIB_NAME}.a")
	SET(TARGET_LIB_NAME	"newfish")
	
	
ELSE(TARGET_FORMAT STREQUAL "exe")	
	MESSAGE("-- Compile mode :executable, name :${TARGET_NAME}")
	SET(TARGET_LIB_NAME	"")
	ADD_EXECUTABLE(${TARGET_NAME} ${TARGET_TEST_FILE} ${LOGIC_CODE} ${SERVER_CODE} ${UTILS_LIST})	
ENDIF(TARGET_FORMAT STREQUAL "so")

#使用动态库-lrt链接不到?
SET(EXT_LINK	${TARGET_LIB_NAME} librt.a)
TARGET_LINK_LIBRARIES(${TARGET_NAME} ${EXT_LINK})	
SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${OUT_BIN_DIR})

MESSAGE("=================================================")





