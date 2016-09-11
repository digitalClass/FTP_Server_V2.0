/*
 * ClientInfo.h
 *
 *  Created on: Sep 9, 2016
 *      Author: limin
 */

#ifndef CLIENTINFO_H_
#define CLIENTINFO_H_

#include "ftp_server_headers.h"

/************************************************/

const int MAX_LISTEN = 1023;
const int SERVER_LISTEN_PORT = 6410;
const int MAX_DATA_LEN = 4096;
const int FILENAME_LEN = 256;
const int USERNAME_LEN = 128;
const int MAX_CLASS_NUM= 15;
const int CLASS_NUM_LEN = 128;
const int MAX_ID_LEN = 10;
const int LEN_COOKIES = 128;

typedef struct clientinfo_t
{
	mutable int socketfd;			// Socket ID

	// Connection informations
	mutable sockaddr_in peerinfo;	// The peer connection information
	char cookies[LEN_COOKIES];
	mutable bool active;

	// class informations
	char username[USERNAME_LEN] = {0};
	char classinfo[MAX_CLASS_NUM][CLASS_NUM_LEN];
	char classids[MAX_CLASS_NUM][MAX_ID_LEN];
	unsigned int classnums = 0;

	mutable char classid[MAX_ID_LEN] = {0};
	mutable unsigned int fileindex = 0;
	mutable char classtitle[64] = {0};
	mutable char filename[FILENAME_LEN];

	bool operator < (const struct clientinfo_t &a) const
	{
		return socketfd < a.socketfd;
	}

}ClientInfo;



#endif /* CLIENTINFO_H_ */
