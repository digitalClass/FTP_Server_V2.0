/*
 * ftp_server_mysql_op.cpp
 *
 *  Created on: Sep 10, 2016
 *      Author: limin
 */


#include "ftp_server_mysql_op.h"

void PrintMySqlInfo(MYSQL &mysql)
{
	/*
	 * Function Name: PrintMySqlInfo
	 * Author		: liminkmf@mail.ustc.edu.cn
	 * Version		: 2016-09-02 V1.1
	 * Usage		:
	 * Description	: This is the function used to print the basic info
	 *     of the mysql database.
	 * Copyright (c) limin AT ISPC-USTC
	 * */
	const char* stat = mysql_stat(&mysql);
	const char* server_info = mysql_get_server_info(&mysql);
	const char* client_info = mysql_get_client_info();
	const char* host_info   = mysql_get_host_info(&mysql);
	unsigned long clientversion = mysql_get_client_version();
	unsigned long serverversion = mysql_get_server_version(&mysql);
	unsigned int  protoinfo = mysql_get_proto_info(&mysql);
	const char* charactername = mysql_character_set_name(&mysql);

	cout << "The SQL Details:"
	     << "\n\t State        : " << stat
		 << "\n\t ServerInfo   : " << server_info
		 << "\n\t ClientInfo   : " << client_info
		 << "\n\t Host Info    : " << host_info
		 << "\n\t ClientVersion: " << clientversion
		 << "\n\t ServerVersion: " << serverversion
		 << "\n\t ProtoInfo    : " << protoinfo
		 << "\n\n Client Character Set: " << charactername
		 << endl;

	if( !mysql_set_character_set(&mysql, "utf8") )
	{
		cout << "New client character set is : " << mysql_character_set_name(&mysql) << endl;
	}
} // The Ends of the function PrintMySqlInfo

MYSQL_RES* query(const char* request, MYSQL &mysql)
{
	/*
	 * Function Name: query
	 * Author		: liminkmf@mail.ustc.edu.cn
	 * Version		: 2016-09-02 V1.1
	 * Usage		:
	 * Description	: This is the function used to query from a
	 *     mysql database.
	 * Return code   :
	 *     0 : username verification pass!
	 *     1 : username doesn't exist
	 *     2 : username is not a teacher
	 *     -1: failed connect to the database
	 * Copyright (c) limin AT ISPC-USTC
	 * */
	int res_query = mysql_real_query(&mysql, request, (unsigned int)strlen(request) );
	if( res_query )
	{
		cout << "Query Error: " << mysql_errno(&mysql) << " from " << mysql_error(&mysql) << endl;
		return NULL;
	}
	return mysql_store_result(&mysql);
} // The Ends of the Fucntion query()

int mysql_username_check(const char* username, MYSQL &mysql,  clientinfo_t &client)
{
	/*
	 * Function Name: mysql_username_check
	 * Author		: liminkmf@mail.ustc.edu.cn
	 * Version		: 2016-09-02 V1.1
	 * Usage		:
	 * Description	: This is the function used to check whether a username is a teacher.
	 *
	 * Copyright (c) limin AT ISPC-USTC
	 * */
	// Initialize the MYSQL connection
	mysql_init(&mysql);
	// Connect to the MySql
	if( !mysql_real_connect(&mysql, "localhost", "root", "1234", "DCdb", 0, NULL, 0) )
	{
		cout << "mysql_username_check() : Username Verify Failed -- Failed connect to MySql:" << mysql_errno(&mysql) << endl;
		return -1;
	}

	cout << "Test Point--mysql_username_check() : Succeed connect to MySql!" << endl;
	PrintMySqlInfo(mysql);

	char request[256] = "select user_role from users_user where username='";
	strcat(request, username);
	strcat(request, "'");
	cout << "Test Point--mysql_username_check : " << request << endl;

	MYSQL_RES *res_user_role = query(request, mysql);

	if( mysql_num_rows(res_user_role)==1 )
	{
		//cout << "The User is Unique!" << endl;
		MYSQL_ROW res_data = mysql_fetch_row(res_user_role);
		cout << "Test Point--mysql_username_check: user_role--" << res_data[0] << endl;
		if( strcmp(res_data[0], "te")==0 )
		{
			mysql_free_result(res_user_role);

			// Query the mysql to get the teacher's user ID
			char request_user_id[256] = "select id from users_user where username='";
			strcat(request_user_id, username);
			strcat(request_user_id, "'");
			cout << "Test Point--mysql_username_check: " << request_user_id << endl;
			MYSQL_RES *res_id = query(request_user_id, mysql);
			MYSQL_ROW res_data_user_id = mysql_fetch_row(res_id);
			cout << "Query res: " << res_data_user_id[0] << endl;
			// Query the mysql to get the courses ids
			char request_course_id[256] = "select course_id from courses_course_teacher where user_id='";
			strcat(request_course_id, res_data_user_id[0]);
			strcat(request_course_id, "'");
			mysql_free_result(res_id);
			cout << "Test Point--mysql_username_check: " << request_course_id << endl;
			MYSQL_RES *res_course_ids = query(request_course_id, mysql);
			MYSQL_ROW res_data_course_id ;//= mysql_fetch_row(res_course_ids);
			//cout << "Query res: " << res_data_course_id[0] << endl;

			// Set the Class infomation : Total class numbers
			client.classnums = mysql_num_rows(res_course_ids);
			cout << "Total Class Number: " << client.classnums << endl;
			unsigned int i = 0;
			while( res_data_course_id = mysql_fetch_row(res_course_ids) )
			{
				// Query the mysql to get the courses title
				char request_course_title[256] = "select title from courses_course where id='";
				strcat(request_course_title, res_data_course_id[0]);
				strcat(request_course_title, "'");
				strcpy(client.classinfo[i], "[");
				strcat(client.classinfo[i], res_data_course_id[0]);
				strcat(client.classinfo[i], "]--");
				cout << "Test Point--mysql_username_check: " << request_course_title << endl;
				MYSQL_RES *course_tile = query(request_course_title, mysql);
				MYSQL_ROW res_data_class_title = mysql_fetch_row(course_tile);
				cout << "Query res: " << res_data_class_title[0] << endl;
				strcat(client.classinfo[i], res_data_class_title[0]);
				strcpy(client.classids[i], res_data_course_id[0]);
				bzero(request_course_title,256);
				i++;
			}
			// Here Should add class title verification function to ensure the class title is legal.
			client.classnums = (client.classnums<i) ? client.classnums : i;
			return 0;
		}
		else
		{
			cout << "mysql_username_check() : Username Verify Failed -- username is not a teacher!" << endl;
			return 2;
		}
	}
	else
	{
		cout << "mysql_username_check() : Username Verify Failed -- username doesn't exist!" << endl;
		return 1;
	}
}


int mysql_passwd_check(const char* passwd, MYSQL mysql, clientinfo_t &client)
{
	/*
	 * Function Name: mysql_passwd_check
	 * Author		: liminkmf@mail.ustc.edu.cn
	 * Version		: 2016-09-02 V1.1
	 * Usage		:
	 * Description	: This is the function used to check whether the password is correct.
	 *
	 * Copyright (c) limin AT ISPC-USTC
	 * */
	// Because of the technology reason we ignore the password verification
	// However the API should be designed

	return 0;
}


int mysql_insert_video_record(MYSQL &mysql, const char*introduce, const char*upload_time, const char*title, const char*video_path, const char*course_id)
{
	/*
	 * Function Name: mysql_insert_video_record
	 * Author		: liminkmf@mail.ustc.edu.cn
	 * Version		: 2016-09-11 V2.0
	 * Usage		:
	 * Description	: This is the function used to add video path record to mysql table courses_video
	 *     mysql database.
	 * Return code   :
	 *     0 : username verification pass!
	 *     1 : username doesn't exist
	 *     2 : username is not a teacher
	 *     -1: failed connect to the database
	 * Copyright (c) limin AT ISPC-USTC
	 * */
	char request[256] = {0};
	strcpy(request, "insert into courses_video (introduce, upload_time, title, video_path, course_id) values ('");
	strcat(request, introduce);
	strcat(request, "', '");
	strcat(request, upload_time);
	strcat(request, "', '");
	strcat(request, title);
	strcat(request, "', '");
	strcat(request, video_path);
	strcat(request, "',");
	strcat(request, course_id);
	strcat(request, " );");
	cout << "mysql_insert_video_record : " << request << endl;

	int res_add = mysql_real_query(&mysql, request, strlen(request));
	if( res_add )
	{
		cout << "mysql_insert_video_record Error: " << mysql_errno(&mysql) << " from " << mysql_error(&mysql) << endl;
		return -1;
	}

	return 0;
}

