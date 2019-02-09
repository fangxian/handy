#include <handy/handy.h>
#include <iostream>
#include <stdio.h>
using namespace std;
using namespace handy;

int readcount = 0;
int testwrite = 1;
int main()
{
	int count = 0;
	setloglevel("DEBUG");
	EventBase bases;
	Signal::signal(SIGINT, [&]{bases.exit();});
	TcpServer echo(&bases);
	int r = echo.bind("", 2099);
	auto writecb = [&](const TcpConnPtr& con)
	{
		count++;
		printf("do write\n");
		//std::cout << "write " << con->getInput().data()  << std::endl; 
		con->send(con->getInput());
	};	
	echo.onConnCreate([writecb](){
		TcpConnPtr con(new TcpConn);
		con->onState([writecb](const TcpConnPtr& con)
		{
			if(con->getState()==TcpConn::Connected)
			{
				con->onWritable(writecb);
				Channel* ch = con->getChannel();
				ch->enableWrite(true);
			}		
		});
		return con;
	});
	echo.onConnRead([](const TcpConnPtr& con){
		printf("do read \n");
		Channel* ch = con->getChannel();
		//ch->enableRead(true);
		ch->enableWrite(true);
	});
	bases.loop();
	return 0;
}

