/*
 * ftp_server_logs.cpp
 *
 *  Created on: Sep 11, 2016
 *      Author: limin
 */

#include "ftp_server_logs.h"

void PrintProgramInfo()
{
	cout << "\n\t*******************************************************"
		 << "\n\t                    Video Upload Server V2.0           "
		 << "\n\t  Welcome to Use the Digital Class. We are a team named"
		 << "\n\ti9 from USTC.                                          "
		 << "\n\t                                                       "
		 << "\n\t                                                       "
		 << "\n\t                                                       "
		 << "\n\t Copyright(c) limin @ ISPC-USTC                        "
		 << "\n\t If any bug happened, please contact                   "
		 << "\n\t       liminkmf@mail.ustc.edu.cn                       "
		 << "\n\t*******************************************************"
		 << endl;
}

void PrintServerSocketInfo(sockaddr_in server_addr)
{
	time_t t;
	time(&t);

	cout << "The Video Upload Server Details: "
		 << "\n\t Server IP   : " << inet_ntoa(server_addr.sin_addr)
		 << "\n\t Listen Port : " << server_addr.sin_port
		 << "\n\t Process ID  : " << getpid()
		 << "\n\t Start Time  : " << ctime(&t)
		 << "\n"
		 << endl;
}
