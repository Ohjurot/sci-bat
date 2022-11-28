#include "GatewayThread.h"

SCI::BAT::Gateway::GatewayThread::GatewayThread(const std::shared_ptr<spdlog::logger>& gatewayLogger /*= spdlog::default_logger()*/)
{
    SetLogger(gatewayLogger);
}

int SCI::BAT::Gateway::GatewayThread::ThreadMain()
{
    throw std::logic_error("The method or operation is not implemented.");
}

void SCI::BAT::Gateway::GatewayThread::OnStop()
{
    throw std::logic_error("The method or operation is not implemented.");
}
