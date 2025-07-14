#include "rpc/rpcChannel.h"
#include "rpc/header.pb.h"
#include "zkClient/zkClient.h"
#include "socket/socket.h"
#include "log/logger.h"
#include <string>

namespace sylar {

static std::string zk_host = "127.0.0.1:0000";

static Logger::Ptr g_logger = Name_Logger("system");

void rpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method, google::protobuf::RpcController *controller,
    const google::protobuf::Message *request, google::protobuf::Message *response, google::protobuf::Closure *done) {
    const google::protobuf::ServiceDescriptor *service_des = method->service();
    std::string service_name = service_des->name();
    std::string method_name = method->name();
    int arg_size = 0;
    std::string arg_str;
    if (request->SerializeToString(&arg_str)) {
        arg_size = arg_str.size();
    }
    else {
        Log_Error(g_logger) << "rpcChannel::CallMethod() request SerializeToString Failed : "
        << service_name << " " << method_name;
        controller->SetFailed("request SerializeToString Failed");
        return;
    }
    
    sylar_rpc::Header rpc_header;
    rpc_header.set_service_name(service_name);
    rpc_header.set_method_name(method_name);
    rpc_header.set_args_size(arg_size);

    uint32_t header_size = 0;
    std::string rpc_header_str;
    if(rpc_header.SerializeToString(&rpc_header_str)) {
        header_size = rpc_header_str.size();
    }
    else {
        Log_Error(g_logger) << "rpcChannel::CallMethod() request SerializeToString Failed : "
        << service_name << ' ' << method_name << ' ' << arg_size << ' ' << arg_str;
        controller->SetFailed("request SerializeToString Failed");
        return;
    }

    std::string send_rpc_str;
    send_rpc_str.insert(0, std::string((char *)&header_size, 4));
    send_rpc_str += rpc_header_str;
    send_rpc_str += arg_str;

    auto client_fd = Socket::CreateTCPSocket();
    ZKClient::Ptr zk_client(new ZKClient());
    zk_client->init(zk_host, 5000, nullptr);
    
    std::string method_path = "/" + service_name + "/" + method_name;
    std::string host_data;
    int rt = zk_client->get(method_path, host_data, false);
    if(rt != ZOK) {
        Log_Error(g_logger) << "rpcChannel::CallMethod() zookeeper get failed "
        << service_name << ' ' << method_name << ' ' << arg_size << ' ' << arg_str
        << ' ' << rt;
        controller->SetFailed("zookeeper get failed");
        return;
    }

}

} // namespace sylar
