/*
 * ftp_server_login_manager.h
 *
 *  Created on: Sep 10, 2016
 *      Author: limin
 */

#ifndef FTP_SERVER_LOGIN_MANAGER_H_
#define FTP_SERVER_LOGIN_MANAGER_H_

#include "ftp_server_headers.h"
#include "ftp_server_mysql_op.h"
#include "ftpserver.h"

using namespace std;

extern MYSQL mysql;

/************************************************/

void videoname(char*filename, char *classnum, int index);
void cookies_gen(clientinfo_t &client);

void SaveData(const char*buf, size_t nlen, char* filename);

int logincheck(clientinfo_t &client);
int logincheck(clientinfo_t &client, const char*username);

int getclassinfo(clientinfo_t &client);

void PrintClientInfo(clientinfo_t &client);


#endif /* FTP_SERVER_LOGIN_MANAGER_H_ */
