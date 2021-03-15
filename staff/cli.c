/*================================================================
*   Copyright (C) 2021 hqyj Ltd. All rights reserved.
*   
*   文件名称：cli.c
*   创 建 者：cx
*   创建日期：2021年03月12日
*   描    述：小菜鸟
*
================================================================*/
#include "func.h"
InfoP SIG;
int fd;
void KILL_PID(int sig){
	SIG.Type = EXIT;
	send(fd, (void*)&SIG, sizeof(SIG), 0);
	close(fd);
	exit(1);
}

int main(int argc, char *argv[])
{
	signal(SIGINT, KILL_PID);
	InfoP Recvmsg;
	InfoP Sendmsg;
	bzero(&SIG, sizeof(SIG));
	bzero(&Recvmsg, sizeof(Recvmsg));
	bzero(&Sendmsg, sizeof(Sendmsg));
	int cfd = socket(AF_INET, SOCK_STREAM, 0);
	if(cfd<0){
		perror("socket");
		return -1;
	}
	fd = cfd;
	struct sockaddr_in Send;
	Send.sin_family = AF_INET;
	Send.sin_port   = htons(PORT);
	Send.sin_addr.s_addr=inet_addr(IP);

	int duankou = 1;
	setsockopt(cfd, SOL_SOCKET, SO_REUSEADDR, &duankou, sizeof(int));

	ret = connect(cfd, (void*)&Send, sizeof(Send));
	if(ret<0){
		perror("connect");
		return -1;
	}
	fprintf(stderr, "服务器连接成功！  行号：%d\n", __LINE__);//调试代码

	while(1){
		system("clear");
		fprintf(stderr, "************************************************\n");
		fprintf(stderr, "      1.注册       2.登录        3.退出         \n");
		fprintf(stderr, "************************************************\n");
		fprintf(stderr, "请输入：");
SCANF:		
		num=0;
		scanf("%d", &num);
		while(getchar()!='\n');
		if(!(num<4 && num>0)){
			fprintf(stderr, "输入有误，请重新输入:\n");
			goto SCANF;
		}
		switch(num){
		case LOGIN:
CliLogin1:
			ret = CliLogin(cfd, Sendmsg, Recvmsg);
			if(ret<0){
				fprintf(stderr, "请输入任意键继续！\n");
				while(getchar()!='\n');
				goto CliLogin1;
			}else if(ret >0){
				fprintf(stderr, "请输入任意键返回上一级！\n");
				while(getchar()!='\n');
				break;
			}
			break;
		case ENTER:
			ret = CliEnter(cfd, Sendmsg, Recvmsg);
			if(ret!=0){
				fprintf(stderr, "请输入任意键继续！\n");
				while(getchar()!='\n');
			}else if(ret == 0){
				fprintf(stderr, "请输入任意键返回上一级！\n");
				while(getchar()!='\n');
			}
			break;
		case EXITOUT:
			Sendmsg.Type = EXIT;
			strcpy(Sendmsg.Id, ID);
			do{
				ret = send(cfd, (void*)&Sendmsg, sizeof(Sendmsg), 0);
			}while(ret < 0 && errno ==   EINTR);
			close(cfd);
			return 0;
		}
	}
	close(cfd);	
	return 0;
}

int CliLogin(int cfd, InfoP Sendmsg, InfoP Recvmsg){
	bzero(&Sendmsg, sizeof(Sendmsg));
	bzero(&Recvmsg, sizeof(Recvmsg));
	while(1){
		system("clear");
		fprintf(stderr, "************************************************\n");
		fprintf(stderr, "           1.注册            2.返回上一级		 \n");
		fprintf(stderr, "************************************************\n");
		fprintf(stderr, "请输入：");
SCANF_1:
		num=0;
		scanf("%d", &num);
		while(getchar()!='\n');
		if(num>3||num<1){
			fprintf(stderr, "输入错误，请重新输入：");
			goto SCANF_1;
		}
		switch(num){
		case LOGIN_1:
			fprintf(stderr, "请输入ID(请输入数字或者字母)：");
			scanf("%s", Sendmsg.Id);
			while(getchar()!='\n');
			fprintf(stderr, "请输入Password(请输入数字或者字母)：");
			scanf("%s", Sendmsg.Password);
			while(getchar()!='\n');
			Sendmsg.Type = ROOT_LOGIN;
			do{
				ret = send(cfd, (void*)&Sendmsg, sizeof(Sendmsg), 0);
			}while(ret<0 && errno == EINTR);
			if(ret<0){
				fprintf(stderr, "网络不佳，请重新注册！\n");
				return -1;//网络不好
			}
			do{
				ret = recv(cfd, (void*)&Recvmsg, sizeof(Recvmsg), 0);
			}while(ret<0 && errno ==EINTR);
			if(Recvmsg.Type == ROOT_EXIT){
				fprintf(stderr, "只能注册一个管理员！\n");
				return -2;
			}else if(Recvmsg.Type == LOGIN_SUCCESS){
				fprintf(stderr, "用户注册成功！\n");
				return 1;
			}
			break;
		case EXITOUT_1:
			return 0;
		}
	}
	return 0;
}

int CliEnter(int cfd, InfoP Sendmsg, InfoP Recvmsg){
	bzero(&Sendmsg, sizeof(Sendmsg));
	bzero(&Recvmsg, sizeof(Recvmsg));
	while(1){
		system("clear");
		fprintf(stderr, "************************************************\n");
		fprintf(stderr, "           1.登录            2.返回上一级       \n");
		fprintf(stderr, "************************************************\n");
		fprintf(stderr, "请输入：");
SCANF_2:
		num = 0;
		scanf("%d", &num);
		while(getchar()!='\n');
		if(num>3||num<1){
			fprintf(stderr, "输入错误，请重新输入：");
			goto SCANF_2;
		}
		switch(num){
		case ENTER_1:
			fprintf(stderr, "请输入ID：");
			scanf("%s", Sendmsg.Id);
			while(getchar()!='\n');
			fprintf(stderr, "请输入Password：");
			scanf("%s", Sendmsg.Password);
			while(getchar()!='\n');
			Sendmsg.Type = ID_LOGIN;
			do{
				ret = send(cfd, (void*)&Sendmsg, sizeof(Sendmsg), 0);
			}while(ret <0 && errno == EINTR);
			if(ret<0)
				perror("send");
			do{
				ret = recv(cfd, (void*)&Recvmsg, sizeof(Recvmsg), 0);
			}while(ret<0 && errno == EINTR);
			if(ret<0)
				perror("recv");
			if(Recvmsg.Type == LOGIN_SUCCESS){
				strcpy(ID, Sendmsg.Id);
				strcpy(SIG.Id, ID);
				if(CliInter(cfd, Sendmsg, Recvmsg) == 10){
					break;
				}
			}else if(Recvmsg.Type == USER_FAIL){
				fprintf(stderr, "用户已登录！\n");
				return -1;
			}else if(Recvmsg.Type == PSWD_FAIL){
				fprintf(stderr, "用户名或密码错误！\n");
				return -2;
			}else if(Recvmsg.Type == USER_UNFIND){
				fprintf(stderr, "用户不存在！\n");
				return -3;
			}
			break;
		case EXITOUT_2:
			return 0;
		}
	}
	return 0;
}

int CliInter(int cfd, InfoP Sendmsg, InfoP Recvmsg){
	int quanxian=0;
	int workid=0;
	if(strcmp(Recvmsg.Res, "0"))
		quanxian=1;
	else
		quanxian=0;
	while(1){
		system("clear");
		fprintf(stderr, "************************************************************\n");
		fprintf(stderr, "1.添加信息  2.修改信息  3.删除信息  4.查看信息  5.退出登录  \n");
		fprintf(stderr, "************************************************************\n");
		fprintf(stderr, "请输入：");
SCANF_3:		
		num=0;
		scanf("%d", &num);
		while(getchar()!='\n');
		if(num>5||num<1){
			fprintf(stderr, "输入有误，请重新输入：");
			goto SCANF_3;
		}
		switch(num){
		case 1:
			if(quanxian==0){
				//普通用户操作
				fprintf(stderr, "此用户无权限！请重新输入：\n");
				goto SCANF_3;
			}else{
				//管理员用户操作增加
				fprintf(stderr, "请输入员工姓名：");
				fgets(Sendmsg.INFO.Name, 20, stdin);
				while(getchar()!='\n');
				fprintf(stderr, "请输入员工工号：");
SCANF_4:
				workid=0;
				scanf("%d", &workid);
				while(getchar()!='\n');
				if(workid>10000 || workid<999){
					fprintf(stderr, "输入不规范，请重新输入：");
					goto SCANF_4;
				}
				sprintf(Sendmsg.INFO.WorkId, "%d", workid);
				fprintf(stderr, "请输入员工性别：");
				scanf("%c", &Sendmsg.INFO.Sex);
				while(getchar()!='\n');
				fprintf(stderr, "请输入员工电话：");
				scanf("%s", Sendmsg.INFO.Phone);
				while(getchar()!='\n');
				fprintf(stderr, "请输入员工工资：");
				scanf("%f", &Sendmsg.INFO.Salary);
				while(getchar()!='\n');
				Sendmsg.Type = ROOT_ADD;
				do{
					ret = send(cfd, (void*)&Sendmsg, sizeof(Sendmsg), 0);
				}while(ret<0 && ret ==EINTR);
				do{
					ret = recv(cfd, (void*)&Recvmsg, sizeof(Recvmsg), 0);
				}while(ret<0 && errno == EINTR);
				if(Recvmsg.Type == ADD_FAIL){
					fprintf(stderr, "该员工信息添加失败，信息已存在！输入任意键继续！\n");
				}else if(Recvmsg.Type == ROOT_ADD_SUCCESS){
					fprintf(stderr, "该员工信息添加成功！账号为电话号码，密码为工号。输入任意键继续！\n");
				}
			}
			while(getchar()!='\n');
			break;
		case 2:
			Changeinfo(cfd, Sendmsg, Recvmsg);
			break;
		case 3:
			if(quanxian==0){
				//普通用户操作
				fprintf(stderr, "此用户无权限！请重新输入：\n");
				goto SCANF_3;
			}else{
				//管理员用户操作删除
				fprintf(stderr, "请输入要删除员工的员工号：");
SCAN_5://输入的工号非法
				workid =0;
				scanf("%d", &workid);
				while(getchar()!='\n');
				if(workid>10000 || workid <999){
					fprintf(stderr, "输入的员工号非法，请重新输入：");
					goto SCAN_5;
				}
				sprintf(Sendmsg.INFO.WorkId, "%d", workid);
				Sendmsg.Type = ROOT_DEL;
				do{
					ret = send(cfd, (void*)&Sendmsg, sizeof(Sendmsg), 0);
				}while(ret<0 && errno == EINTR);
				do{
					ret = recv(cfd, (void*)&Recvmsg, sizeof(Recvmsg), 0);
				}while(ret<0&&errno == EINTR);
				if(Recvmsg.Type == ROOT_DEL_FAIL){
					fprintf(stderr, "删除失败，该工号不存在！\n");
				}else if(Recvmsg.Type == ROOT_DEL_SUCCESS){
					fprintf(stderr, "员工信息删除成功！\n");
				}
			}
			fprintf(stderr, "请输入任意键继续！");
			while(getchar()!='\n');
			break;
		case 4:
			if(quanxian == 0){
				strcpy(Sendmsg.Res, "0");
				Findmsg(cfd, Sendmsg, Recvmsg);
			}else{
				strcpy(Sendmsg.Res, "1");
				Findmsg(cfd, Sendmsg, Recvmsg);
			}
			while(getchar()!='\n');
			break;
		case 5:
			Sendmsg.Type = EXIT_SUCCESS1;
			strcpy(Sendmsg.Id, ID);
			bzero(ID, sizeof(ID));
			fprintf(stderr, "%s:账号需要退出登录！\n", Recvmsg.Id);
			do{
				ret = send(cfd, (void*)&Sendmsg, sizeof(Sendmsg), 0);
			}while(ret<0 && errno ==EINTR);
			fprintf(stderr, "退出成功！line:%d\n", __LINE__);
			return 10;
		}
	}
}

void Findmsg(int cfd, InfoP Sendmsg, InfoP Recvmsg){
	int tmp =0;
	fprintf(stderr, "请输入要查找的员工工号(查看所有请输入ALL)：");
TEM:
	tmp =0;
	scanf("%d", &tmp);
	while(getchar()!='\n');
	if(tmp>10000 || tmp<999){
		fprintf(stderr, "输入错误，请重新输入：");
		goto TEM;
	}else if(tmp == 27){
		bzero(Sendmsg.INFO.WorkId, sizeof(Sendmsg.INFO.WorkId));
		Sendmsg.Type = ROOT_FIND; 
		do{
			ret = send(cfd, (void*)&Sendmsg, sizeof(Sendmsg), 0);
		}while(ret<0 && errno == EINTR);
	}else{
		Sendmsg.Type = ROOT_FIND;
		sprintf(Sendmsg.INFO.WorkId, "%d", tmp);
		do{
			ret = send(cfd, (void*)&Sendmsg, sizeof(Sendmsg), 0);
		}while(ret<0 && errno == EINTR);
	}
	while(strcmp(Recvmsg.Res, "10")){
		do{
			ret = recv(cfd, (void*)&Recvmsg, sizeof(Recvmsg), 0);
		}while(ret<0 && errno == EINTR);
		if(!(Recvmsg.Type == FIND_FAIL))
			fprintf(stderr, "姓名：%s  工号：%s  性别：%c  工资：%g  电话：%s", 
				Recvmsg.INFO.Name, Recvmsg.INFO.WorkId, Recvmsg.INFO.Sex,
				Recvmsg.INFO.Salary, Recvmsg.INFO.Phone);
		else{
			fprintf(stderr, "%s:该员工信息不存在！\n", Sendmsg.INFO.WorkId);
			return;
		}
	}
}

void Changeinfo(int cfd, InfoP Sendmsg, InfoP Recvmsg){
	int quanxian=0;
	int workid =0;
	if(strcmp(Recvmsg.Res, "0"))
		quanxian=1;
	else
		quanxian=0;
	fprintf(stderr, "请输入修改后的员工姓名(可忽略)：");
	fgets(Sendmsg.INFO.Name, 20, stdin);
	Sendmsg.INFO.Name[strlen(Sendmsg.INFO.Name)-1] = '\0';
	fprintf(stderr, "请输入员工工号：");
SCANF_5:
	workid=0;
	scanf("%d", &workid);
	while(getchar()!='\n');
	if(workid>10000 || workid<999){
		fprintf(stderr, "输入不规范，请重新输入：");
		goto SCANF_5;
	}
	sprintf(Sendmsg.INFO.WorkId, "%d", workid);
	fprintf(stderr, "请输入员工电话：");
	fgets(Sendmsg.INFO.Phone, 15, stdin);
	Sendmsg.INFO.Phone[strlen(Sendmsg.INFO.Phone)-1] = '\0';
	fprintf(stderr, "请输入员工工资：");
	fgets(cmdbuf, 8, stdin);
	Sendmsg.INFO.Salary = atof(cmdbuf);
	Sendmsg.Type = ROOT_CHANGE;
	if(quanxian == 1)
		sprintf(Sendmsg.Res, "1");
	else
		sprintf(Sendmsg.Res, "0");
	do{
		ret = send(cfd, (void*)&Sendmsg, sizeof(Sendmsg), 0);
	}while(ret<0 && errno == EINTR);

	fprintf(stderr, "%s:%s:%d\n", __FILE__, __func__, __LINE__);
	do{
		ret=recv(cfd, (void*)&Recvmsg, sizeof(Recvmsg), 0);
	}while(ret<0 && errno ==EINTR);

	if(Recvmsg.Type == CHANGE_SUCCESS){
		fprintf(stderr, "修改信息成功！\n");
	}else if(Recvmsg.Type == CHANGE_FAIL){
		fprintf(stderr, "该工号不存在！\n");
	}else if(Recvmsg.Type == CHANGE_NO){
		fprintf(stderr, "该账号无修改姓名和工资权限！\n");
	}
}







