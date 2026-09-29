config DRV_GFX_GLCD_LAYERS_NUMBER_GT_${INSTANCE+1}
    bool
    depends on USE_DRV_GFX_GLCD
<#if INSTANCE != 0>
    default n if DRV_GFX_GLCD_LAYERS_NUMBER_GT_${INSTANCE} = n
</#if>	
    default n if DRV_GFX_GLCD_LAYERS_NUMBER = ${INSTANCE+1}
	default y
	
config DRV_GFX_GLCD_LAYER_${INSTANCE}
    depends on USE_DRV_GFX_GLCD
<#if INSTANCE != 0>
                 && DRV_GFX_GLCD_LAYERS_NUMBER_GT_${INSTANCE}
</#if>
    bool "GLCD Layer ${INSTANCE}"
    default y
    ---help---
    IDH_HTML_Graphics_Driver_Library
    ---endhelp---

ifblock DRV_GFX_GLCD_LAYER_${INSTANCE}

menu "RTOS Configuration (Instance ${INSTANCE})"
    depends on USE_DRV_GFX_GLCD
    depends on USE_3RDPARTY_RTOS
    depends on DRV_TMR_DRIVER_MODE = "DYNAMIC"

config DRV_GFX_GLCD_RTOS_IDX${INSTANCE}
    string "Run This Driver Instance As"
    range 3RDPARTY_RTOS_SYS_TASKS_OPTIONS
    default "Combined with System Tasks"

config DRV_GFX_GLCD_IDX${INSTANCE}_RTOS_TASK_SIZE
    int "Task Size"
    depends on DRV_GFX_GLCD_RTOS_IDX${INSTANCE} = "Standalone"
    default 1024

config DRV_GFX_GLCD_IDX${INSTANCE}_RTOS_TASK_PRIORITY
    int "Task Priority"
    depends on DRV_GFX_GLCD_RTOS_IDX${INSTANCE} = "Standalone"
    default 1

config DRV_GFX_GLCD_IDX${INSTANCE}_RTOS_USE_DELAY
    bool "Use Task Delay?"
    depends on DRV_GFX_GLCD_RTOS_IDX${INSTANCE} = "Standalone"
    default y

config DRV_GFX_GLCD__IDX${INSTANCE}_RTOS_DELAY
    int "Task Delay"
    depends on DRV_GFX_GLCD_RTOS_IDX${INSTANCE} = "Standalone"
    depends on DRV_GFX_GLCD_IDX${INSTANCE}_RTOS_USE_DELAY
    default 1000
endmenu

config DRV_GFX_GLCD_LAYER${INSTANCE}_RES_X
    int "Resolution X"
    depends on DRV_GFX_GLCD_LAYER_${INSTANCE}
    default 0
    ---help---
    IDH_HTML_Graphics_Driver_Library
    ---endhelp---
 
config DRV_GFX_GLCD_LAYER${INSTANCE}_RES_Y
    int "Resolution Y"
    depends on DRV_GFX_GLCD_LAYER_${INSTANCE}
    default 0
    ---help---
    IDH_HTML_Graphics_Driver_Library
    ---endhelp---

config DRV_GFX_GLCD_LAYER${INSTANCE}_START_X
    int "Start X"
    depends on DRV_GFX_GLCD_LAYER_${INSTANCE}
    default 0
    ---help---
    IDH_HTML_Graphics_Driver_Library
    ---endhelp---

config DRV_GFX_GLCD_LAYER${INSTANCE}_START_Y
    int "Start Y"
    depends on DRV_GFX_GLCD_LAYER_${INSTANCE}
    default 0
    ---help---
    IDH_HTML_Graphics_Driver_Library
    ---endhelp---

config DRV_GFX_GLCD_LAYER${INSTANCE}_COLORSPACE
    depends on DRV_GFX_GLCD_LAYER_${INSTANCE}
    string "Color Space"
    range GLCD_LAYER_COLOR_MODE
    default "GLCD_LAYER_COLOR_MODE_RGBA8888"
    ---help---
    IDH_HTML_SYSTEM_Library_Interface
    ---endhelp---

config DRV_GFX_GLCD_LAYER${INSTANCE}_ALPHA_VALUE
    depends on DRV_GFX_GLCD_LAYER_${INSTANCE}
    string "Alpha Value"
    default "0xFF"
    ---help---
    IDH_HTML_SYSTEM_Library_Interface
    ---endhelp---

config DRV_GFX_GLCD_LAYER${INSTANCE}_DESTINATION_BLEND
    depends on DRV_GFX_GLCD_LAYER_${INSTANCE}
    string "Alpha Blend Destination Function"
    range GLCD_LAYER_DEST_BLEND_FUNC
    default "GLCD_LAYER_DEST_BLEND_BLACK"
    ---help---
    IDH_HTML_SYSTEM_Library_Interface
    ---endhelp---

config DRV_GFX_GLCD_LAYER${INSTANCE}_SOURCE_BLEND
    depends on DRV_GFX_GLCD_LAYER_${INSTANCE}
    string "Alpha Blend Source Function"
    range GLCD_LAYER_SRC_BLEND_FUNC
    default "GLCD_LAYER_SRC_BLEND_WHITE"
    ---help---
    IDH_HTML_SYSTEM_Library_Interface
    ---endhelp---
endif
