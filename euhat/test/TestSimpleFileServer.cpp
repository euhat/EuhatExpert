#include <EuhatPreDef.h>
#include <common/OpCommon.h>
#include <common/JyDataStream.h>
#include <common/WhCommon.h>
#include <app/SimpleFileServer/server/SimpleFileServer.h>
#include <app/SimpleFileServer/client/SimpleFileClient.h>
#include <EuhatPostDef.h>

class TestSimpleFileServer
{
	unique_ptr<JyTcpServer> srv_;
	unique_ptr<JyTcpClient> cli_;
public:
	void run()
	{
		SimpleFileServer *sfSrv = new SimpleFileServer(8083);
		SimpleFileClient *sfCli = new SimpleFileClient("127.0.0.1", 8083);
		srv_.reset(sfSrv);
		cli_.reset(sfCli);
		sfCli->peerPath_ = "aa.dat";
		sfCli->localPath_ = "bb.dat";
		sfCli->filePos_ = 0;
		srv_->start();
		cli_->start();
	}
};