/*
 ******************************************************************************
 * @file    lx200_fs2_adapter.c
 * @brief   FS2 telescope command adapter for LX200 protocol
 * @author  Sven Lissel
 * @date    2025
 ******************************************************************************
 */

/* ============================================================================
 *                              INCLUDES
 * ============================================================================ */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "usbd_cdc_if.h"
#include "st4_handler.h"

/* ============================================================================
 *                         PRIVATE DEFINES
 * ============================================================================ */

/* ============================================================================
 *                         PRIVATE VARIABLES
 * ============================================================================ */
static char corrected_command[64];

/* ============================================================================
 *                         PRIVATE FUNCTIONS
 * ============================================================================ */

/* ============================================================================
 *                         PUBLIC FUNCTIONS
 * ============================================================================ */

/* ----------------------------------------------------------------------------
 *                         FS2 COMMAND PROCESSOR
 * ---------------------------------------------------------------------------- */
void ProcessLX200Command_FS2(char* command)
{
    // Debug output of recognized command
    UART_Printf(UART_DEBUG, "%s\r\n", command);
    
    char response[64] = "";
    
    // LX200 Kommando-Parsing for non FS commands
    if(strncmp(command, ":GM#", 4) == 0)
    {
        // Get Site 1 Name
        strcpy(response, "LX200 Site#");
        UART_Printf(UART_DEBUG, "-> Get Site Name: %s\r\n", response);
    }
    else if(strncmp(command, ":Gt#", 4) == 0)
    {
        // Get Current Site Latitude
        strcpy(response, "+47*59:46#");
        UART_Printf(UART_DEBUG, "-> Get Site Latitude: %s\r\n", response);
    }
    else if(strncmp(command, ":Gg#", 4) == 0)
    {
        // Get Current Site Longitude  
        strcpy(response, "+007*51:10#");
        UART_Printf(UART_DEBUG, "-> Get Site Longitude: %s\r\n", response);
    }
    else if(strncmp(command, ":GT#", 4) == 0)
    {
        // Get Tracking Rate
        strcpy(response, "60.1#");
        UART_Printf(UART_DEBUG, "-> Get Tracking Rate: %s\r\n", response);
    }
    else if(strncmp(command, ":Sr", 3) == 0)
    {
        // Set Right Ascension, check for FS2 must have space character
        if(command[3] == ' ')
        {
            // all ok, send direct to FS2
            UART_Printf(UART_DEBUG, "-> space ok, send to FS2\r\n");
            UART_Printf(UART_OUT, command);
        }
        else
        {
            // No space after ":Sr" - insert one at position 3
            strncpy(&corrected_command[0], &command[0],3);
            corrected_command[3] = ' ';
            strcpy(&corrected_command[4], &command[3]);
            
            UART_Printf(UART_DEBUG, "-> space inserted, corrected: %s\r\n", corrected_command);
            UART_Printf(UART_OUT, corrected_command);
        }
        strcpy(response, "");
    }
    else if(strncmp(command, ":Sd", 3) == 0)
    {
        // Set Declination, check for FS2 must have space character
        if(command[3] == ' ')
        {
            // all ok, send direct to FS2
            UART_Printf(UART_DEBUG, "-> space ok, send to FS2\r\n");
            UART_Printf(UART_OUT, command);
        }
        else
        {
            // No space after ":Sd" - insert one at position 3
            strncpy(&corrected_command[0], &command[0],3);
            corrected_command[3] = ' ';
            strcpy(&corrected_command[4], &command[3]);
            
            UART_Printf(UART_DEBUG, "-> space inserted, corrected: %s\r\n", corrected_command);
            UART_Printf(UART_OUT, corrected_command);
        }
        strcpy(response, "");
    }
    else if(strncmp(command, ":MS#", 4) == 0)
    {
        // there is a bug in the FS2 that the MS (move to target) command is aborted -> send two times
        UART_Printf(UART_OUT, ":MS#");
        UART_Printf(UART_DEBUG, "-> FS2 BUGFIX, send :MS# two times\r\n");
        volatile uint32_t count = 50 * 18000;
        while(count--);
        UART_Printf(UART_OUT, ":MS#");
        strcpy(response, "");
    }
    else if(strncmp(command, ":Q#", 3) == 0)
    {
        // there is a bug in the FS2 that the Q (stop movingt) not executed -> send two times
        UART_Printf(UART_OUT, ":Q#");
        UART_Printf(UART_DEBUG, "-> FS2 BUGFIX, send :Q# two times\r\n");
        volatile uint32_t count = 50 * 18000;
        while(count--);
        UART_Printf(UART_OUT, ":Q#");
        strcpy(response, "");
    }
    /* Guiding commands, will be mapped to ST4 output*/
    else if(strncmp(command, ":Mgn", 4) == 0)
    {
        // Move guide rate North with duration parameter
        uint32_t duration = st4_parse_duration(command);
        st4_set(ST4_NORTH, duration);
        strcpy(response, "");
    }
    else if(strncmp(command, ":Mgs", 4) == 0)
    {
        // Move guide rate South with duration parameter
        uint32_t duration = st4_parse_duration(command);
        st4_set(ST4_SOUTH, duration);
        strcpy(response, "");
    }
    else if(strncmp(command, ":Mge", 4) == 0)
    {
        // Move guide rate East with duration parameter
        uint32_t duration = st4_parse_duration(command);
        st4_set(ST4_EAST, duration);
        strcpy(response, "");
    }
    else if(strncmp(command, ":Mgw", 4) == 0)
    {
        // Move guide rate West with duration parameter
        uint32_t duration = st4_parse_duration(command);
        st4_set(ST4_WEST, duration);
        strcpy(response, "");
    }
    else
    {
        /* not handled command, send direct to FS2 */
        strcpy(response, "");
        UART_Printf(UART_OUT, command);
        UART_Printf(UART_DEBUG, "-> send to FS2\r\n");
        return;
    }
    
    // Send response via USB CDC (if available)
    if(strlen(response) > 0)
    {
        CDC_Transmit_FS((uint8_t*)response, strlen(response));
    }
}
