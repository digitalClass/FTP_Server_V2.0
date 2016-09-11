/*
 * main.cpp
 *
 *  Created on: Sep 10, 2016
 *      Author: limin
 */

#include "ftpserver.h"
#include "ftp_server_logs.h"


MYSQL mysql;

int main(int argc, char* argv[])
{
	if(argc != 1)
	{
		cout << "Usage: FTPServerV1.1 " << endl;
		return -1;
	}

	// Logs
	PrintProgramInfo();


	ftp_server test;

	if ( test.Start(6410) == -1)
	{
		cout << "Failed to start the server!" <<endl;
		return -1;
	}

	while( true )
	{
		sleep(5);
	}
}

