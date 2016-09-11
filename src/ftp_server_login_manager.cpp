/*
 * ftp_server_login_manager.cpp
 *
 *  Created on: Sep 10, 2016
 *      Author: limin
 */

#include "ftp_server_login_manager.h"

void videoname(char*filename, char *classnum, int index)
{
	/*
	 * Function Name: videoname
	 * Author		: liminkmf@mail.ustc.edu.cn
	 * Version		: 2016-09-09 V2.0
	 * Usage		:
	 * Description	: This is the function used to generate the video filename
	 * Copyright (c) limin AT ISPC-USTC
	 * */
	bzero(filename, FILENAME_LEN);
	char const *prefix = "/media/digitalClass/medias/";
	char const *suffix = ".avi";
	strcat(filename,prefix);
	strcat(filename,classnum);
	mkdir(filename, 0755);
	strcat(filename, "/videos/");
	mkdir(filename, 0755);
	char index_str[5];
	sprintf(index_str, "%d", index);
	strcat(filename, index_str);
	strcat(filename,suffix);

	cout << "Test Point: The end of videoname(): " << filename << endl;
} // The Ends of the function videoname()

int logincheck(clientinfo_t &client)
{
	/*
	 * Function Name: logincheck
	 * Author		: liminkmf@mail.ustc.edu.cn
	 * Version		: 2016-09-09 V1.2
	 * Usage		:
	 * Description	: This is the function used to login check for a new connection.
	 * Copyright (c) limin AT ISPC-USTC
	 * */
	cout << "Test Point: logincheck()" << endl;
	char username[256] = {0};
	char passwd[256] = {0};

	// Response datas
	char const *userchecked   = "yes";
	char const *userunchecked = "no";
	char const *logged        = "logged";
	char const *unlogged      = "unlogged";

	int res = -1;

	while( true )
	{
		res = recv( client.socketfd, username, sizeof(username), 0 );
		if( res>0 )
		{
			if( mysql_username_check(username, mysql, client)==0 )
			{
				cout << "Username Check Passed: " << username << endl;
				write( client.socketfd, userchecked, strlen(userchecked));
				break;
			}
			else
			{
				write( client.socketfd, userunchecked, strlen(userunchecked));
				client.active = false;
				return -1;
			}
		}
		else if(res<0)
		{
			perror("New Client Login Error: ");
			client.active = false;
			return -2;
		}
		else
		{
			cout << "New Client abandons to login while checking username!" << endl;
			client.active = false;
			return -1;
		}
	}
	while( true )
	{
		res = recv( client.socketfd, passwd, sizeof(passwd), 0 );
		if( res>0 )
		{
			if( mysql_passwd_check(passwd, mysql, client )==0 )
			{
				write( client.socketfd, logged, strlen(logged));
				strncpy(client.username, username, strlen(username));
				client.active = true;
				return 0;
			}
			else
			{
				cout << "Passwd check Fialed: " << passwd << endl;
				write( client.socketfd, unlogged, strlen(unlogged));
				client.active = false;
				return -1;
			}
		}
		else if(res<0)
		{
			perror("New Client Login Error: ");
			client.active = false;
			return -2;
		}
		else
		{
			cout << "New Client abandons to login while checking password!!" << endl;
			client.active = false;
			return -1;
		}
	}
} // The Ends of the function logincheck();

int logincheck(clientinfo_t &client, const char*username)
{
	/*
	 * Function Name: logincheck
	 * Author		: liminkmf@mail.ustc.edu.cn
	 * Version		: 2016-09-10 V2.0
	 * Usage		:
	 * Description	: This is the function used to login check for a new connection.
	 * Copyright (c) limin AT ISPC-USTC
	 * */
	cout << "Test Point: logincheck()" << endl;
	char passwd[256] = {0};

	// Response datas
	char const *userchecked   = "yes";
	char const *userunchecked = "no";
	char const *logged        = "logged";
	char const *unlogged      = "unlogged";
	if( mysql_username_check(username, mysql, client)==0 )
	{
		cout << "Username Check Passed: " << username << endl;
		write( client.socketfd, userchecked, strlen(userchecked));
	}
	else
	{
		write( client.socketfd, userunchecked, strlen(userunchecked));
		client.active = false;
		return -1;
	}
	while( true )
	{
		int res = recv( client.socketfd, passwd, sizeof(passwd), 0 );
		if( res>0 )
		{
			if( mysql_passwd_check(passwd, mysql, client )==0 )
			{
				write( client.socketfd, logged, strlen(logged));
				strncpy(client.username, username, strlen(username));
				client.active = true;
				return 0;
			}
			else
			{
				cout << "Passwd check Fialed: " << passwd << endl;
				write( client.socketfd, unlogged, strlen(unlogged));
				client.active = false;
				return -1;
			}
		}
		else if(res<0)
		{
			perror("New Client Login Error: ");
			client.active = false;
			return -2;
		}
		else
		{
			cout << "New Client abandons to login while checking password!!" << endl;
			client.active = false;
			return -1;
		}
	}
}

void cookies_gen(clientinfo_t &client)
{
	/*
	 * Function Name: cookies_gen
	 * Author		: liminkmf@mail.ustc.edu.cn
	 * Version		: 2016-09-09 V2.0
	 * Usage		:
	 * Description	: This is the function used to generate the connection cookies.
	 *     Remember: this function should only be used when a new client connection accepted.
	 *     A username can only connect to the server through single client.
	 * Copyright (c) limin AT ISPC-USTC
	 * */
	struct tm* tlocal;
	time_t t = time(NULL);
	tlocal = localtime(&t);
	char year[4],mon[2],day[2],hour[2],min[2],sec[2];
	sprintf(year,"%d",tlocal->tm_year+1900);
	sprintf(mon,"%d",tlocal->tm_mon+1);
	sprintf(day,"%d",tlocal->tm_mday);
	sprintf(hour,"%d", tlocal->tm_hour);
	sprintf(min,"%d",tlocal->tm_min);
	sprintf(sec,"%d",tlocal->tm_sec);

	strcpy(client.cookies, client.username);
	strcat(client.cookies, "$");
	strcat(client.cookies, year);
	strcat(client.cookies, mon);
	strcat(client.cookies, day);
	strcat(client.cookies, hour);
	strcat(client.cookies, min);
	strcat(client.cookies, sec);
	strcat(client.cookies, "==");
} // The Ends of the function cookies_gen

int getclassinfo(clientinfo_t &client)
{
	/*
	 * Function Name: getclassinfo
	 * Author		: liminkmf@mail.ustc.edu.cn
	 * Version		: 2016-09-09 V1.2
	 * Usage		:
	 * Description	: This is the function used to get the class information chosed by user.
	 * Copyright (c) limin AT ISPC-USTC
	 * */
	cout << "Test Point: getclassinfo()" << endl;
	char buf[256] = {0};
	int res = 0;
	while( true )
	{
		res = recv( client.socketfd, buf , 256, 0 );
		if( res>0 )
		{
			cout << "Test Point: getclassinfo()--Recv the class info" << buf << endl;
			for(unsigned int i=0; i< client.classnums; i++)
			{
				if( strcmp(client.classinfo[i] , buf)==0 )
				{
					strncpy(client.classid, client.classids[i], 5);
					client.fileindex = 0;
					cout << buf << " has been set as the class now!" <<endl;
					return 0;
				}
			}
			return -3;
		}
		else if( res==0 )
		{
			cout << "getclassinfo--Client Exited!" << endl;
			return -1;
		}
		else
		{
			perror("getclassinfo--Error:");
			return -2;
		}
	}
}

void PrintClientInfo(clientinfo_t &client)
{
	/*
	 * Function Name: PrintClientInfo
	 * Author		: liminkmf@mail.ustc.edu.cn
	 * Version		: 2016-09-09 V1.2
	 * Usage		:
	 * Description	: This is the function used to get the class information chosed by user.
	 * Copyright (c) limin AT ISPC-USTC
	 * */
	// Print the new client information
	cout << "New Client Connected:"
		 << "\n\t IPaddr    : " << inet_ntoa(client.peerinfo.sin_addr) << ":" << client.peerinfo.sin_port
		 << "\n\t Socket_ID : " << client.socketfd
		 << "\n\t FileName  : " << client.filename
		 << "\n\t Username  : " << client.username
		 << "\n\t ClassID   : " << client.classid
		 << "\n\t Cookies   : " << client.cookies
		 << "\n\t State     : " << client.active
		 << endl;
	// The End of Print new Client information
}
