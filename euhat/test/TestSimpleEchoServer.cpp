#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/JyDataStream.h>
#include <common/WhCommon.h>
#include <app/SimpleEchoServer/server/SimpleEchoServer.h>
#include <app/SimpleEchoServer/client/SimpleEchoClient.h>
#include <EuhatPostDef.h>

class TestSimpleEchoServer
{
	unique_ptr<JyTcpServer> srv_;
	unique_ptr<JyTcpClient> cli_;
public:
	void run()
	{
		srv_.reset(new SimpleEchoServer(8083));
		cli_.reset(new SimpleEchoClient("127.0.0.1", 8083));
		srv_->start();
		cli_->start();
	}
};