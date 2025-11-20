/*
 ******************************************************************************
 * @file    lx200_server.c
 * @brief   LX200 protocol handler and command processor
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
#include "lx200_emulator.h"
#include "lx200_fs2_adapter.h" 

/* ============================================================================
 *                         PRIVATE DEFINES
 * ============================================================================ */
#define LX200_CMD_BUFFER_SIZE 64

/* ============================================================================
 *                         PRIVATE VARIABLES
 * ============================================================================ */
static char lx200_cmd_buffer[LX200_CMD_BUFFER_SIZE];

/* ============================================================================
 *                         PRIVATE FUNCTION PROTOTYPES
 * ============================================================================ */

/* ============================================================================
 *                         PUBLIC FUNCTIONS
 * ============================================================================ */

// Function for processing LX200 commands
void ProcessLX200Command(char* command)
{

    //ProcessLX200Command_Emulator(command);
    ProcessLX200Command_FS2(command);
}

/* ----------------------------------------------------------------------------
 *                         LX200 COMMAND PARSER
 * ---------------------------------------------------------------------------- */

// parsing LX200 commands from received data
void ParseLX200Data(uint8_t* data, uint32_t length)
{
    static uint8_t lx200_cmd_index = 0;
    static uint8_t lx200_cmd_started = 0;

    for(uint32_t i = 0; i < length; i++)
    {
        char current_char = (char)data[i];       

        // If command has already started, collect characters
        if(lx200_cmd_started)
        {
            if(lx200_cmd_index < LX200_CMD_BUFFER_SIZE - 1)
            {
                lx200_cmd_buffer[lx200_cmd_index++] = current_char;
                
                // Search for end of LX200 command '#'
                if(current_char == '#')
                {
                    lx200_cmd_buffer[lx200_cmd_index] = '\0'; // Terminate string
                    ProcessLX200Command(lx200_cmd_buffer); // Process command
                    
                    // Reset for next command
                    lx200_cmd_started = 0;
                    lx200_cmd_index = 0;
                }
            }
            else
            {
                // Buffer overflow - command too long, reset
                lx200_cmd_started = 0;
                lx200_cmd_index = 0;
            }
        }
        // Search for start of LX200 command ':'
        else if(current_char == ':')
        {
            lx200_cmd_started = 1;
            lx200_cmd_index = 0;
            lx200_cmd_buffer[lx200_cmd_index++] = current_char;
        }
        // Special handling for ACK (0x06)
        else if(current_char == 0x06)
        {
            UART_Printf(UART_DEBUG, "ACK (0x06) received\r\n");
            // Send response directly: "G" for Autostar/LX200GPS
            char ack_response[] = "G";
            CDC_Transmit_FS((uint8_t*)ack_response, strlen(ack_response));
            UART_Printf(UART_DEBUG, "-> ACK Response: G\r\n");
            continue; // Process next character
        }
        
        // Ignore characters that are not part of an LX200 command
    }
}
