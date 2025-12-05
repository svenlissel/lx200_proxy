/*
 ******************************************************************************
 * @file    st4_handler.c
 * @brief   ST4 GPIO handler for telescope guiding interface
 * @author  Sven Lissel
 * @date    2025
 ******************************************************************************
 */

/* ============================================================================
 *                              INCLUDES
 * ============================================================================ */
#include "st4_handler.h"

/* ============================================================================
 *                         PRIVATE TYPES & STRUCTS
 * ============================================================================ */
/* using ticktimes to be safe for race conditions with interrupts */
/* setter functiions only sets tick time for off timepoint */
typedef struct {
    volatile uint8_t active;
    volatile int32_t off_ticktime;
} ST4_Pin_State_t;

typedef struct {
    volatile ST4_Pin_State_t north;
    volatile ST4_Pin_State_t south;
    volatile ST4_Pin_State_t east; 
    volatile ST4_Pin_State_t west;
} ST4_States_t;

/* ============================================================================
 *                         PRIVATE VARIABLES
 * ============================================================================ */
static ST4_States_t st4_states = {0};

#define ENABLE_DEBUG_PRINTF 0

/* Debug output macros */
#if ENABLE_DEBUG_PRINTF >= 1
    #define DEBUG_PRINTF(...) UART_Printf( __VA_ARGS__)
#else
    #define DEBUG_PRINTF(...)
#endif

/* ============================================================================
 *                         PUBLIC FUNCTIONS
 * ============================================================================ */

 /**
 * @brief Extract duration from guiding command for ST4 movement
 * @param command: Full LX200 command string
 * @retval Duration in milliseconds
 */
uint32_t st4_parse_duration(char* command)
{
    if(strlen(command) > 5) 
    {
        char duration_str[16];
        strncpy(duration_str, command + 4, strlen(command) - 5); // Extract duration between ":Mgx" and "#"
        duration_str[strlen(command) - 5] = '\0';
        int duration = atoi(duration_str);
        return duration;
    } 
    else 
    {
        /* Default duration */
        return 1000;
    }
}

void st4_set(ST4_Direction_t direction, uint32_t duration_ms)
{
    uint32_t time2set = HAL_GetTick() + duration_ms;

    switch(direction)
    {
        case ST4_NORTH:
                st4_states.north.off_ticktime = time2set;
            break;
        case ST4_SOUTH:
                st4_states.south.off_ticktime = time2set;
            break;
        case ST4_EAST:
                st4_states.east.off_ticktime = time2set;
            break;
        case ST4_WEST:
                st4_states.west.off_ticktime = time2set;
            break;     
        default:
            return;
    }
}

/* ----------------------------------------------------------------------------
 *                         ST4 CYCLIC PROCESSING
 * ---------------------------------------------------------------------------- */
void st4_process(void)
{
    int32_t remaining_ticks;
    /* int32_t cast handles HAL_GetTick() overflow correctly (two's complement arithmetic) */
    /* Race condition safe: interrupt only writes off_ticktime, process only reads it atomically */

    /* North */
    remaining_ticks = (int32_t)(st4_states.north.off_ticktime - HAL_GetTick());
    if((!st4_states.north.active) && (remaining_ticks > 0))
    {
        //turn on!
        st4_states.north.active = 1;
        HAL_GPIO_WritePin(ST4_PORT, ST4_NORTH_Pin, GPIO_PIN_RESET); // signal is active low
        DEBUG_PRINTF(UART_DEBUG, "ST4 North: ON %dms\r\n", remaining_ticks);
    }
    else if((st4_states.north.active) && (remaining_ticks <= 0))
    {
        // switch off!
        st4_states.north.active = 0;
        HAL_GPIO_WritePin(ST4_PORT, ST4_NORTH_Pin, GPIO_PIN_SET); // set to floating (high impedance)
        DEBUG_PRINTF(UART_DEBUG, "ST4 North: OFF\r\n");
    }

    /* South */
    remaining_ticks = (int32_t)(st4_states.south.off_ticktime - HAL_GetTick());
    if((!st4_states.south.active) && (remaining_ticks > 0))
    {
        //turn on!
        st4_states.south.active = 1;
        HAL_GPIO_WritePin(ST4_PORT, ST4_SOUTH_Pin, GPIO_PIN_RESET); // signal is active low
        DEBUG_PRINTF(UART_DEBUG, "ST4 South: ON %dms\r\n", remaining_ticks);
    }
    else if((st4_states.south.active) && (remaining_ticks <= 0))
    {
        // switch off!
        st4_states.south.active = 0;
        HAL_GPIO_WritePin(ST4_PORT, ST4_SOUTH_Pin, GPIO_PIN_SET); // set to floating (high impedance)
        DEBUG_PRINTF(UART_DEBUG, "ST4 South: OFF\r\n");
    }

    /* East */
    remaining_ticks = (int32_t)(st4_states.east.off_ticktime - HAL_GetTick());
    if((!st4_states.east.active) && (remaining_ticks > 0))
    {
        //turn on!
        st4_states.east.active = 1;
        HAL_GPIO_WritePin(ST4_PORT, ST4_EAST_Pin, GPIO_PIN_RESET); // signal is active low
        DEBUG_PRINTF(UART_DEBUG, "ST4 East: ON %dms\r\n", remaining_ticks);
    }
    else if((st4_states.east.active) && (remaining_ticks <= 0))
    {
        // switch off!
        st4_states.east.active = 0;
        HAL_GPIO_WritePin(ST4_PORT, ST4_EAST_Pin, GPIO_PIN_SET); // set to floating (high impedance)
        DEBUG_PRINTF(UART_DEBUG, "ST4 East: OFF\r\n");
    }

    /* West */
    remaining_ticks = (int32_t)(st4_states.west.off_ticktime - HAL_GetTick());
    if((!st4_states.west.active) && (remaining_ticks > 0))
    {
        //turn on!
        st4_states.west.active = 1;
        HAL_GPIO_WritePin(ST4_PORT, ST4_WEST_Pin, GPIO_PIN_RESET); // signal is active low
        DEBUG_PRINTF(UART_DEBUG, "ST4 West: ON %dms\r\n", remaining_ticks);
    }
    else if((st4_states.west.active) && (remaining_ticks <= 0))
    {
        // switch off!
        st4_states.west.active = 0;
        HAL_GPIO_WritePin(ST4_PORT, ST4_WEST_Pin, GPIO_PIN_SET); // set to floating (high impedance)
        DEBUG_PRINTF(UART_DEBUG, "ST4 West: OFF\r\n");
    }
}
