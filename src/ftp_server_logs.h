/*
 * ftp_server_logs.h
 *
 *  Created on: Sep 11, 2016
 *      Author: limin
 */

#ifndef FTP_SERVER_LOGS_H_
#define FTP_SERVER_LOGS_H_


#include "ftpserver.h"

void PrintProgramInfo();
void PrintServerSocketInfo(sockaddr_in server_addr);

#endif /* FTP_SERVER_LOGS_H_ */
