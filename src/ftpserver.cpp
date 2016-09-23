/*
 * ftpserver.cpp
 *
 *  Created on: Sep 9, 2016
 *      Author: limin
 */

#include "ftpserver.h"

ftp_server::ftp_server() {
	/*
	 * Description : This is the Constructor of the class ftp_server.
	 * Version     : 2016-09-09 V2.0
	 * Author      : liminkmf@mail.ustc.edu.cn
	 * Copyright (c) limin AT ISPC-USTC
	 * */
	pthread_mutex_init(&m_mutex_login, NULL);
	FD_ZERO(&m_fdReads);
	m_client_list.clear();
	m_pidAccept = 0;
	m_pidManage = 0;
	m_server_socket = 0;

}

ftp_server::~ftp_server() {
	/*
	 * Description : This is the Destructor of the class ftp_server.
	 * Version     : 2016-09-09 V2.0
	 * Author      : liminkmf@mail.ustc.edu.cn
	 * Copyright (c) limin AT ISPC-USTC
	 * */
	// Destructor
	FD_ZERO(&m_fdReads);
	m_client_list.clear();
	pthread_mutex_destroy(&m_mutex_login);
}

bool ftp_server::Initialize(unsigned int nPort)
{
	/*
	 * Function Name: Initialize
	 * Author		: liminkmf@mail.ustc.edu.cn
	 * Version		: 2016-08-31 V1.1
	 * Usage		:
	 * Description	: This is the Initialize function for TCP Server
	 * Copyright (c) limin AT ISPC-USTC
	 * */


	/**************************************************/
	// The Begin of Start the Server!
	// Create a Server Socket
	m_server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(m_server_socket == -1)
	{
		cout << "Create Socket Error!" << endl;
		return false;
	}
	// bind the port and IP address
	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(struct sockaddr) );
	server_addr.sin_family = AF_INET; // IPv4
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(nPort);
	// Set the Address Port Reuse
	int reuse = 1;
	if((setsockopt(m_server_socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse))) < 0)
	{
		perror("SetSocketOpet Error:");
		return false;
	}
	// Bind the port
	int res = bind(m_server_socket, (struct sockaddr*)(&server_addr), sizeof(struct sockaddr));
	if( res == -1)
	{
		perror("Failed to Bind!");
		return false;
	}
	//Listen the Port
	res = listen(m_server_socket, MAX_LISTEN);
	if( res == -1 )
	{
		perror("Listen Error:");
		return false;
	}

	// The End of Start the Server!
	/**************************************************/

	// Create thread to accept connect requirements act as the login manager
	if( pthread_create(&m_pidAccept, NULL, AcceptThread, this) != 0)
	{
		perror("Create Accept Thread Error!");
		return false;
	}
	// Create thread to Manage the Data Transfer
	if( pthread_create(&m_pidManage, NULL, ManageThread, this) != 0)
	{
		perror("Create Manage Thread Error!");
		return false;
	}

	PrintServerSocketInfo(server_addr);
	return true;
}

void * ftp_server::AcceptThread(void *pParam)
{
	/*
	 * Function Name: Initialize
	 * Author		: liminkmf@mail.ustc.edu.cn
	 * Version		: 2016-09-11 V2.0
	 * Usage		:
	 * Description	: This is the Accept request handlers
	 * Copyright (c) limin AT ISPC-USTC
	 * */
	if( pParam == NULL )
	{
		cout << "AcceptThread: The Parameter is NULL!" << endl;
		return 0;
	}
	ftp_server *pThis = (ftp_server*)pParam;
	int nMAX_fd = 0;

	while( true )
	{
		FD_ZERO(&pThis->m_fdReads);
		FD_SET(pThis->m_server_socket, &pThis->m_fdReads);
		nMAX_fd = nMAX_fd>pThis->m_server_socket ? nMAX_fd:pThis->m_server_socket;

		auto iter = pThis->m_client_list.begin();
		for(iter=pThis->m_client_list.begin(); iter!=pThis->m_client_list.end(); iter++)
		{
			FD_SET( (*iter).socketfd, &pThis->m_fdReads);
		}
		if(!pThis->m_client_list.empty())
		{
			--iter;
			nMAX_fd = nMAX_fd > (*iter).socketfd ? nMAX_fd : (*iter).socketfd;
		}

		// Use the selec() to monitor all the socket
		int res = select(nMAX_fd+1, &pThis->m_fdReads, 0, 0, NULL);
		if( res == -1 )
		{
			perror("Select Error!");
			continue;
		}

		// whether the server socket is readable
		if( FD_ISSET(pThis->m_server_socket, &pThis->m_fdReads) )
		{
			unsigned int sin_size = sizeof(struct sockaddr_in);
			clientinfo_t newClient;
			int newClient_fd;
			sockaddr_in newClient_addr;
			// Accept the new connection and set the basic new connection information: socket, addr
			newClient_fd = accept(pThis->m_server_socket, (struct sockaddr*)(&newClient_addr), &sin_size);
			if(newClient_fd == -1)
			{
					perror("AcceptThread--Accept Error:");
					continue;
			}
			// Fill the newFClient information
			newClient.peerinfo = newClient_addr;
			newClient.socketfd = newClient_fd;

			loginmanager(newClient, pThis);

		}// The End of if <whether the server socket is readable>
	} // The Ends of the while(true)
}

void ftp_server::loginmanager(clientinfo_t client, void *pParam)
{

	if( pParam == NULL )
	{
		cout << "loginmanager: The Parameter is NULL!" << endl;
		return ;
	}
	ftp_server *pThis = (ftp_server*)pParam;
	// Recv the first data
	char buf[256] = {0};
	int res = recv(client.socketfd, buf, 256, 0);
	if( res>0 )
	{
		// check whether it is a cookies or a username
		cout << "Test Point--getclassinfo: recv the cookies or username : " << buf << endl;
		for ( auto iter = pThis->m_client_list.begin(); iter!=pThis->m_client_list.end(); iter++ )
		{
			if( strcmp(buf, (*iter).cookies)==0 )
			{   // The Start of reconnection handle
				cout << "loginmanager: Accept a reconnection request!" << endl;
				// update the client information
				(*iter).socketfd = client.socketfd;
				(*iter).peerinfo = client.peerinfo;
				(*iter).active   = true;
				(*iter).fileindex++;
				videoname((*iter).filename, (*iter).classid, (*iter).fileindex);
				// The end of update the client information
				char const *userchecked   = "yes";
				write( (*iter).socketfd, userchecked, strlen(userchecked));
				// void PrintClientInfo( *(iter) ); // I don't know why it cause error (>_<)

				cout << "Client Reconnected:"
					 << "\n\t IPaddr    : " << inet_ntoa((*iter).peerinfo.sin_addr) << ":" << (*iter).peerinfo.sin_port
					 << "\n\t Socket_ID : " << (*iter).socketfd
					 << "\n\t FileName  : " << (*iter).filename
					 << "\n\t Username  : " << (*iter).username
					 << "\n\t ClassID   : " << (*iter).classid
					 << "\n\t Class Tile: " << client.classtitle
					 << "\n\t Cookies   : " << (*iter).cookies
					 << "\n\t State     : " << (*iter).active
					 << endl;

				return ;
			} // The end of reconnection handle
		}
		int logres = logincheck(client, buf);
		if( logres==0 )
		{
			// Send the Classinfo to the client
			cout << "Test Point: User check passed! " << endl;
			cout << "\t*** The Classes Details ***" << endl;
			for(unsigned int i=0; i<client.classnums; i++)
			{
				cout << "\t"<< client.classinfo[i] << endl;
				write( client.socketfd, client.classinfo[i], strlen(client.classinfo[i]));
				sleep(1);
			}
			char const *Flagends = "ends";
			sleep(1);
			write( client.socketfd, Flagends, strlen(Flagends));

			// wait for the client send the class info
			int infores = getclassinfo(client);
			if ( infores==0 )
			{
				// Reset the client class info
				// Get the max fileindex and set the index
				client.fileindex = mysql_get_fileindex(mysql, client.classid);
				//
				videoname(client.filename, client.classid, client.fileindex);
				cookies_gen(client);
				sleep(1);
				// Send cookies to the client
				write(client.socketfd, client.cookies, strlen(client.cookies));
				client.active = true;
				// the end of reset the class info
				// Add the new Client to the set: m_client_list
				pThis->m_client_list.insert(client);
				PrintClientInfo(client);
				return ;
			}
			else
			{
				cout << "loginmanager--Illegal Classinfo!" << endl;
				return ;
			}
		}
	}
	else if( res<0 )
	{
		perror("loginmanager: Client Connection Error: ");
		client.active = false;
		return ;
	}
	else
	{
		perror("loginmanager: Client Exit: ");
		client.active = false;
		return ;
	}
}


void * ftp_server::ManageThread(void *pParam)
{
	/*
		 * Function Name: ManageThread
		 * Author		: limnkmf@mail.ustc.edu.cn
		 * Version		: 2016-09-10 V2.0
		 * Usage		:
		 * Description	:
		 *
		 * */
		if( pParam == NULL )
		{
			cout << "ManageThread: The Parameter is NULL!" << endl;
			return 0;
		}


		ftp_server *pThis = (ftp_server*)pParam;

		while( true )
		{
			for( auto iter=pThis->m_client_list.begin(); iter!=pThis->m_client_list.end(); iter++ )
			{
				if( (*iter).socketfd!=-1 && FD_ISSET( (*iter).socketfd, &pThis->m_fdReads ) && (*iter).active )
				{
					cout << "Test Point--ManageThread : Socketfd --  " << (*iter).socketfd << endl;
					cout << "Test Point--ManageThread : Data Arrive!" << endl;
					// Save the file

					cout << "Filename: " << (*iter).filename << endl;
					FILE *fp = fopen((*iter).filename, "w");
					if(fp == NULL)
					{
						perror("Failed to open file for writing!");
						exit(-1);
					}
					char buffer[MAX_DATA_LEN] = {0};
					unsigned int res = 0;
					do
					{
						res = recv( (*iter).socketfd, buffer, sizeof(buffer), 0 );
						if( fwrite(buffer, sizeof(char), res, fp) < res)
						{
							perror("File Write Error: ");
							break;
						}
						if( strcmp(buffer,"fileends")==0 )
						{
							cout << "Video Transfer Ends!" <<endl;
							break;
						}
						bzero(buffer, MAX_DATA_LEN);
					}while(res>0);
					cout << "Test Point: file closed!" << endl;
					fclose(fp);
					// Insert the file record to the mysql table--courses_video
					char introduce[256] = {0};
					strcpy(introduce, "This is the video of class--");
					strcat(introduce, (*iter).classtitle);
					strcat(introduce, ", the video index is: ");
					char file_index[5] = {0};
					sprintf(file_index, "%d", (*iter).fileindex);
					strcat(introduce, file_index);
					strcat(introduce, ".");
					time_t utc_time;
					tm *local_time = NULL;
					char str_time[100];
					utc_time = time(NULL);
					local_time = localtime(&utc_time);
					strftime(str_time, sizeof(str_time), "%Y#%m#%d %H+%M+%S", local_time);
					char title[5] = {0};
					sprintf(title,"%d", (*iter).fileindex);
					int res_insert = mysql_insert_video_record(mysql, (*iter).fileindex, introduce,str_time, title, (*iter).filename, (*iter).classid);
					if(res_insert==-1)
					{
						cout << "ManageThread: Failed to insert the record to the mysql!"<< endl;
					}

					if( res==0 )
					{
						// Client exit
						cout << "ManageThread -- the client exit:" << inet_ntoa( (*iter).peerinfo.sin_addr ) << ":" << (*iter).peerinfo.sin_port  << endl;
						(*iter).active = false;
						break;
					}
					else
					{
						// Error
						cout << "User Defined Error: Read Data ERROR(AcceptThread)" << endl;
					}
				}
			}
		} // The Ends of the main loop while(true)
}

int ftp_server::Start(unsigned int nPort)
{
	if( Initialize(nPort) )
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

