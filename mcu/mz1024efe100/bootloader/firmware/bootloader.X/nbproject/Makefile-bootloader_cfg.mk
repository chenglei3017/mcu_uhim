#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-bootloader_cfg.mk)" "nbproject/Makefile-local-bootloader_cfg.mk"
include nbproject/Makefile-local-bootloader_cfg.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=bootloader_cfg
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=../src/system_config/bootloader_cfg/framework/driver/pmp/src/drv_pmp_static.c ../src/system_config/bootloader_cfg/framework/driver/usart/src/drv_usart_mapping.c ../src/system_config/bootloader_cfg/framework/driver/usart/src/drv_usart_static.c ../src/system_config/bootloader_cfg/framework/driver/usart/src/drv_usart_static_byte_model.c ../src/system_config/bootloader_cfg/framework/system/clk/src/sys_clk_static.c ../src/system_config/bootloader_cfg/framework/system/ports/src/sys_ports_static.c ../src/system_config/bootloader_cfg/gfx_resources_int.S ../src/system_config/bootloader_cfg/gfx_resources_int_reference.c ../src/system_config/bootloader_cfg/system_init.c ../src/system_config/bootloader_cfg/system_interrupt.c ../src/system_config/bootloader_cfg/system_exceptions.c ../src/system_config/bootloader_cfg/system_tasks.c ../src/system_config/bootloader_cfg/gfx_hgc_definitions.c ../src/app.c ../src/main.c ../../../../microchip/harmony/v1_08/framework/bootloader/src/datastream/datastream.c ../../../../microchip/harmony/v1_08/framework/bootloader/src/datastream/datastream_usart.c ../../../../microchip/harmony/v1_08/framework/bootloader/src/bootloader.c ../../../../microchip/harmony/v1_08/framework/bootloader/src/nvm.c ../../../../microchip/harmony/v1_08/framework/decoder/jpeg/JpegDecoder.c ../../../../microchip/harmony/v1_08/framework/driver/gfx/controller/ili9488/src/drv_gfx_ili9488.c ../../../../microchip/harmony/v1_08/framework/gfx/src/gfx.c ../../../../microchip/harmony/v1_08/framework/gfx/src/gfx_primitive.c ../../../../microchip/harmony/v1_08/framework/gfx/src/jpeg_image.c ../../../../microchip/harmony/v1_08/framework/gfx/src/gfx_image_decoder.c ../../../../microchip/harmony/v1_08/framework/system/devcon/src/sys_devcon.c ../../../../microchip/harmony/v1_08/framework/system/devcon/src/sys_devcon_pic32mz.c ../../../../microchip/harmony/v1_08/framework/system/devcon/src/sys_devcon_cache_pic32mz.S ../../../../microchip/harmony/v1_08/third_party/decoder/jidctint/src/jidctint.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/_ext/515941952/drv_pmp_static.o ${OBJECTDIR}/_ext/1010059186/drv_usart_mapping.o ${OBJECTDIR}/_ext/1010059186/drv_usart_static.o ${OBJECTDIR}/_ext/1010059186/drv_usart_static_byte_model.o ${OBJECTDIR}/_ext/787999830/sys_clk_static.o ${OBJECTDIR}/_ext/138989690/sys_ports_static.o ${OBJECTDIR}/_ext/310189631/gfx_resources_int.o ${OBJECTDIR}/_ext/310189631/gfx_resources_int_reference.o ${OBJECTDIR}/_ext/310189631/system_init.o ${OBJECTDIR}/_ext/310189631/system_interrupt.o ${OBJECTDIR}/_ext/310189631/system_exceptions.o ${OBJECTDIR}/_ext/310189631/system_tasks.o ${OBJECTDIR}/_ext/310189631/gfx_hgc_definitions.o ${OBJECTDIR}/_ext/1360937237/app.o ${OBJECTDIR}/_ext/1360937237/main.o ${OBJECTDIR}/_ext/1152266019/datastream.o ${OBJECTDIR}/_ext/1152266019/datastream_usart.o ${OBJECTDIR}/_ext/2110890954/bootloader.o ${OBJECTDIR}/_ext/2110890954/nvm.o ${OBJECTDIR}/_ext/490539985/JpegDecoder.o ${OBJECTDIR}/_ext/314127580/drv_gfx_ili9488.o ${OBJECTDIR}/_ext/2145404914/gfx.o ${OBJECTDIR}/_ext/2145404914/gfx_primitive.o ${OBJECTDIR}/_ext/2145404914/jpeg_image.o ${OBJECTDIR}/_ext/2145404914/gfx_image_decoder.o ${OBJECTDIR}/_ext/1863813466/sys_devcon.o ${OBJECTDIR}/_ext/1863813466/sys_devcon_pic32mz.o ${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o ${OBJECTDIR}/_ext/980971323/jidctint.o
POSSIBLE_DEPFILES=${OBJECTDIR}/_ext/515941952/drv_pmp_static.o.d ${OBJECTDIR}/_ext/1010059186/drv_usart_mapping.o.d ${OBJECTDIR}/_ext/1010059186/drv_usart_static.o.d ${OBJECTDIR}/_ext/1010059186/drv_usart_static_byte_model.o.d ${OBJECTDIR}/_ext/787999830/sys_clk_static.o.d ${OBJECTDIR}/_ext/138989690/sys_ports_static.o.d ${OBJECTDIR}/_ext/310189631/gfx_resources_int.o.d ${OBJECTDIR}/_ext/310189631/gfx_resources_int_reference.o.d ${OBJECTDIR}/_ext/310189631/system_init.o.d ${OBJECTDIR}/_ext/310189631/system_interrupt.o.d ${OBJECTDIR}/_ext/310189631/system_exceptions.o.d ${OBJECTDIR}/_ext/310189631/system_tasks.o.d ${OBJECTDIR}/_ext/310189631/gfx_hgc_definitions.o.d ${OBJECTDIR}/_ext/1360937237/app.o.d ${OBJECTDIR}/_ext/1360937237/main.o.d ${OBJECTDIR}/_ext/1152266019/datastream.o.d ${OBJECTDIR}/_ext/1152266019/datastream_usart.o.d ${OBJECTDIR}/_ext/2110890954/bootloader.o.d ${OBJECTDIR}/_ext/2110890954/nvm.o.d ${OBJECTDIR}/_ext/490539985/JpegDecoder.o.d ${OBJECTDIR}/_ext/314127580/drv_gfx_ili9488.o.d ${OBJECTDIR}/_ext/2145404914/gfx.o.d ${OBJECTDIR}/_ext/2145404914/gfx_primitive.o.d ${OBJECTDIR}/_ext/2145404914/jpeg_image.o.d ${OBJECTDIR}/_ext/2145404914/gfx_image_decoder.o.d ${OBJECTDIR}/_ext/1863813466/sys_devcon.o.d ${OBJECTDIR}/_ext/1863813466/sys_devcon_pic32mz.o.d ${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o.d ${OBJECTDIR}/_ext/980971323/jidctint.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/_ext/515941952/drv_pmp_static.o ${OBJECTDIR}/_ext/1010059186/drv_usart_mapping.o ${OBJECTDIR}/_ext/1010059186/drv_usart_static.o ${OBJECTDIR}/_ext/1010059186/drv_usart_static_byte_model.o ${OBJECTDIR}/_ext/787999830/sys_clk_static.o ${OBJECTDIR}/_ext/138989690/sys_ports_static.o ${OBJECTDIR}/_ext/310189631/gfx_resources_int.o ${OBJECTDIR}/_ext/310189631/gfx_resources_int_reference.o ${OBJECTDIR}/_ext/310189631/system_init.o ${OBJECTDIR}/_ext/310189631/system_interrupt.o ${OBJECTDIR}/_ext/310189631/system_exceptions.o ${OBJECTDIR}/_ext/310189631/system_tasks.o ${OBJECTDIR}/_ext/310189631/gfx_hgc_definitions.o ${OBJECTDIR}/_ext/1360937237/app.o ${OBJECTDIR}/_ext/1360937237/main.o ${OBJECTDIR}/_ext/1152266019/datastream.o ${OBJECTDIR}/_ext/1152266019/datastream_usart.o ${OBJECTDIR}/_ext/2110890954/bootloader.o ${OBJECTDIR}/_ext/2110890954/nvm.o ${OBJECTDIR}/_ext/490539985/JpegDecoder.o ${OBJECTDIR}/_ext/314127580/drv_gfx_ili9488.o ${OBJECTDIR}/_ext/2145404914/gfx.o ${OBJECTDIR}/_ext/2145404914/gfx_primitive.o ${OBJECTDIR}/_ext/2145404914/jpeg_image.o ${OBJECTDIR}/_ext/2145404914/gfx_image_decoder.o ${OBJECTDIR}/_ext/1863813466/sys_devcon.o ${OBJECTDIR}/_ext/1863813466/sys_devcon_pic32mz.o ${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o ${OBJECTDIR}/_ext/980971323/jidctint.o

# Source Files
SOURCEFILES=../src/system_config/bootloader_cfg/framework/driver/pmp/src/drv_pmp_static.c ../src/system_config/bootloader_cfg/framework/driver/usart/src/drv_usart_mapping.c ../src/system_config/bootloader_cfg/framework/driver/usart/src/drv_usart_static.c ../src/system_config/bootloader_cfg/framework/driver/usart/src/drv_usart_static_byte_model.c ../src/system_config/bootloader_cfg/framework/system/clk/src/sys_clk_static.c ../src/system_config/bootloader_cfg/framework/system/ports/src/sys_ports_static.c ../src/system_config/bootloader_cfg/gfx_resources_int.S ../src/system_config/bootloader_cfg/gfx_resources_int_reference.c ../src/system_config/bootloader_cfg/system_init.c ../src/system_config/bootloader_cfg/system_interrupt.c ../src/system_config/bootloader_cfg/system_exceptions.c ../src/system_config/bootloader_cfg/system_tasks.c ../src/system_config/bootloader_cfg/gfx_hgc_definitions.c ../src/app.c ../src/main.c ../../../../microchip/harmony/v1_08/framework/bootloader/src/datastream/datastream.c ../../../../microchip/harmony/v1_08/framework/bootloader/src/datastream/datastream_usart.c ../../../../microchip/harmony/v1_08/framework/bootloader/src/bootloader.c ../../../../microchip/harmony/v1_08/framework/bootloader/src/nvm.c ../../../../microchip/harmony/v1_08/framework/decoder/jpeg/JpegDecoder.c ../../../../microchip/harmony/v1_08/framework/driver/gfx/controller/ili9488/src/drv_gfx_ili9488.c ../../../../microchip/harmony/v1_08/framework/gfx/src/gfx.c ../../../../microchip/harmony/v1_08/framework/gfx/src/gfx_primitive.c ../../../../microchip/harmony/v1_08/framework/gfx/src/jpeg_image.c ../../../../microchip/harmony/v1_08/framework/gfx/src/gfx_image_decoder.c ../../../../microchip/harmony/v1_08/framework/system/devcon/src/sys_devcon.c ../../../../microchip/harmony/v1_08/framework/system/devcon/src/sys_devcon_pic32mz.c ../../../../microchip/harmony/v1_08/framework/system/devcon/src/sys_devcon_cache_pic32mz.S ../../../../microchip/harmony/v1_08/third_party/decoder/jidctint/src/jidctint.c


CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-bootloader_cfg.mk dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=32MZ1024EFE100
MP_LINKER_FILE_OPTION=,--script="..\src\system_config\bootloader_cfg\btl_mz.ld"
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/310189631/gfx_resources_int.o: ../src/system_config/bootloader_cfg/gfx_resources_int.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/310189631" 
	@${RM} ${OBJECTDIR}/_ext/310189631/gfx_resources_int.o.d 
	@${RM} ${OBJECTDIR}/_ext/310189631/gfx_resources_int.o 
	@${RM} ${OBJECTDIR}/_ext/310189631/gfx_resources_int.o.ok ${OBJECTDIR}/_ext/310189631/gfx_resources_int.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/310189631/gfx_resources_int.o.d" "${OBJECTDIR}/_ext/310189631/gfx_resources_int.o.asm.d" -t $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC} $(MP_EXTRA_AS_PRE)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/310189631/gfx_resources_int.o.d"  -o ${OBJECTDIR}/_ext/310189631/gfx_resources_int.o ../src/system_config/bootloader_cfg/gfx_resources_int.S  -DXPRJ_bootloader_cfg=$(CND_CONF)    -Wa,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_AS_POST),-MD="${OBJECTDIR}/_ext/310189631/gfx_resources_int.o.asm.d",--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--gdwarf-2,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1
	
${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o: ../../../../microchip/harmony/v1_08/framework/system/devcon/src/sys_devcon_cache_pic32mz.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1863813466" 
	@${RM} ${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o.d 
	@${RM} ${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o 
	@${RM} ${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o.ok ${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o.d" "${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o.asm.d" -t $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC} $(MP_EXTRA_AS_PRE)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o.d"  -o ${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o ../../../../microchip/harmony/v1_08/framework/system/devcon/src/sys_devcon_cache_pic32mz.S  -DXPRJ_bootloader_cfg=$(CND_CONF)    -Wa,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_AS_POST),-MD="${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o.asm.d",--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--gdwarf-2,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1
	
else
${OBJECTDIR}/_ext/310189631/gfx_resources_int.o: ../src/system_config/bootloader_cfg/gfx_resources_int.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/310189631" 
	@${RM} ${OBJECTDIR}/_ext/310189631/gfx_resources_int.o.d 
	@${RM} ${OBJECTDIR}/_ext/310189631/gfx_resources_int.o 
	@${RM} ${OBJECTDIR}/_ext/310189631/gfx_resources_int.o.ok ${OBJECTDIR}/_ext/310189631/gfx_resources_int.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/310189631/gfx_resources_int.o.d" "${OBJECTDIR}/_ext/310189631/gfx_resources_int.o.asm.d" -t $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC} $(MP_EXTRA_AS_PRE)  -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/310189631/gfx_resources_int.o.d"  -o ${OBJECTDIR}/_ext/310189631/gfx_resources_int.o ../src/system_config/bootloader_cfg/gfx_resources_int.S  -DXPRJ_bootloader_cfg=$(CND_CONF)    -Wa,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_AS_POST),-MD="${OBJECTDIR}/_ext/310189631/gfx_resources_int.o.asm.d",--gdwarf-2
	
${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o: ../../../../microchip/harmony/v1_08/framework/system/devcon/src/sys_devcon_cache_pic32mz.S  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1863813466" 
	@${RM} ${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o.d 
	@${RM} ${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o 
	@${RM} ${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o.ok ${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o.err 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o.d" "${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o.asm.d" -t $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC} $(MP_EXTRA_AS_PRE)  -c -mprocessor=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o.d"  -o ${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o ../../../../microchip/harmony/v1_08/framework/system/devcon/src/sys_devcon_cache_pic32mz.S  -DXPRJ_bootloader_cfg=$(CND_CONF)    -Wa,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_AS_POST),-MD="${OBJECTDIR}/_ext/1863813466/sys_devcon_cache_pic32mz.o.asm.d",--gdwarf-2
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/_ext/515941952/drv_pmp_static.o: ../src/system_config/bootloader_cfg/framework/driver/pmp/src/drv_pmp_static.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/515941952" 
	@${RM} ${OBJECTDIR}/_ext/515941952/drv_pmp_static.o.d 
	@${RM} ${OBJECTDIR}/_ext/515941952/drv_pmp_static.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/515941952/drv_pmp_static.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/515941952/drv_pmp_static.o.d" -o ${OBJECTDIR}/_ext/515941952/drv_pmp_static.o ../src/system_config/bootloader_cfg/framework/driver/pmp/src/drv_pmp_static.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1010059186/drv_usart_mapping.o: ../src/system_config/bootloader_cfg/framework/driver/usart/src/drv_usart_mapping.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1010059186" 
	@${RM} ${OBJECTDIR}/_ext/1010059186/drv_usart_mapping.o.d 
	@${RM} ${OBJECTDIR}/_ext/1010059186/drv_usart_mapping.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1010059186/drv_usart_mapping.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/1010059186/drv_usart_mapping.o.d" -o ${OBJECTDIR}/_ext/1010059186/drv_usart_mapping.o ../src/system_config/bootloader_cfg/framework/driver/usart/src/drv_usart_mapping.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1010059186/drv_usart_static.o: ../src/system_config/bootloader_cfg/framework/driver/usart/src/drv_usart_static.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1010059186" 
	@${RM} ${OBJECTDIR}/_ext/1010059186/drv_usart_static.o.d 
	@${RM} ${OBJECTDIR}/_ext/1010059186/drv_usart_static.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1010059186/drv_usart_static.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/1010059186/drv_usart_static.o.d" -o ${OBJECTDIR}/_ext/1010059186/drv_usart_static.o ../src/system_config/bootloader_cfg/framework/driver/usart/src/drv_usart_static.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1010059186/drv_usart_static_byte_model.o: ../src/system_config/bootloader_cfg/framework/driver/usart/src/drv_usart_static_byte_model.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1010059186" 
	@${RM} ${OBJECTDIR}/_ext/1010059186/drv_usart_static_byte_model.o.d 
	@${RM} ${OBJECTDIR}/_ext/1010059186/drv_usart_static_byte_model.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1010059186/drv_usart_static_byte_model.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/1010059186/drv_usart_static_byte_model.o.d" -o ${OBJECTDIR}/_ext/1010059186/drv_usart_static_byte_model.o ../src/system_config/bootloader_cfg/framework/driver/usart/src/drv_usart_static_byte_model.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/787999830/sys_clk_static.o: ../src/system_config/bootloader_cfg/framework/system/clk/src/sys_clk_static.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/787999830" 
	@${RM} ${OBJECTDIR}/_ext/787999830/sys_clk_static.o.d 
	@${RM} ${OBJECTDIR}/_ext/787999830/sys_clk_static.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/787999830/sys_clk_static.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/787999830/sys_clk_static.o.d" -o ${OBJECTDIR}/_ext/787999830/sys_clk_static.o ../src/system_config/bootloader_cfg/framework/system/clk/src/sys_clk_static.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/138989690/sys_ports_static.o: ../src/system_config/bootloader_cfg/framework/system/ports/src/sys_ports_static.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/138989690" 
	@${RM} ${OBJECTDIR}/_ext/138989690/sys_ports_static.o.d 
	@${RM} ${OBJECTDIR}/_ext/138989690/sys_ports_static.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/138989690/sys_ports_static.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/138989690/sys_ports_static.o.d" -o ${OBJECTDIR}/_ext/138989690/sys_ports_static.o ../src/system_config/bootloader_cfg/framework/system/ports/src/sys_ports_static.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/310189631/gfx_resources_int_reference.o: ../src/system_config/bootloader_cfg/gfx_resources_int_reference.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/310189631" 
	@${RM} ${OBJECTDIR}/_ext/310189631/gfx_resources_int_reference.o.d 
	@${RM} ${OBJECTDIR}/_ext/310189631/gfx_resources_int_reference.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/310189631/gfx_resources_int_reference.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/310189631/gfx_resources_int_reference.o.d" -o ${OBJECTDIR}/_ext/310189631/gfx_resources_int_reference.o ../src/system_config/bootloader_cfg/gfx_resources_int_reference.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/310189631/system_init.o: ../src/system_config/bootloader_cfg/system_init.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/310189631" 
	@${RM} ${OBJECTDIR}/_ext/310189631/system_init.o.d 
	@${RM} ${OBJECTDIR}/_ext/310189631/system_init.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/310189631/system_init.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/310189631/system_init.o.d" -o ${OBJECTDIR}/_ext/310189631/system_init.o ../src/system_config/bootloader_cfg/system_init.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/310189631/system_interrupt.o: ../src/system_config/bootloader_cfg/system_interrupt.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/310189631" 
	@${RM} ${OBJECTDIR}/_ext/310189631/system_interrupt.o.d 
	@${RM} ${OBJECTDIR}/_ext/310189631/system_interrupt.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/310189631/system_interrupt.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/310189631/system_interrupt.o.d" -o ${OBJECTDIR}/_ext/310189631/system_interrupt.o ../src/system_config/bootloader_cfg/system_interrupt.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/310189631/system_exceptions.o: ../src/system_config/bootloader_cfg/system_exceptions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/310189631" 
	@${RM} ${OBJECTDIR}/_ext/310189631/system_exceptions.o.d 
	@${RM} ${OBJECTDIR}/_ext/310189631/system_exceptions.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/310189631/system_exceptions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/310189631/system_exceptions.o.d" -o ${OBJECTDIR}/_ext/310189631/system_exceptions.o ../src/system_config/bootloader_cfg/system_exceptions.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/310189631/system_tasks.o: ../src/system_config/bootloader_cfg/system_tasks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/310189631" 
	@${RM} ${OBJECTDIR}/_ext/310189631/system_tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/310189631/system_tasks.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/310189631/system_tasks.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/310189631/system_tasks.o.d" -o ${OBJECTDIR}/_ext/310189631/system_tasks.o ../src/system_config/bootloader_cfg/system_tasks.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/310189631/gfx_hgc_definitions.o: ../src/system_config/bootloader_cfg/gfx_hgc_definitions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/310189631" 
	@${RM} ${OBJECTDIR}/_ext/310189631/gfx_hgc_definitions.o.d 
	@${RM} ${OBJECTDIR}/_ext/310189631/gfx_hgc_definitions.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/310189631/gfx_hgc_definitions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/310189631/gfx_hgc_definitions.o.d" -o ${OBJECTDIR}/_ext/310189631/gfx_hgc_definitions.o ../src/system_config/bootloader_cfg/gfx_hgc_definitions.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/app.o: ../src/app.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/app.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/1360937237/app.o.d" -o ${OBJECTDIR}/_ext/1360937237/app.o ../src/app.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1152266019/datastream.o: ../../../../microchip/harmony/v1_08/framework/bootloader/src/datastream/datastream.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1152266019" 
	@${RM} ${OBJECTDIR}/_ext/1152266019/datastream.o.d 
	@${RM} ${OBJECTDIR}/_ext/1152266019/datastream.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1152266019/datastream.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/1152266019/datastream.o.d" -o ${OBJECTDIR}/_ext/1152266019/datastream.o ../../../../microchip/harmony/v1_08/framework/bootloader/src/datastream/datastream.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1152266019/datastream_usart.o: ../../../../microchip/harmony/v1_08/framework/bootloader/src/datastream/datastream_usart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1152266019" 
	@${RM} ${OBJECTDIR}/_ext/1152266019/datastream_usart.o.d 
	@${RM} ${OBJECTDIR}/_ext/1152266019/datastream_usart.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1152266019/datastream_usart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/1152266019/datastream_usart.o.d" -o ${OBJECTDIR}/_ext/1152266019/datastream_usart.o ../../../../microchip/harmony/v1_08/framework/bootloader/src/datastream/datastream_usart.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2110890954/bootloader.o: ../../../../microchip/harmony/v1_08/framework/bootloader/src/bootloader.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2110890954" 
	@${RM} ${OBJECTDIR}/_ext/2110890954/bootloader.o.d 
	@${RM} ${OBJECTDIR}/_ext/2110890954/bootloader.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2110890954/bootloader.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/2110890954/bootloader.o.d" -o ${OBJECTDIR}/_ext/2110890954/bootloader.o ../../../../microchip/harmony/v1_08/framework/bootloader/src/bootloader.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2110890954/nvm.o: ../../../../microchip/harmony/v1_08/framework/bootloader/src/nvm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2110890954" 
	@${RM} ${OBJECTDIR}/_ext/2110890954/nvm.o.d 
	@${RM} ${OBJECTDIR}/_ext/2110890954/nvm.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2110890954/nvm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/2110890954/nvm.o.d" -o ${OBJECTDIR}/_ext/2110890954/nvm.o ../../../../microchip/harmony/v1_08/framework/bootloader/src/nvm.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/490539985/JpegDecoder.o: ../../../../microchip/harmony/v1_08/framework/decoder/jpeg/JpegDecoder.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/490539985" 
	@${RM} ${OBJECTDIR}/_ext/490539985/JpegDecoder.o.d 
	@${RM} ${OBJECTDIR}/_ext/490539985/JpegDecoder.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/490539985/JpegDecoder.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/490539985/JpegDecoder.o.d" -o ${OBJECTDIR}/_ext/490539985/JpegDecoder.o ../../../../microchip/harmony/v1_08/framework/decoder/jpeg/JpegDecoder.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/314127580/drv_gfx_ili9488.o: ../../../../microchip/harmony/v1_08/framework/driver/gfx/controller/ili9488/src/drv_gfx_ili9488.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/314127580" 
	@${RM} ${OBJECTDIR}/_ext/314127580/drv_gfx_ili9488.o.d 
	@${RM} ${OBJECTDIR}/_ext/314127580/drv_gfx_ili9488.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/314127580/drv_gfx_ili9488.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/314127580/drv_gfx_ili9488.o.d" -o ${OBJECTDIR}/_ext/314127580/drv_gfx_ili9488.o ../../../../microchip/harmony/v1_08/framework/driver/gfx/controller/ili9488/src/drv_gfx_ili9488.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2145404914/gfx.o: ../../../../microchip/harmony/v1_08/framework/gfx/src/gfx.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2145404914" 
	@${RM} ${OBJECTDIR}/_ext/2145404914/gfx.o.d 
	@${RM} ${OBJECTDIR}/_ext/2145404914/gfx.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2145404914/gfx.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/2145404914/gfx.o.d" -o ${OBJECTDIR}/_ext/2145404914/gfx.o ../../../../microchip/harmony/v1_08/framework/gfx/src/gfx.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2145404914/gfx_primitive.o: ../../../../microchip/harmony/v1_08/framework/gfx/src/gfx_primitive.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2145404914" 
	@${RM} ${OBJECTDIR}/_ext/2145404914/gfx_primitive.o.d 
	@${RM} ${OBJECTDIR}/_ext/2145404914/gfx_primitive.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2145404914/gfx_primitive.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/2145404914/gfx_primitive.o.d" -o ${OBJECTDIR}/_ext/2145404914/gfx_primitive.o ../../../../microchip/harmony/v1_08/framework/gfx/src/gfx_primitive.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2145404914/jpeg_image.o: ../../../../microchip/harmony/v1_08/framework/gfx/src/jpeg_image.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2145404914" 
	@${RM} ${OBJECTDIR}/_ext/2145404914/jpeg_image.o.d 
	@${RM} ${OBJECTDIR}/_ext/2145404914/jpeg_image.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2145404914/jpeg_image.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/2145404914/jpeg_image.o.d" -o ${OBJECTDIR}/_ext/2145404914/jpeg_image.o ../../../../microchip/harmony/v1_08/framework/gfx/src/jpeg_image.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2145404914/gfx_image_decoder.o: ../../../../microchip/harmony/v1_08/framework/gfx/src/gfx_image_decoder.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2145404914" 
	@${RM} ${OBJECTDIR}/_ext/2145404914/gfx_image_decoder.o.d 
	@${RM} ${OBJECTDIR}/_ext/2145404914/gfx_image_decoder.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2145404914/gfx_image_decoder.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/2145404914/gfx_image_decoder.o.d" -o ${OBJECTDIR}/_ext/2145404914/gfx_image_decoder.o ../../../../microchip/harmony/v1_08/framework/gfx/src/gfx_image_decoder.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1863813466/sys_devcon.o: ../../../../microchip/harmony/v1_08/framework/system/devcon/src/sys_devcon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1863813466" 
	@${RM} ${OBJECTDIR}/_ext/1863813466/sys_devcon.o.d 
	@${RM} ${OBJECTDIR}/_ext/1863813466/sys_devcon.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1863813466/sys_devcon.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/1863813466/sys_devcon.o.d" -o ${OBJECTDIR}/_ext/1863813466/sys_devcon.o ../../../../microchip/harmony/v1_08/framework/system/devcon/src/sys_devcon.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1863813466/sys_devcon_pic32mz.o: ../../../../microchip/harmony/v1_08/framework/system/devcon/src/sys_devcon_pic32mz.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1863813466" 
	@${RM} ${OBJECTDIR}/_ext/1863813466/sys_devcon_pic32mz.o.d 
	@${RM} ${OBJECTDIR}/_ext/1863813466/sys_devcon_pic32mz.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1863813466/sys_devcon_pic32mz.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/1863813466/sys_devcon_pic32mz.o.d" -o ${OBJECTDIR}/_ext/1863813466/sys_devcon_pic32mz.o ../../../../microchip/harmony/v1_08/framework/system/devcon/src/sys_devcon_pic32mz.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/980971323/jidctint.o: ../../../../microchip/harmony/v1_08/third_party/decoder/jidctint/src/jidctint.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/980971323" 
	@${RM} ${OBJECTDIR}/_ext/980971323/jidctint.o.d 
	@${RM} ${OBJECTDIR}/_ext/980971323/jidctint.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/980971323/jidctint.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -fframe-base-loclist  -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/980971323/jidctint.o.d" -o ${OBJECTDIR}/_ext/980971323/jidctint.o ../../../../microchip/harmony/v1_08/third_party/decoder/jidctint/src/jidctint.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
else
${OBJECTDIR}/_ext/515941952/drv_pmp_static.o: ../src/system_config/bootloader_cfg/framework/driver/pmp/src/drv_pmp_static.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/515941952" 
	@${RM} ${OBJECTDIR}/_ext/515941952/drv_pmp_static.o.d 
	@${RM} ${OBJECTDIR}/_ext/515941952/drv_pmp_static.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/515941952/drv_pmp_static.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/515941952/drv_pmp_static.o.d" -o ${OBJECTDIR}/_ext/515941952/drv_pmp_static.o ../src/system_config/bootloader_cfg/framework/driver/pmp/src/drv_pmp_static.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1010059186/drv_usart_mapping.o: ../src/system_config/bootloader_cfg/framework/driver/usart/src/drv_usart_mapping.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1010059186" 
	@${RM} ${OBJECTDIR}/_ext/1010059186/drv_usart_mapping.o.d 
	@${RM} ${OBJECTDIR}/_ext/1010059186/drv_usart_mapping.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1010059186/drv_usart_mapping.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/1010059186/drv_usart_mapping.o.d" -o ${OBJECTDIR}/_ext/1010059186/drv_usart_mapping.o ../src/system_config/bootloader_cfg/framework/driver/usart/src/drv_usart_mapping.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1010059186/drv_usart_static.o: ../src/system_config/bootloader_cfg/framework/driver/usart/src/drv_usart_static.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1010059186" 
	@${RM} ${OBJECTDIR}/_ext/1010059186/drv_usart_static.o.d 
	@${RM} ${OBJECTDIR}/_ext/1010059186/drv_usart_static.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1010059186/drv_usart_static.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/1010059186/drv_usart_static.o.d" -o ${OBJECTDIR}/_ext/1010059186/drv_usart_static.o ../src/system_config/bootloader_cfg/framework/driver/usart/src/drv_usart_static.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1010059186/drv_usart_static_byte_model.o: ../src/system_config/bootloader_cfg/framework/driver/usart/src/drv_usart_static_byte_model.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1010059186" 
	@${RM} ${OBJECTDIR}/_ext/1010059186/drv_usart_static_byte_model.o.d 
	@${RM} ${OBJECTDIR}/_ext/1010059186/drv_usart_static_byte_model.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1010059186/drv_usart_static_byte_model.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/1010059186/drv_usart_static_byte_model.o.d" -o ${OBJECTDIR}/_ext/1010059186/drv_usart_static_byte_model.o ../src/system_config/bootloader_cfg/framework/driver/usart/src/drv_usart_static_byte_model.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/787999830/sys_clk_static.o: ../src/system_config/bootloader_cfg/framework/system/clk/src/sys_clk_static.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/787999830" 
	@${RM} ${OBJECTDIR}/_ext/787999830/sys_clk_static.o.d 
	@${RM} ${OBJECTDIR}/_ext/787999830/sys_clk_static.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/787999830/sys_clk_static.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/787999830/sys_clk_static.o.d" -o ${OBJECTDIR}/_ext/787999830/sys_clk_static.o ../src/system_config/bootloader_cfg/framework/system/clk/src/sys_clk_static.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/138989690/sys_ports_static.o: ../src/system_config/bootloader_cfg/framework/system/ports/src/sys_ports_static.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/138989690" 
	@${RM} ${OBJECTDIR}/_ext/138989690/sys_ports_static.o.d 
	@${RM} ${OBJECTDIR}/_ext/138989690/sys_ports_static.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/138989690/sys_ports_static.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/138989690/sys_ports_static.o.d" -o ${OBJECTDIR}/_ext/138989690/sys_ports_static.o ../src/system_config/bootloader_cfg/framework/system/ports/src/sys_ports_static.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/310189631/gfx_resources_int_reference.o: ../src/system_config/bootloader_cfg/gfx_resources_int_reference.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/310189631" 
	@${RM} ${OBJECTDIR}/_ext/310189631/gfx_resources_int_reference.o.d 
	@${RM} ${OBJECTDIR}/_ext/310189631/gfx_resources_int_reference.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/310189631/gfx_resources_int_reference.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/310189631/gfx_resources_int_reference.o.d" -o ${OBJECTDIR}/_ext/310189631/gfx_resources_int_reference.o ../src/system_config/bootloader_cfg/gfx_resources_int_reference.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/310189631/system_init.o: ../src/system_config/bootloader_cfg/system_init.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/310189631" 
	@${RM} ${OBJECTDIR}/_ext/310189631/system_init.o.d 
	@${RM} ${OBJECTDIR}/_ext/310189631/system_init.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/310189631/system_init.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/310189631/system_init.o.d" -o ${OBJECTDIR}/_ext/310189631/system_init.o ../src/system_config/bootloader_cfg/system_init.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/310189631/system_interrupt.o: ../src/system_config/bootloader_cfg/system_interrupt.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/310189631" 
	@${RM} ${OBJECTDIR}/_ext/310189631/system_interrupt.o.d 
	@${RM} ${OBJECTDIR}/_ext/310189631/system_interrupt.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/310189631/system_interrupt.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/310189631/system_interrupt.o.d" -o ${OBJECTDIR}/_ext/310189631/system_interrupt.o ../src/system_config/bootloader_cfg/system_interrupt.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/310189631/system_exceptions.o: ../src/system_config/bootloader_cfg/system_exceptions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/310189631" 
	@${RM} ${OBJECTDIR}/_ext/310189631/system_exceptions.o.d 
	@${RM} ${OBJECTDIR}/_ext/310189631/system_exceptions.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/310189631/system_exceptions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/310189631/system_exceptions.o.d" -o ${OBJECTDIR}/_ext/310189631/system_exceptions.o ../src/system_config/bootloader_cfg/system_exceptions.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/310189631/system_tasks.o: ../src/system_config/bootloader_cfg/system_tasks.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/310189631" 
	@${RM} ${OBJECTDIR}/_ext/310189631/system_tasks.o.d 
	@${RM} ${OBJECTDIR}/_ext/310189631/system_tasks.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/310189631/system_tasks.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/310189631/system_tasks.o.d" -o ${OBJECTDIR}/_ext/310189631/system_tasks.o ../src/system_config/bootloader_cfg/system_tasks.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/310189631/gfx_hgc_definitions.o: ../src/system_config/bootloader_cfg/gfx_hgc_definitions.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/310189631" 
	@${RM} ${OBJECTDIR}/_ext/310189631/gfx_hgc_definitions.o.d 
	@${RM} ${OBJECTDIR}/_ext/310189631/gfx_hgc_definitions.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/310189631/gfx_hgc_definitions.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/310189631/gfx_hgc_definitions.o.d" -o ${OBJECTDIR}/_ext/310189631/gfx_hgc_definitions.o ../src/system_config/bootloader_cfg/gfx_hgc_definitions.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/app.o: ../src/app.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/app.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/app.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/1360937237/app.o.d" -o ${OBJECTDIR}/_ext/1360937237/app.o ../src/app.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1360937237/main.o: ../src/main.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1360937237" 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o.d 
	@${RM} ${OBJECTDIR}/_ext/1360937237/main.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1360937237/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/1360937237/main.o.d" -o ${OBJECTDIR}/_ext/1360937237/main.o ../src/main.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1152266019/datastream.o: ../../../../microchip/harmony/v1_08/framework/bootloader/src/datastream/datastream.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1152266019" 
	@${RM} ${OBJECTDIR}/_ext/1152266019/datastream.o.d 
	@${RM} ${OBJECTDIR}/_ext/1152266019/datastream.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1152266019/datastream.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/1152266019/datastream.o.d" -o ${OBJECTDIR}/_ext/1152266019/datastream.o ../../../../microchip/harmony/v1_08/framework/bootloader/src/datastream/datastream.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1152266019/datastream_usart.o: ../../../../microchip/harmony/v1_08/framework/bootloader/src/datastream/datastream_usart.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1152266019" 
	@${RM} ${OBJECTDIR}/_ext/1152266019/datastream_usart.o.d 
	@${RM} ${OBJECTDIR}/_ext/1152266019/datastream_usart.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1152266019/datastream_usart.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/1152266019/datastream_usart.o.d" -o ${OBJECTDIR}/_ext/1152266019/datastream_usart.o ../../../../microchip/harmony/v1_08/framework/bootloader/src/datastream/datastream_usart.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2110890954/bootloader.o: ../../../../microchip/harmony/v1_08/framework/bootloader/src/bootloader.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2110890954" 
	@${RM} ${OBJECTDIR}/_ext/2110890954/bootloader.o.d 
	@${RM} ${OBJECTDIR}/_ext/2110890954/bootloader.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2110890954/bootloader.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/2110890954/bootloader.o.d" -o ${OBJECTDIR}/_ext/2110890954/bootloader.o ../../../../microchip/harmony/v1_08/framework/bootloader/src/bootloader.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2110890954/nvm.o: ../../../../microchip/harmony/v1_08/framework/bootloader/src/nvm.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2110890954" 
	@${RM} ${OBJECTDIR}/_ext/2110890954/nvm.o.d 
	@${RM} ${OBJECTDIR}/_ext/2110890954/nvm.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2110890954/nvm.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/2110890954/nvm.o.d" -o ${OBJECTDIR}/_ext/2110890954/nvm.o ../../../../microchip/harmony/v1_08/framework/bootloader/src/nvm.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/490539985/JpegDecoder.o: ../../../../microchip/harmony/v1_08/framework/decoder/jpeg/JpegDecoder.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/490539985" 
	@${RM} ${OBJECTDIR}/_ext/490539985/JpegDecoder.o.d 
	@${RM} ${OBJECTDIR}/_ext/490539985/JpegDecoder.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/490539985/JpegDecoder.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/490539985/JpegDecoder.o.d" -o ${OBJECTDIR}/_ext/490539985/JpegDecoder.o ../../../../microchip/harmony/v1_08/framework/decoder/jpeg/JpegDecoder.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/314127580/drv_gfx_ili9488.o: ../../../../microchip/harmony/v1_08/framework/driver/gfx/controller/ili9488/src/drv_gfx_ili9488.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/314127580" 
	@${RM} ${OBJECTDIR}/_ext/314127580/drv_gfx_ili9488.o.d 
	@${RM} ${OBJECTDIR}/_ext/314127580/drv_gfx_ili9488.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/314127580/drv_gfx_ili9488.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/314127580/drv_gfx_ili9488.o.d" -o ${OBJECTDIR}/_ext/314127580/drv_gfx_ili9488.o ../../../../microchip/harmony/v1_08/framework/driver/gfx/controller/ili9488/src/drv_gfx_ili9488.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2145404914/gfx.o: ../../../../microchip/harmony/v1_08/framework/gfx/src/gfx.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2145404914" 
	@${RM} ${OBJECTDIR}/_ext/2145404914/gfx.o.d 
	@${RM} ${OBJECTDIR}/_ext/2145404914/gfx.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2145404914/gfx.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/2145404914/gfx.o.d" -o ${OBJECTDIR}/_ext/2145404914/gfx.o ../../../../microchip/harmony/v1_08/framework/gfx/src/gfx.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2145404914/gfx_primitive.o: ../../../../microchip/harmony/v1_08/framework/gfx/src/gfx_primitive.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2145404914" 
	@${RM} ${OBJECTDIR}/_ext/2145404914/gfx_primitive.o.d 
	@${RM} ${OBJECTDIR}/_ext/2145404914/gfx_primitive.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2145404914/gfx_primitive.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/2145404914/gfx_primitive.o.d" -o ${OBJECTDIR}/_ext/2145404914/gfx_primitive.o ../../../../microchip/harmony/v1_08/framework/gfx/src/gfx_primitive.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2145404914/jpeg_image.o: ../../../../microchip/harmony/v1_08/framework/gfx/src/jpeg_image.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2145404914" 
	@${RM} ${OBJECTDIR}/_ext/2145404914/jpeg_image.o.d 
	@${RM} ${OBJECTDIR}/_ext/2145404914/jpeg_image.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2145404914/jpeg_image.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/2145404914/jpeg_image.o.d" -o ${OBJECTDIR}/_ext/2145404914/jpeg_image.o ../../../../microchip/harmony/v1_08/framework/gfx/src/jpeg_image.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/2145404914/gfx_image_decoder.o: ../../../../microchip/harmony/v1_08/framework/gfx/src/gfx_image_decoder.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/2145404914" 
	@${RM} ${OBJECTDIR}/_ext/2145404914/gfx_image_decoder.o.d 
	@${RM} ${OBJECTDIR}/_ext/2145404914/gfx_image_decoder.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/2145404914/gfx_image_decoder.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/2145404914/gfx_image_decoder.o.d" -o ${OBJECTDIR}/_ext/2145404914/gfx_image_decoder.o ../../../../microchip/harmony/v1_08/framework/gfx/src/gfx_image_decoder.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1863813466/sys_devcon.o: ../../../../microchip/harmony/v1_08/framework/system/devcon/src/sys_devcon.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1863813466" 
	@${RM} ${OBJECTDIR}/_ext/1863813466/sys_devcon.o.d 
	@${RM} ${OBJECTDIR}/_ext/1863813466/sys_devcon.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1863813466/sys_devcon.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/1863813466/sys_devcon.o.d" -o ${OBJECTDIR}/_ext/1863813466/sys_devcon.o ../../../../microchip/harmony/v1_08/framework/system/devcon/src/sys_devcon.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/1863813466/sys_devcon_pic32mz.o: ../../../../microchip/harmony/v1_08/framework/system/devcon/src/sys_devcon_pic32mz.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/1863813466" 
	@${RM} ${OBJECTDIR}/_ext/1863813466/sys_devcon_pic32mz.o.d 
	@${RM} ${OBJECTDIR}/_ext/1863813466/sys_devcon_pic32mz.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/1863813466/sys_devcon_pic32mz.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/1863813466/sys_devcon_pic32mz.o.d" -o ${OBJECTDIR}/_ext/1863813466/sys_devcon_pic32mz.o ../../../../microchip/harmony/v1_08/framework/system/devcon/src/sys_devcon_pic32mz.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
${OBJECTDIR}/_ext/980971323/jidctint.o: ../../../../microchip/harmony/v1_08/third_party/decoder/jidctint/src/jidctint.c  nbproject/Makefile-${CND_CONF}.mk
	@${MKDIR} "${OBJECTDIR}/_ext/980971323" 
	@${RM} ${OBJECTDIR}/_ext/980971323/jidctint.o.d 
	@${RM} ${OBJECTDIR}/_ext/980971323/jidctint.o 
	@${FIXDEPS} "${OBJECTDIR}/_ext/980971323/jidctint.o.d" $(SILENT) -rsi ${MP_CC_DIR}../  -c ${MP_CC}  $(MP_EXTRA_CC_PRE)  -g -x c -c -mprocessor=$(MP_PROCESSOR_OPTION)  -ffunction-sections -O1 -I"../src" -I"../src/system_config/bootloader_cfg" -I"../src/bootloader_cfg" -I"../../../../microchip/harmony/v1_08/framework" -I"../src/system_config/bootloader_cfg/framework" -MMD -MF "${OBJECTDIR}/_ext/980971323/jidctint.o.d" -o ${OBJECTDIR}/_ext/980971323/jidctint.o ../../../../microchip/harmony/v1_08/third_party/decoder/jidctint/src/jidctint.c    -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD) 
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compileCPP
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../../../../microchip/harmony/v1_08/bin/framework/peripheral/PIC32MZ1024EFE100_peripherals.a ../../../../microchip/harmony/v1_08/framework/tcpip/src/crypto/aes_pic32mx.a  ../src/system_config/bootloader_cfg/btl_mz.ld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mdebugger -D__MPLAB_DEBUGGER_PK3=1 -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}    ..\..\..\..\microchip\harmony\v1_08\bin\framework\peripheral\PIC32MZ1024EFE100_peripherals.a ..\..\..\..\microchip\harmony\v1_08\framework\tcpip\src\crypto\aes_pic32mx.a      -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD)      -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1,--defsym=_min_heap_size=4096,--gc-sections,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml
	
else
dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk  ../../../../microchip/harmony/v1_08/bin/framework/peripheral/PIC32MZ1024EFE100_peripherals.a ../../../../microchip/harmony/v1_08/framework/tcpip/src/crypto/aes_pic32mx.a ../src/system_config/bootloader_cfg/btl_mz.ld
	@${MKDIR} dist/${CND_CONF}/${IMAGE_TYPE} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -mprocessor=$(MP_PROCESSOR_OPTION)  -o dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}    ..\..\..\..\microchip\harmony\v1_08\bin\framework\peripheral\PIC32MZ1024EFE100_peripherals.a ..\..\..\..\microchip\harmony\v1_08\framework\tcpip\src\crypto\aes_pic32mx.a      -DXPRJ_bootloader_cfg=$(CND_CONF)    $(COMPARISON_BUILD)  -Wl,--defsym=__MPLAB_BUILD=1$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=_min_heap_size=4096,--gc-sections,-Map="${DISTDIR}/${PROJECTNAME}.${IMAGE_TYPE}.map",--memorysummary,dist/${CND_CONF}/${IMAGE_TYPE}/memoryfile.xml
	${MP_CC_DIR}\\xc32-bin2hex dist/${CND_CONF}/${IMAGE_TYPE}/bootloader.X.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} 
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r build/bootloader_cfg
	${RM} -r dist/bootloader_cfg

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
