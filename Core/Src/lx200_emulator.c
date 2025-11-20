/*
 ******************************************************************************
 * @file    lx200_emulator.c
 * @brief   LX200 telescope emulator for testing and development
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

/* ============================================================================
 *                         PRIVATE DEFINES
 * ============================================================================ */

/* ============================================================================
 *                         PRIVATE VARIABLES
 * ============================================================================ */

/* ============================================================================
 *                         PUBLIC FUNCTIONS
 * ============================================================================ */

/* ----------------------------------------------------------------------------
 *                         LX200 TELESCOPE EMULATOR
 * ---------------------------------------------------------------------------- */

// Function for processing LX200 commands
void ProcessLX200Command_Emulator(char* command)
{
    // Debug output of recognized command
    UART_Printf(UART_DEBUG, "%s\r\n", command);
    
    char response[64] = "";
    
    // LX200 Command parsing and responses
    if(strncmp(command, ":GR#", 4) == 0)
    {
        // Get Right Ascension
        strcpy(response, "12:34:56#");
        UART_Printf(UART_DEBUG, "-> Get RA: %s\r\n", response);
    }
    else if(strncmp(command, ":GD#", 4) == 0)
    {
        // Get Declination
        strcpy(response, "+45*30:45#");
        UART_Printf(UART_DEBUG, "-> Get DEC: %s\r\n", response);
    }
    else if(strncmp(command, ":GM#", 4) == 0)
    {
        // Get Site 1 Name
        strcpy(response, "STM32 Site#");
        UART_Printf(UART_DEBUG, "-> Get Site Name: %s\r\n", response);
    }
    else if(strncmp(command, ":Gt#", 4) == 0)
    {
        // Get Current Site Latitude
        strcpy(response, "+50*30:00#");
        UART_Printf(UART_DEBUG, "-> Get Site Latitude: %s\r\n", response);
    }
    else if(strncmp(command, ":Gg#", 4) == 0)
    {
        // Get Current Site Longitude  
        strcpy(response, "+010*15:30#");
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
        // Set Right Ascension
        strcpy(response, "1");
        UART_Printf(UART_DEBUG, "-> Set RA: OK\r\n");
    }
    else if(strncmp(command, ":Sd", 3) == 0)
    {
        // Set Declination
        strcpy(response, "1");
        UART_Printf(UART_DEBUG, "-> Set DEC: OK\r\n");
    }
    else if(strncmp(command, ":MS#", 4) == 0)
    {
        // Move to target (Slew)
        strcpy(response, "0");
        UART_Printf(UART_DEBUG, "-> Move to target: OK\r\n");
    }
    else if(strncmp(command, ":Q#", 3) == 0)
    {
        // Halt all movement
        strcpy(response, "");
        UART_Printf(UART_DEBUG, "-> Halt all movement\r\n");
    }
    else if(strncmp(command, ":Qn#", 4) == 0)
    {
        // Halt North movement
        strcpy(response, "");
        UART_Printf(UART_DEBUG, "-> Halt North movement\r\n");
    }
    else if(strncmp(command, ":Qs#", 4) == 0)
    {
        // Halt South movement
        strcpy(response, "");
        UART_Printf(UART_DEBUG, "-> Halt South movement\r\n");
    }
    else if(strncmp(command, ":Qe#", 4) == 0)
    {
        // Halt East movement
        strcpy(response, "");
        UART_Printf(UART_DEBUG, "-> Halt East movement\r\n");
    }
    else if(strncmp(command, ":Qw#", 4) == 0)
    {
        // Halt West movement
        strcpy(response, "");
        UART_Printf(UART_DEBUG, "-> Halt West movement\r\n");
    }
    else if(strncmp(command, ":Mn#", 4) == 0)
    {
        // Move North
        strcpy(response, "");
        UART_Printf(UART_DEBUG, "-> Move North\r\n");
    }
    else if(strncmp(command, ":Ms#", 4) == 0)
    {
        // Move South
        strcpy(response, "");
        UART_Printf(UART_DEBUG, "-> Move South\r\n");
    }
    else if(strncmp(command, ":Me#", 4) == 0)
    {
        // Move East
        strcpy(response, "");
        UART_Printf(UART_DEBUG, "-> Move East\r\n");
    }
    else if(strncmp(command, ":Mw#", 4) == 0)
    {
        // Move West
        strcpy(response, "");
        UART_Printf(UART_DEBUG, "-> Move West\r\n");
    }
    else if(strncmp(command, ":Mgn", 4) == 0)
    {
        // Move guide rate North with duration parameter
        if(strlen(command) > 5) {
            char duration_str[16];
            strncpy(duration_str, command + 4, strlen(command) - 5); // Extract duration between ":Mgn" and "#"
            duration_str[strlen(command) - 5] = '\0';
            int duration = atoi(duration_str);
            UART_Printf(UART_DEBUG, "-> Move guide rate North for %d ms\r\n", duration);
        } else {
            UART_Printf(UART_DEBUG, "-> Move guide rate North\r\n");
        }
        strcpy(response, "");
    }
    else if(strncmp(command, ":Mgs", 4) == 0)
    {
        // Move guide rate South with duration parameter
        if(strlen(command) > 5) {
            char duration_str[16];
            strncpy(duration_str, command + 4, strlen(command) - 5);
            duration_str[strlen(command) - 5] = '\0';
            int duration = atoi(duration_str);
            UART_Printf(UART_DEBUG, "-> Move guide rate South for %d ms\r\n", duration);
        } else {
            UART_Printf(UART_DEBUG, "-> Move guide rate South\r\n");
        }
        strcpy(response, "");
    }
    else if(strncmp(command, ":Mge", 4) == 0)
    {
        // Move guide rate East with duration parameter
        if(strlen(command) > 5) {
            char duration_str[16];
            strncpy(duration_str, command + 4, strlen(command) - 5);
            duration_str[strlen(command) - 5] = '\0';
            int duration = atoi(duration_str);
            UART_Printf(UART_DEBUG, "-> Move guide rate East for %d ms\r\n", duration);
        } else {
            UART_Printf(UART_DEBUG, "-> Move guide rate East\r\n");
        }
        strcpy(response, "");
    }
    else if(strncmp(command, ":Mgw", 4) == 0)
    {
        // Move guide rate West with duration parameter
        if(strlen(command) > 5) {
            char duration_str[16];
            strncpy(duration_str, command + 4, strlen(command) - 5);
            duration_str[strlen(command) - 5] = '\0';
            int duration = atoi(duration_str);
            UART_Printf(UART_DEBUG, "-> Move guide rate West for %d ms\r\n", duration);
        } else {
            UART_Printf(UART_DEBUG, "-> Move guide rate West\r\n");
        }
        strcpy(response, "");
    }
    else if(strncmp(command, ":RS#", 4) == 0)
    {
        // Set slew rate to fastest
        strcpy(response, "");
        UART_Printf(UART_DEBUG, "-> Set slew rate: Fastest\r\n");
    }
    else if(strncmp(command, ":RM#", 4) == 0)
    {
        // Set slew rate to medium
        strcpy(response, "");
        UART_Printf(UART_DEBUG, "-> Set slew rate: Medium\r\n");
    }
    else if(strncmp(command, ":RC#", 4) == 0)
    {
        // Set slew rate to centering
        strcpy(response, "");
        UART_Printf(UART_DEBUG, "-> Set slew rate: Centering\r\n");
    }
    else if(strncmp(command, ":RG#", 4) == 0)
    {
        // Set slew rate to guiding
        strcpy(response, "");
        UART_Printf(UART_DEBUG, "-> Set slew rate: Guiding (slowest)\r\n");
    }
    else if(strncmp(command, ":CM#", 4) == 0)
    {
        // Sync telescope
        strcpy(response, "");
        UART_Printf(UART_DEBUG, "-> Sync telescope\r\n");
    }
    else if(strncmp(command, ":U#", 3) == 0)
    {
        // Toggle precision mode
        strcpy(response, "");
        UART_Printf(UART_DEBUG, "-> Toggle precision mode\r\n");
    }
    // FS2 not supportted commands
    else if(strncmp(command, ":Mgn#", 5) == 0)
    {
        // Move guide rate North
        strcpy(response, "");
        UART_Printf(UART_DEBUG, "-> Move guide rate North\r\n");
    }
    else if(strncmp(command, ":Mgs#", 5) == 0)
    {
        // Move guide rate South
        strcpy(response, "");
        UART_Printf(UART_DEBUG, "-> Move guide rate South\r\n");
    }
    else if(strncmp(command, ":Mge#", 5) == 0)
    {
        // Move guide rate East
        strcpy(response, "");
        UART_Printf(UART_DEBUG, "-> Move guide rate East\r\n");
    }
    else if(strncmp(command, ":Mgw#", 5) == 0)
    {
        // Move guide rate West
        strcpy(response, "");
        UART_Printf(UART_DEBUG, "-> Move guide rate West\r\n");
    }
    else
    {
        // Unknown command
        UART_Printf(UART_DEBUG, "!! Unknown command\r\n");
        //return; // Do not send response
    }
    
    // Send response via USB CDC (if available)
    if(strlen(response) > 0)
    {
        CDC_Transmit_FS((uint8_t*)response, strlen(response));
    }
}
