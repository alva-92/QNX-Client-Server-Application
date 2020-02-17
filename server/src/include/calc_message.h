/*
 * calc_message.h
 *
 *  Created on: Sep 26, 2018
 *      Author: hurdleg
 *
 * Edit: Changed "operator" to "operation" as "operator" is a
 * keyword in C++
 */

#ifndef CALC_MESSAGE_H_
#define CALC_MESSAGE_H_

struct client_send
{
	int left_hand;
	char operation;
	int right_hand;
} typedef client_send_t;

#define SRVR_OK 0
#define SRVR_UNDEFINED 1
#define SRVR_INVALID_OPERATOR 2
#define SRVR_OVERFLOW 3

struct server_response
{
	double answer;
	int statusCode; // [OK, UNDEFINED, INVALID_OPERATOR, OVERFLOW]
	char errorMsg[128];
} typedef server_response_t;

#endif /* CALC_MESSAGE_H_ */
