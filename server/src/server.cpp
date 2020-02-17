/**
 * Copyright 2019 - Gerardo Alvarenga
 * Version 1, 29 August 2019
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 3 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 *
 * @name         server.cpp
 * @description  This program creates the communication channel for the client to connect and
 *               handles the messages that the client application sends.
 *               Sends a response back within a structure.
 * @author       Gerardo Enrique Alvarenga
 * @version      1.0
 *
 */

#include <iostream>
#include <process.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <string.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>
#include "./include/calc_message.h"


int main( int argc, char **argv )
{
    int rcvid;         /* Key to identify what client reply to */
    int chid;          /* Holds the channel ID */
    char message[200]; /* Message object to receive and send data to client */

    client_send_t* client_message;
    server_response_t response_message;

						/* Phase 1 */

    /* Create a channel for the client to connect to */
    chid = ChannelCreate(0);
    if (chid == -1)
    {
        perror("failed to create the channel.");
        exit(EXIT_FAILURE);
    }

    printf("Server PID is %d\n", getpid());

						/* Phase 2 */

    /* Put server in an endless listening state */
    while (1)
    {
    	/*
    	 * Retrieve the message
    	 * @params
    	 * int chid
    	 * void* rmsg - Pointer to received message object
    	 * size_t rbytes
    	 * struct _msg_info *info
    	 */
    	rcvid = MsgReceive(chid, (void*) message, sizeof(message), NULL);

        if (rcvid == -1)
        {
        	printf("Could not parse message");
        }

        client_message = (client_send_t*) message;

        std::string received = std::to_string(client_message->left_hand) + " " + client_message->operation + " " + std::to_string(client_message->right_hand) + " ";

        double solution = 0.0;
        std::string content = " ";
        switch(client_message->operation){
        	case '+':
        	    if (client_message->left_hand + client_message->right_hand < client_message->left_hand ) {
        	        /* handle overflow */
        	    	// TODO: Very rough overflow check, update this section and add to multiplication case
            		std::string code = "OVERFLOW: ";
            		content.append(code);
            		content.append(received);
        			response_message.answer = 0.0;
        			response_message.statusCode = SRVR_OVERFLOW;
        			strcpy(response_message.errorMsg , content.c_str());
        	    	break;
        	    } else {
            		solution = client_message->left_hand + client_message->right_hand;
    				content.append(received);
    				content.append(std::to_string(solution));
    				response_message.answer = solution;
    				response_message.statusCode = SRVR_OK;
    				break;
        	    }

        	case '-':
        		solution = client_message->left_hand - client_message->right_hand;
				content.append(received);
				content.append(std::to_string(solution));
				response_message.answer = solution;
				response_message.statusCode = SRVR_OK;
				break;
        	case 'x':
        		solution = (double) client_message->left_hand * client_message->right_hand;
				content.append(received);
				content.append(std::to_string(solution));
				response_message.answer = solution;
				response_message.statusCode = SRVR_OK;
				break;
        	case '/':
        		if (client_message->right_hand == 0){
        			content.append("UNDEFINED: ");
        			content.append(received);
        			response_message.answer = 0.0;
        			response_message.statusCode = SRVR_UNDEFINED;
        			strcpy(response_message.errorMsg , content.c_str());

        		} else{
        			solution = (double) client_message->left_hand / client_message->right_hand;
    				content.append(received);
    				content.append(std::to_string(solution));
    				response_message.answer = (double) solution;
    				response_message.statusCode = SRVR_OK;
        		}
				break;
        	default:
        		std::string code = "INVALID OPERATOR: ";
        		std::string server_error = code + client_message->operation;
        		content.append(server_error);
    			response_message.answer = 0.0;
    			response_message.statusCode = SRVR_INVALID_OPERATOR;
    			strcpy(response_message.errorMsg , content.c_str());
        }

        /*
         * rcvid  - The receive ID that MsgReceive*() returned when you received the message.
         * status - The status to use when unblocking the MsgSend*() call in the rcvid thread.
         * msg    - A pointer to a buffer that contains the message that you want to reply with.
         * size   - The size of the message, in bytes.
         */
        MsgReply(rcvid, EOK, (void*) &response_message, sizeof(server_response_t));
    }

								/* Phase 3 */

    /* Destroy the channel when done */
    ChannelDestroy(chid);
    return EXIT_SUCCESS;
}
