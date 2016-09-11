/*
 * ftpserver.h
 *
 *  Created on: Sep 9, 2016
 *      Author: limin
 */

#ifndef FTPSERVER_H_
#define FTPSERVER_H_

#include "ftp_server_headers.h"
#include "ftp_server_mysql_op.h"
#include "ftp_server_login_manager.h"
#include "ClientInfo.h"
#include "ftp_server_logs.h"

using namespace std;

class ftp_server {
public:
	ftp_server();
	virtual ~ftp_server();

private:
	int m_server_socket;
	fd_set m_fdReads;

	set<clientinfo_t> m_client_list;

	pthread_mutex_t m_mutex_login;

	pthread_t m_pidAccept;
	pthread_t m_pidManage;

	bool Initialize(unsigned int nPort);

private:
	static void *AcceptThread(void *pParam);
	static void *ManageThread(void *pParam);
	static void loginmanager(clientinfo_t client, void*pPram);

public:
	int Start(unsigned int nPort);
	int Stop();
	int Restart();

};

#endif /* FTPSERVER_H_ */
