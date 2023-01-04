/*
 *  SCI-BAT Dashboard script
 *  (c) Copyright 2023 by Ludwig Fuechsl
 */

// Event: Page loaded
$(document).ready(function() {
    // Schedule updates and initial load
    SciBatSettings_Status_OnTick();
    setInterval('SciBatSettings_Status_OnTick()', 5000);
});

// Event: Tick 5s
function SciBatSettings_Status_OnTick()
{
    // Load current JSON data
    SciBatSettings_A_LoadJSON("/api/sysstatus", SciBatSettings_Status_OnLoad);
}

// Sets the state of a thread (badge)
function SciBatSettings_Status_SetThreadStatus(base, running)
{
    if(running)
    {
        $("#" + base + "-running").show();
        $("#" + base + "-terminated").hide();
    }
    else
    {
        $("#" + base + "-running").hide();
        $("#" + base + "-terminated").show();
    }
}

// Event: On JSON load
function SciBatSettings_Status_OnLoad(status)
{
    // Load Gateway status
    $("#sci-bat-conf-status-gateway-tid").html(status["threads"]["gateway"]["tid"]);
    SciBatSettings_Status_SetThreadStatus("sci-bat-conf-status-gateway-ts", status["threads"]["gateway"]["running"]);
    $("#sci-bat-conf-status-gateway-connection").html(status["system"]["gateway"]["connection"]);
    $("#sci-bat-conf-status-gateway-connected").html(status["system"]["gateway"]["inverterConnected"].toString());
    $("#sci-bat-conf-status-gateway-updated").html(status["system"]["gateway"]["inverterUpdated"].toString());

    // Load Mailbox status
    $("#sci-bat-conf-status-mailbox-tid").html(status["threads"]["mailbox"]["tid"]);
    SciBatSettings_Status_SetThreadStatus("sci-bat-conf-status-mailbox-ts", status["threads"]["mailbox"]["running"]);
    $("#sci-bat-conf-status-mailbox-connection").html(status["system"]["mailbox"]["connection"]);
    $("#sci-bat-conf-status-mailbox-connected").html(status["system"]["mailbox"]["connected"]);

    // Load TControl status
    $("#sci-bat-conf-status-tcontrol-tid").html(status["threads"]["tcontrol"]["tid"]);
    SciBatSettings_Status_SetThreadStatus("sci-bat-conf-status-tcontrol-ts", status["threads"]["tcontrol"]["running"]);
    $("#sci-bat-conf-status-tcontrol-device").html(status["system"]["tcontrol"]["device"]);
    $("#sci-bat-conf-status-tcontrol-available").html(status["system"]["tcontrol"]["deviceAvailable"].toString());
    $("#sci-bat-conf-status-tcontrol-command").html(status["system"]["tcontrol"]["lastCmdOk"].toString());
    
    // Hide loader and view container
    $("#sci-bat-conf-status-loader").hide();
    $("#sci-bat-conf-status-main").show();
}

// Event: Restart service button
$("#sci-bat-conf-status-restart").click(function (){
    $("#sci-bat-conf-status-restart-modal-open").prop("disabled", true);
    SciBatSettings_A_SavePlain("/api/sysctrl/restart", "", SciBatSettings_Status_OnRestart);
})

// Event: Restart done
function SciBatSettings_Status_OnRestart(success)
{
    // Show toast
    if(success)
    {
        const toast = new bootstrap.Toast($("#sci-bat-conf-status-fb-toast-ok"));
        toast.show()
    }
    else
    {
        const toast = new bootstrap.Toast($("#sci-bat-conf-status-fb-toast-err"));
        $("#sci-bat-conf-status-restart-modal-open").prop("disabled", false);
        toast.show()
    }
}
