/*
 * ftp_server_mysql_op.h
 *
 *  Created on: Sep 9, 2016
 *      Author: limin
 */

#ifndef FTP_SERVER_MYSQL_OP_H_
#define FTP_SERVER_MYSQL_OP_H_

#include <mysql.h>
#include "ftp_server_headers.h"
#include "ClientInfo.h"

using namespace std;

// Print the SQL information
void PrintMySqlInfo(MYSQL &mysql);

// Query
MYSQL_RES* query(const char* request, MYSQL &mysql);

int mysql_insert_video_record(MYSQL &mysql, const char*introduce, const char*upload_time, const char*title, const char*video_path, const char*course_id);

int mysql_username_check(const char* username, MYSQL &mysql,  clientinfo_t &client);
int mysql_passwd_check(const char* passwd, MYSQL mysql, clientinfo_t &client);



#endif /* FTP_SERVER_MYSQL_OP_H_ */
