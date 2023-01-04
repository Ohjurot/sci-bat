#include "SysStatusController.h"

void SCI::BAT::Webserver::Controllers::SysStatusController::OnGet(const httplib::Request& request, httplib::Response& response)
{
    nlohmann::json data;
    auto user = HTTPAuthentication::Session(request, response, data);
    if (user && (int)user.permissionLevel >= (int)HTTPUser::PermissionLevel::Operator)
    {
        // Get stats of relevant threads
        auto tidGateway = Gateway::GatewayThread::GetStaticTID();
        bool runningGateway = !Gateway::GatewayThread::IsStaticFinished();
        auto tidMailbox = Mailbox::MailboxThread::GetStaticTID();
        bool runningMailbox = !Mailbox::MailboxThread::IsStaticFinished();
        auto tidTControle = TControle::TControlThread::GetStaticTID();
        bool runningTControle = !TControle::TControlThread::IsStaticFinished();

        // Get system status
        auto gatewayConnection = Gateway::GatewayThread::GetConnectionString();
        bool gatewaySmaConnected = Gateway::GatewayThread::GetSMAConnected();
        bool gatewaySmaUpdated = Gateway::GatewayThread::GetSMAUpdateOk();
        auto mailboxConnection = Mailbox::MailboxThread::GetConnectionString();
        bool mailboxConnected = Mailbox::MailboxThread::GetConnected();
        auto tcontroleDevice = TControle::TControlThread::GetSerialDevice();
        bool tcontroleDeviceAvailable = TControle::TControlThread::GetDeviceAvailable();
        bool tcontroleLastCmdOk = TControle::TControlThread::GetLastCommandOk();

        // Build json
        nlohmann::json sysStatusJson = {
            { "threads", {
                { "gateway", {
                    { "tid", ConvertTID(tidGateway) },
                    { "running", runningGateway},
                }},
                { "mailbox", {
                    { "tid", ConvertTID(tidMailbox) },
                    { "running", runningMailbox},
                }},
                { "tcontrol", {
                    { "tid", ConvertTID(tidTControle) },
                    { "running", runningTControle},
                }},
            }},
            { "system", {
                { "gateway", {
                    { "connection", gatewayConnection },
                    { "inverterConnected", gatewaySmaConnected },
                    { "inverterUpdated", gatewaySmaUpdated },
                }},
                { "mailbox", {
                    { "connection", mailboxConnection },
                    { "connected", mailboxConnected },
                }},
                { "tcontrol", {
                    { "device", tcontroleDevice },
                    { "deviceAvailable", tcontroleDeviceAvailable },
                    { "lastCmdOk", tcontroleLastCmdOk},
                }},
            }}
        };

        // Render data
        return RenderJSON(response, sysStatusJson);
    }
    else
    {
        response.status = 401;
    }
}

int SCI::BAT::Webserver::Controllers::SysStatusController::ConvertTID(const std::jthread::id& id)
{
    int rc;
    std::stringstream ss;
    ss << id;
    ss >> rc;
    return rc;
}
