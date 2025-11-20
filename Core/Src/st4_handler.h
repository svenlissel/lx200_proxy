/*
 ******************************************************************************
 * @file    st4_handler.h
 * @brief   Header for ST4 GPIO handler for telescope guiding interface
 * @author  Sven Lissel
 * @date    2025
 ******************************************************************************
 */

#ifndef ST4_HANDLER_H
#define ST4_HANDLER_H

/* ============================================================================
 *                              INCLUDES
 * ============================================================================ */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "main.h"  // For HAL includes and ST4 pin definitions

/* ============================================================================
 *                         PUBLIC TYPES
 * ============================================================================ */

// ST4 Direction Enum
typedef enum {
    ST4_NORTH = 0,
    ST4_SOUTH = 1,
    ST4_EAST = 2,
    ST4_WEST = 3
} ST4_Direction_t;

/* ============================================================================
 *                         PUBLIC FUNCTION PROTOTYPES
 * ============================================================================ */

// functions
void st4_process(void);
void st4_set(ST4_Direction_t direction, uint32_t duration_ms);
uint32_t st4_parse_duration(char* command);

#endif // ST4_HANDLER_H