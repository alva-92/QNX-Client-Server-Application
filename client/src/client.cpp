#include <iostream>
#include <process.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <string.h>

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/iofunc.h>
#include <sys/dispatch.h>

#include "../../server/src/include/calc_message.h"

/* User message constants */
#define NORM_SUM_CASE_MSG " (normal case +) "
#define NORM_SUB_CASE_MSG " (normal case -) "
#define NORM_DIV_CASE_MSG " (normal case /) "
#define NORM_MUL_CASE_MSG " (normal case x) "
#define ERR_DIV_BY_ZERO   " SRVR_UNDEFINED (handle divide by 0) "
#define ERR_INV_OPERATOR  " SRVR INVALID OPERATOR (handle unsupported operator) "
#define ERR_UNDEFINED     " Undefined behavior - Unsupported case "
#define ERR_USAGE_1       " Usage message of calc_client\n "
#define ERR_USAGE_2       " error. as calc_client can't connect attached to processID "
#define USAGE_INST        " Usage ./calc_client <Calc-Server-PID> left_operand operator right_operand"
#define ERR_OVERFLOW      " SRVR OVERFLOW (handle overflow) "


/**
 * This function formats the 'Expected Result' from the server and displays it to the
 * client
 *
 * @params:
 * phase - The communication where the error occurred
 * missing_params - Flag to indicate whether any paramaters were missing suchs as the PID
 * operation - The type of operation to run on the numbers provided
 * op_1 - left hand operand
 * op_2 - Right hand operand
 * extras - any other information that is needed to be displayed to the user
 */
void expected_result(int phase, int missing_params=0,
					char operation='N', int op_1=0, int op_2=0,
					int extras=0)
{
    double expected_solution = 0.0;
    std::string content      = "Expected result: ";

	if(phase == 1)
	{
		if (missing_params == 1){ /* Missing arguments */
			content.append(ERR_USAGE_1);
		}
	}
	else if (phase == 2)
	{
		if(missing_params == 3){
			content.append(ERR_USAGE_2);
			content.append(std::to_string(extras));
			content.append("\nMsgSend had an error");
		}else{
		    /* Display expected result */
			switch(operation){
				case '+':
					if (op_1 + op_2 < op_1 ){
						content.append(ERR_OVERFLOW);
					}else{
						expected_solution = op_1 + op_2;
						content.append(std::to_string(expected_solution));
						content.append(NORM_SUM_CASE_MSG);
					}
					break;
				case '-':
					expected_solution = op_1 - op_2;
					content.append(std::to_string(expected_solution));
					content.append(NORM_SUB_CASE_MSG);
					break;
				case 'x':
					expected_solution = op_1 * op_2;
					content.append(std::to_string(expected_solution));
					content.append(NORM_MUL_CASE_MSG);
					break;
				case '/':
					if (op_2 == 0){
						content.append(ERR_DIV_BY_ZERO);
					} else{
						expected_solution = (double) op_1 / op_2;
						content.append(std::to_string(expected_solution));
						content.append(NORM_DIV_CASE_MSG);
					}
					break;
				default:
					content.append(ERR_INV_OPERATOR);
			}
		}

	}

    std::cout << content << std::endl;
}

int main(int argc, char* argv[])
{
	/* Validate the number of command line arguments as per requirements */
	if (argc != 5)
	{
		expected_result(1, 1);
		exit(EXIT_FAILURE);
	}

	client_send_t client_message;    /* Struct to be sent to the server */
	char resp_msg [200]; 		     /* Response message buffer */
	int  coid;
	server_response_t* response_message;

	/* Clear the memory for the message and the response */
	memset( &client_message, 0, sizeof(client_message));
	memset( &resp_msg, 0, sizeof(resp_msg));

	/* Retrieve the values of the command line */
	pid_t serverpid = atoi(argv[1]);

	/* Set up the message data to send to the server */
	client_message.left_hand = atoi(argv[2]);
	client_message.operation =  *argv[3];
	client_message.right_hand = atoi(argv[4]);


								/* Phase 1 */

	/* Establish a connection */

	/*
	 * @params
	 * nd The node descriptor of the node (e.g. ND_LOCAL_NODE for the local node) on which the process that owns the channel is running; see “Node descriptors,” below.
	 * pid The process ID of the owner of the channel. If pid is zero, the calling process is assumed.
	 * chid The channel ID, returned by ChannelCreate(), of the channel to connect to the process.
	 * index The lowest acceptable connection ID.
	 * flags If flags contains _NTO_COF_CLOEXEC, the connection is closed when your process calls an exec
	 */
	coid = ConnectAttach(ND_LOCAL_NODE, serverpid, 1, _NTO_SIDE_CHANNEL, 0);
	if (coid == -1)
	{
		printf("\n Could not attach to channel");
		exit(EXIT_FAILURE);
	}

								/* Phase 2 */

    /* Send the message */
    int size = sizeof(client_send_t);
    char buffer[size]; /* For testing, sending the struct as a buffer to the server */

    memcpy(buffer, &client_message, sizeof(client_send_t));


    /* Send the message */

	/*
     * @params
     * the connection ID of the target server (coid),
     * a pointer to the send message (smsg),
     * the size of the send message (sbytes),
     * a pointer to the reply message (rmsg), and
     * the size of the reply message (rbytes).
     */
	if (MsgSend(coid, &client_message, sizeof(client_message), resp_msg, sizeof(server_response_t)) == -1)
	{
		expected_result(2, 3, 'N', 0, 0, serverpid);
		exit(EXIT_FAILURE);
	}

	expected_result(2, 0, client_message.operation, client_message.left_hand, client_message.right_hand);
	response_message = (server_response_t*) resp_msg;
	switch (response_message->statusCode)
	{
		case SRVR_OK:
			std::cout << "The server has calculated the result of: " <<
			client_message.left_hand << " " << client_message.operation << " " << client_message.right_hand << " as " <<
			response_message->answer <<std::endl;
			break;
		default:
			std::cout << "Error message from server: " <<
			response_message->errorMsg <<std::endl;
	}

								/* Phase 3 */

	/* Disconnect from the channel */
	ConnectDetach(coid);
	return EXIT_SUCCESS;
}
