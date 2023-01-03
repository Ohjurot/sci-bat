/*
 *  SCI-BAT Dashboard script
 *  (c) Copyright 2023 by Ludwig Fuechsl
 */

// Event: Page loaded
$(document).ready(function() {
    // Load initial settings
    SciBatSettings_A_Load("gateway", SciBatSettings_Gateway_OnLoad);
});

// Event: Current configuration loaded
function SciBatSettings_Gateway_OnLoad(config)
{
    // Set current values
    $("#sci-bat-conf-gateway-ipaddr").val(config["address"]);
    $("#sci-bat-conf-gateway-port").val(config["port"]);
    $("#sci-bat-conf-gateway-node").val(config["node"]);
    $("#sci-bat-conf-gateway-pollrate").val(config["pollrate"]);

    // Enable button
    $("#sci-bat-conf-gateway-save").prop("disabled", false);
    $("#sci-bat-conf-gateway-save").html("Save Changes");
}

// Event: Save clicked
$("#sci-bat-conf-gateway-save").click(function() {
    // Disable button
    $("#sci-bat-conf-gateway-save").prop("disabled", true);
    $("#sci-bat-conf-gateway-save").html('<i class="fa-regular fa-floppy-disk fa-beat-fade"></i> Saving Changes...');

    // Build json information
    config = {};
    config["address"] = $("#sci-bat-conf-gateway-ipaddr").val();
    config["port"] = parseInt($("#sci-bat-conf-gateway-port").val());
    config["node"] = parseInt($("#sci-bat-conf-gateway-node").val());
    config["pollrate"] = parseInt($("#sci-bat-conf-gateway-pollrate").val());
    
    // Save settings
    SciBatSettings_A_Save("gateway", config, SciBatSettings_Gateway_OnSave);
});

// Event: Configuration saved
function SciBatSettings_Gateway_OnSave(success)
{
    // Show toast
    if(success)
    {
        const toast = new bootstrap.Toast($("#sci-bat-conf-gateway-fb-toast-ok"));
        toast.show()
    }
    else
    {
        const toast = new bootstrap.Toast($("#sci-bat-conf-gateway-fb-toast-err"));
        toast.show()
    }

    // Reload new data
    SciBatSettings_A_Load("gateway", SciBatSettings_Gateway_OnLoad);
}
