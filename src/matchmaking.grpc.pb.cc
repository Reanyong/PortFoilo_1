// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: matchmaking.proto

#include "matchmaking.pb.h"
#include "matchmaking.grpc.pb.h"

#include <functional>
#include <grpcpp/support/async_stream.h>
#include <grpcpp/support/async_unary_call.h>
#include <grpcpp/impl/channel_interface.h>
#include <grpcpp/impl/client_unary_call.h>
#include <grpcpp/support/client_callback.h>
#include <grpcpp/support/message_allocator.h>
#include <grpcpp/support/method_handler.h>
#include <grpcpp/impl/rpc_service_method.h>
#include <grpcpp/support/server_callback.h>
#include <grpcpp/impl/server_callback_handlers.h>
#include <grpcpp/server_context.h>
#include <grpcpp/impl/service_type.h>
#include <grpcpp/support/sync_stream.h>
namespace matchmaking {

static const char* MatchmakingService_method_names[] = {
  "/matchmaking.MatchmakingService/JoinMatchmaking",
  "/matchmaking.MatchmakingService/CancelMatchmaking",
  "/matchmaking.MatchmakingService/GetMatchmakingStatus",
  "/matchmaking.MatchmakingService/UpdateMMR",
  "/matchmaking.MatchmakingService/GetMMR",
};

std::unique_ptr< MatchmakingService::Stub> MatchmakingService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< MatchmakingService::Stub> stub(new MatchmakingService::Stub(channel, options));
  return stub;
}

MatchmakingService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options)
  : channel_(channel), rpcmethod_JoinMatchmaking_(MatchmakingService_method_names[0], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_CancelMatchmaking_(MatchmakingService_method_names[1], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetMatchmakingStatus_(MatchmakingService_method_names[2], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_UpdateMMR_(MatchmakingService_method_names[3], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_GetMMR_(MatchmakingService_method_names[4], options.suffix_for_stats(),::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::Status MatchmakingService::Stub::JoinMatchmaking(::grpc::ClientContext* context, const ::matchmaking::JoinRequest& request, ::matchmaking::MatchmakingResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::matchmaking::JoinRequest, ::matchmaking::MatchmakingResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_JoinMatchmaking_, context, request, response);
}

void MatchmakingService::Stub::async::JoinMatchmaking(::grpc::ClientContext* context, const ::matchmaking::JoinRequest* request, ::matchmaking::MatchmakingResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::matchmaking::JoinRequest, ::matchmaking::MatchmakingResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_JoinMatchmaking_, context, request, response, std::move(f));
}

void MatchmakingService::Stub::async::JoinMatchmaking(::grpc::ClientContext* context, const ::matchmaking::JoinRequest* request, ::matchmaking::MatchmakingResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_JoinMatchmaking_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::matchmaking::MatchmakingResponse>* MatchmakingService::Stub::PrepareAsyncJoinMatchmakingRaw(::grpc::ClientContext* context, const ::matchmaking::JoinRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::matchmaking::MatchmakingResponse, ::matchmaking::JoinRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_JoinMatchmaking_, context, request);
}

::grpc::ClientAsyncResponseReader< ::matchmaking::MatchmakingResponse>* MatchmakingService::Stub::AsyncJoinMatchmakingRaw(::grpc::ClientContext* context, const ::matchmaking::JoinRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncJoinMatchmakingRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status MatchmakingService::Stub::CancelMatchmaking(::grpc::ClientContext* context, const ::matchmaking::CancelRequest& request, ::matchmaking::MatchmakingResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::matchmaking::CancelRequest, ::matchmaking::MatchmakingResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_CancelMatchmaking_, context, request, response);
}

void MatchmakingService::Stub::async::CancelMatchmaking(::grpc::ClientContext* context, const ::matchmaking::CancelRequest* request, ::matchmaking::MatchmakingResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::matchmaking::CancelRequest, ::matchmaking::MatchmakingResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_CancelMatchmaking_, context, request, response, std::move(f));
}

void MatchmakingService::Stub::async::CancelMatchmaking(::grpc::ClientContext* context, const ::matchmaking::CancelRequest* request, ::matchmaking::MatchmakingResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_CancelMatchmaking_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::matchmaking::MatchmakingResponse>* MatchmakingService::Stub::PrepareAsyncCancelMatchmakingRaw(::grpc::ClientContext* context, const ::matchmaking::CancelRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::matchmaking::MatchmakingResponse, ::matchmaking::CancelRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_CancelMatchmaking_, context, request);
}

::grpc::ClientAsyncResponseReader< ::matchmaking::MatchmakingResponse>* MatchmakingService::Stub::AsyncCancelMatchmakingRaw(::grpc::ClientContext* context, const ::matchmaking::CancelRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncCancelMatchmakingRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status MatchmakingService::Stub::GetMatchmakingStatus(::grpc::ClientContext* context, const ::matchmaking::StatusRequest& request, ::matchmaking::MatchmakingResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::matchmaking::StatusRequest, ::matchmaking::MatchmakingResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_GetMatchmakingStatus_, context, request, response);
}

void MatchmakingService::Stub::async::GetMatchmakingStatus(::grpc::ClientContext* context, const ::matchmaking::StatusRequest* request, ::matchmaking::MatchmakingResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::matchmaking::StatusRequest, ::matchmaking::MatchmakingResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetMatchmakingStatus_, context, request, response, std::move(f));
}

void MatchmakingService::Stub::async::GetMatchmakingStatus(::grpc::ClientContext* context, const ::matchmaking::StatusRequest* request, ::matchmaking::MatchmakingResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetMatchmakingStatus_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::matchmaking::MatchmakingResponse>* MatchmakingService::Stub::PrepareAsyncGetMatchmakingStatusRaw(::grpc::ClientContext* context, const ::matchmaking::StatusRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::matchmaking::MatchmakingResponse, ::matchmaking::StatusRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_GetMatchmakingStatus_, context, request);
}

::grpc::ClientAsyncResponseReader< ::matchmaking::MatchmakingResponse>* MatchmakingService::Stub::AsyncGetMatchmakingStatusRaw(::grpc::ClientContext* context, const ::matchmaking::StatusRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncGetMatchmakingStatusRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status MatchmakingService::Stub::UpdateMMR(::grpc::ClientContext* context, const ::matchmaking::MMRUpdate& request, ::matchmaking::MMRResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::matchmaking::MMRUpdate, ::matchmaking::MMRResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_UpdateMMR_, context, request, response);
}

void MatchmakingService::Stub::async::UpdateMMR(::grpc::ClientContext* context, const ::matchmaking::MMRUpdate* request, ::matchmaking::MMRResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::matchmaking::MMRUpdate, ::matchmaking::MMRResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_UpdateMMR_, context, request, response, std::move(f));
}

void MatchmakingService::Stub::async::UpdateMMR(::grpc::ClientContext* context, const ::matchmaking::MMRUpdate* request, ::matchmaking::MMRResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_UpdateMMR_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::matchmaking::MMRResponse>* MatchmakingService::Stub::PrepareAsyncUpdateMMRRaw(::grpc::ClientContext* context, const ::matchmaking::MMRUpdate& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::matchmaking::MMRResponse, ::matchmaking::MMRUpdate, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_UpdateMMR_, context, request);
}

::grpc::ClientAsyncResponseReader< ::matchmaking::MMRResponse>* MatchmakingService::Stub::AsyncUpdateMMRRaw(::grpc::ClientContext* context, const ::matchmaking::MMRUpdate& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncUpdateMMRRaw(context, request, cq);
  result->StartCall();
  return result;
}

::grpc::Status MatchmakingService::Stub::GetMMR(::grpc::ClientContext* context, const ::matchmaking::MMRRequest& request, ::matchmaking::MMRResponse* response) {
  return ::grpc::internal::BlockingUnaryCall< ::matchmaking::MMRRequest, ::matchmaking::MMRResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), rpcmethod_GetMMR_, context, request, response);
}

void MatchmakingService::Stub::async::GetMMR(::grpc::ClientContext* context, const ::matchmaking::MMRRequest* request, ::matchmaking::MMRResponse* response, std::function<void(::grpc::Status)> f) {
  ::grpc::internal::CallbackUnaryCall< ::matchmaking::MMRRequest, ::matchmaking::MMRResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetMMR_, context, request, response, std::move(f));
}

void MatchmakingService::Stub::async::GetMMR(::grpc::ClientContext* context, const ::matchmaking::MMRRequest* request, ::matchmaking::MMRResponse* response, ::grpc::ClientUnaryReactor* reactor) {
  ::grpc::internal::ClientCallbackUnaryFactory::Create< ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(stub_->channel_.get(), stub_->rpcmethod_GetMMR_, context, request, response, reactor);
}

::grpc::ClientAsyncResponseReader< ::matchmaking::MMRResponse>* MatchmakingService::Stub::PrepareAsyncGetMMRRaw(::grpc::ClientContext* context, const ::matchmaking::MMRRequest& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderHelper::Create< ::matchmaking::MMRResponse, ::matchmaking::MMRRequest, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(channel_.get(), cq, rpcmethod_GetMMR_, context, request);
}

::grpc::ClientAsyncResponseReader< ::matchmaking::MMRResponse>* MatchmakingService::Stub::AsyncGetMMRRaw(::grpc::ClientContext* context, const ::matchmaking::MMRRequest& request, ::grpc::CompletionQueue* cq) {
  auto* result =
    this->PrepareAsyncGetMMRRaw(context, request, cq);
  result->StartCall();
  return result;
}

MatchmakingService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      MatchmakingService_method_names[0],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< MatchmakingService::Service, ::matchmaking::JoinRequest, ::matchmaking::MatchmakingResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](MatchmakingService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::matchmaking::JoinRequest* req,
             ::matchmaking::MatchmakingResponse* resp) {
               return service->JoinMatchmaking(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      MatchmakingService_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< MatchmakingService::Service, ::matchmaking::CancelRequest, ::matchmaking::MatchmakingResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](MatchmakingService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::matchmaking::CancelRequest* req,
             ::matchmaking::MatchmakingResponse* resp) {
               return service->CancelMatchmaking(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      MatchmakingService_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< MatchmakingService::Service, ::matchmaking::StatusRequest, ::matchmaking::MatchmakingResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](MatchmakingService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::matchmaking::StatusRequest* req,
             ::matchmaking::MatchmakingResponse* resp) {
               return service->GetMatchmakingStatus(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      MatchmakingService_method_names[3],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< MatchmakingService::Service, ::matchmaking::MMRUpdate, ::matchmaking::MMRResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](MatchmakingService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::matchmaking::MMRUpdate* req,
             ::matchmaking::MMRResponse* resp) {
               return service->UpdateMMR(ctx, req, resp);
             }, this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      MatchmakingService_method_names[4],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< MatchmakingService::Service, ::matchmaking::MMRRequest, ::matchmaking::MMRResponse, ::grpc::protobuf::MessageLite, ::grpc::protobuf::MessageLite>(
          [](MatchmakingService::Service* service,
             ::grpc::ServerContext* ctx,
             const ::matchmaking::MMRRequest* req,
             ::matchmaking::MMRResponse* resp) {
               return service->GetMMR(ctx, req, resp);
             }, this)));
}

MatchmakingService::Service::~Service() {
}

::grpc::Status MatchmakingService::Service::JoinMatchmaking(::grpc::ServerContext* context, const ::matchmaking::JoinRequest* request, ::matchmaking::MatchmakingResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status MatchmakingService::Service::CancelMatchmaking(::grpc::ServerContext* context, const ::matchmaking::CancelRequest* request, ::matchmaking::MatchmakingResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status MatchmakingService::Service::GetMatchmakingStatus(::grpc::ServerContext* context, const ::matchmaking::StatusRequest* request, ::matchmaking::MatchmakingResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status MatchmakingService::Service::UpdateMMR(::grpc::ServerContext* context, const ::matchmaking::MMRUpdate* request, ::matchmaking::MMRResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status MatchmakingService::Service::GetMMR(::grpc::ServerContext* context, const ::matchmaking::MMRRequest* request, ::matchmaking::MMRResponse* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace matchmaking

