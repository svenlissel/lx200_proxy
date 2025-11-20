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
typedef struct {
    volatile uint8_t active;
    volatile int32_t remaining_ms;
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

    switch(direction)
    {
        case ST4_NORTH:
            st4_states.north.remaining_ms = duration_ms;
            if(duration_ms > 0)
            {
                st4_states.north.active = 1;
                HAL_GPIO_WritePin(ST4_PORT, ST4_NORTH_Pin, GPIO_PIN_RESET); // signal is active low
                UART_Printf(UART_DEBUG, "ST4 North: ON for %lu ms\r\n", duration_ms);
            }
            else
            {
                st4_states.north.active = 0;
                HAL_GPIO_WritePin(ST4_PORT, ST4_NORTH_Pin, GPIO_PIN_SET); // set to floating (high impedance)
                UART_Printf(UART_DEBUG, "ST4 North: OFF command\r\n");
            }
            break;
            
        case ST4_SOUTH:
            st4_states.south.remaining_ms = duration_ms;
            if(duration_ms > 0)
            {
                st4_states.south.active = 1;
                HAL_GPIO_WritePin(ST4_PORT, ST4_SOUTH_Pin, GPIO_PIN_RESET); // signal is active low
                UART_Printf(UART_DEBUG, "ST4 South: ON for %lu ms\r\n", duration_ms);
            }
            else
            {
                st4_states.south.active = 0;
                HAL_GPIO_WritePin(ST4_PORT, ST4_SOUTH_Pin, GPIO_PIN_SET); // set to floating (high impedance)
                UART_Printf(UART_DEBUG, "ST4 South: OFF command\r\n");
            }
            break;
        case ST4_EAST:
            st4_states.east.remaining_ms = duration_ms;
            if(duration_ms > 0)
            {
                st4_states.east.active = 1;
                HAL_GPIO_WritePin(ST4_PORT, ST4_EAST_Pin, GPIO_PIN_RESET); // signal is active low
                UART_Printf(UART_DEBUG, "ST4 East: ON for %lu ms\r\n", duration_ms);
            }
            else
            {
                st4_states.east.active = 0;
                HAL_GPIO_WritePin(ST4_PORT, ST4_EAST_Pin, GPIO_PIN_SET); // set to floating (high impedance)
                UART_Printf(UART_DEBUG, "ST4 East: OFF command\r\n");
            }
            break;
        case ST4_WEST:
            st4_states.west.remaining_ms = duration_ms;
            if(duration_ms > 0)
            {
                st4_states.west.active = 1;
                HAL_GPIO_WritePin(ST4_PORT, ST4_WEST_Pin, GPIO_PIN_RESET); // signal is active low
                UART_Printf(UART_DEBUG, "ST4 West: ON for %lu ms\r\n", duration_ms);
            }
            else
            {
                st4_states.west.active = 0;
                HAL_GPIO_WritePin(ST4_PORT, ST4_WEST_Pin, GPIO_PIN_SET); // set to floating (high impedance)
                UART_Printf(UART_DEBUG, "ST4 West: OFF command\r\n");
            }
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
    // Check each 10ms if one of the 4 ST4 GPIO pins should be still active low
    // IMPORTANT: This function runs not in interrupt context, so __disable_irq() is needed
     __disable_irq();

    // North pin
    if(st4_states.north.active && st4_states.north.remaining_ms > 0)
    {
        st4_states.north.remaining_ms-=10;
        if(st4_states.north.remaining_ms <= 0)
        {
            st4_states.north.active = 0;
            HAL_GPIO_WritePin(ST4_PORT, ST4_NORTH_Pin, GPIO_PIN_SET); // set to floating (high impedance)
            UART_Printf(UART_DEBUG, "ST4 North: OFF\r\n");
        }
    }
    // South pin
    if(st4_states.south.active && st4_states.south.remaining_ms > 0)
    {
        st4_states.south.remaining_ms-=10;
        if(st4_states.south.remaining_ms <= 0)
        {
            st4_states.south.active = 0;
            HAL_GPIO_WritePin(ST4_PORT, ST4_SOUTH_Pin, GPIO_PIN_SET); // set to floating (high impedance)
            UART_Printf(UART_DEBUG, "ST4 South: OFF\r\n");
        }
    }
    // East pin
    if(st4_states.east.active && st4_states.east.remaining_ms > 0)
    {
        st4_states.east.remaining_ms-=10;
        if(st4_states.east.remaining_ms <= 0)
        {
            st4_states.east.active = 0;
            HAL_GPIO_WritePin(ST4_PORT, ST4_EAST_Pin, GPIO_PIN_SET); // set to floating (high impedance)
            UART_Printf(UART_DEBUG, "ST4 East: OFF\r\n");
        }
    }
    // West pin
    if(st4_states.west.active && st4_states.west.remaining_ms > 0)
    {
        st4_states.west.remaining_ms-=10;
        if(st4_states.west.remaining_ms <= 0)
        {
            st4_states.west.active = 0;
            HAL_GPIO_WritePin(ST4_PORT, ST4_WEST_Pin, GPIO_PIN_SET); // set to floating (high impedance)
            UART_Printf(UART_DEBUG, "ST4 West: OFF\r\n");
        }
    }

    __enable_irq();
}
