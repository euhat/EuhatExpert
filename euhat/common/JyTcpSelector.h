#pragma once

#include <common/JyBuf.h>
#include "JySockSelector.h"
#include <common/JyDataCrypto.h>

#define JY_TCP_INI_CERT_SECTION "cert"
#define JY_TCP_INI_SERVER_CERT_KEY "server"
#define JY_TCP_INI_CLIENT_CERT_KEY "client"

enum JyTcpCmdType
{
	JyTcpCmdExchangeAsymSecurity,
	JyTcpCmdExchangeSymSecurity,
	JyTcpCmdUser = 100
};

class JyTcpBuf
{
public:
	JyBufGuard guard_;
	int recvLen_;

	JyTcpBuf(JyBufPool &pool);
	~JyTcpBuf();
};

enum JyTcpSelectType
{
	JyTcpSelectRecv,
	JyTcpSelectAccept,
	JyTcpSelectConnect
};

class JyTcpEndpoint;

class JyTcpListener
{
public:
	JyTcpSelectType selectType_;
	JyTcpEndpoint *listener_;
	unique_ptr<JyTcpBuf> recvBuf_;
	unique_ptr<JyBuf> sendBuf_;

	unique_ptr<JyDataEncryptClear> encClear_;
	unique_ptr<JyDataDecryptClear> decClear_;
	unique_ptr<JyDataEncryptAsymmetric> encAsym_;
	unique_ptr<JyDataEncryptSymmetric> encSym_;
	unique_ptr<JyDataDecryptSymmetric> decSym_;
	unique_ptr<JyDataDecryptAsymmetric> decAsym_;
	JyBuf e_;

	JyDataEncrypt *enc_;
	JyDataDecrypt *dec_;

	JyTcpListener(JyTcpEndpoint *listener, JyTcpSelectType selectType);
	~JyTcpListener();

	int hasWholePacket(unique_ptr<JyTcpBuf> &splitCompleted);
};

class JyTcpSelector : public JySockSelector
{
	map<WhSockHandle, unique_ptr<JyTcpListener> > socks_;

protected:
	int add(WhSockHandle sock, JyTcpSelectType selectType, JyTcpEndpoint *listener);
	int onRead(WhSockHandle sock);
	int onWrite(WhSockHandle sock);

public:
	JyTcpSelector(JyBufPool *pool, JyTcpEndpoint *listener);
	~JyTcpSelector();

	int addRecv(WhSockHandle sock, JyTcpEndpoint *listener);
	int addAccept(int port, JyTcpEndpoint *listener);
	int addConnect(const char *ip, int port, JyTcpEndpoint *listener);
	int send(WhSockHandle sock, JyBuf &buf);
	int del(WhSockHandle sock);
	JyTcpListener *getListener(WhSockHandle sock);
	void totalSentRefresh(int sent);
	void totalRecvRefresh(int recved);

	JyBufPool *pool_;

	int totalSent_;
	int totalRecv_;
	time_t lastTotalSentRefresh_;
	time_t lastTotalRecvRefresh_;
	JyTcpEndpoint *listener_;
};