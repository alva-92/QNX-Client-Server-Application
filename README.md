# QNX-Client-Server-Application
Simple QNX applicaton that demonstrates how to pass data structures in messages between a client and a server program

# Overview
Write a client that sends a message containing two integers and a character representing the operation
to a server. The server computes the numeric value for the result of the numbers and the given
operation, and then returns a message containing a structure as the result. The client will pass the two
numbers as integer members of an “input parameters” struct and the operation as a character value in
the struct. The server will reply with a different struct containing a double, a flag representing whether
or not an error occurred (or would have occurred if the operation had been attempted), and a character
string of up to 128 bytes containing the error description.

# Usage

## Running the server

Start the server in the background (use the ‘&’ symbol). 
Once the server has started, clients will be able to connect and submit operations to the server.

```bash
./server &
```

## Running the client

Run the client:

```bash
#./client <Server-PID> <left-operand> <operator> <right-operand>

```

The operation should be one of: ‘+’, ‘-‘, ‘x’, and ‘/’
(Note: that is an ‘x’ not a ‘*’).
The client program will block until receiving the result message from the server program and then the
client program will output the result (or error condition) to the console.
