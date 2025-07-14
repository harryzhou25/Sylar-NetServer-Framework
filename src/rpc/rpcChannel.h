#ifndef _SYLAR_RPC_CHANNEL_
#define _SYLAR_RPC_CHANNEL_
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <string>
#include <iostream>

namespace sylar{

class rpcChannel : public google::protobuf::RpcChannel {
public:
void CallMethod(const google::protobuf::MethodDescriptor *method, google::protobuf::RpcController *controller,
                const google::protobuf::Message *request, google::protobuf::Message *response,
                google::protobuf::Closure *done) override;
};

};

#endif //_SYLAR_RPC_CHANNEL_