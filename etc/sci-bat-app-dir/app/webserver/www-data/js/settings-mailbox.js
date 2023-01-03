/*
 *  SCI-BAT Dashboard script
 *  (c) Copyright 2023 by Ludwig Fuechsl
 */

// Event: Page loaded
$(document).ready(function() {
    // Load initial settings
    SciBatSettings_A_Load("mailbox", SciBatSettings_Mailbox_OnLoad);
});

// Event: Current configuration loaded
function SciBatSettings_Mailbox_OnLoad(config)
{
    // Set current values
    $("#sci-bat-conf-mailbox-addr").val(config["broker"]["address"]);
    $("#sci-bat-conf-mailbox-port").val(config["broker"]["port"]);
    $("#sci-bat-conf-mailbox-username").val(config["broker"]["username"]);
    $("#sci-bat-conf-mailbox-password").val(config["broker"]["password"]);
    $("#sci-bat-conf-mailbox-btopic").val(config["basetopic"]);

    // Enable button
    $("#sci-bat-conf-mailbox-save").prop("disabled", false);
    $("#sci-bat-conf-mailbox-save").html("Save Changes");
}

// Event: Save clicked
$("#sci-bat-conf-mailbox-save").click(function() {
    // Disable button
    $("#sci-bat-conf-mailbox-save").prop("disabled", true);
    $("#sci-bat-conf-mailbox-save").html('<i class="fa-regular fa-floppy-disk fa-beat-fade"></i> Saving Changes...');

    // Build json information
    config = {};
    config["broker"] = {};
    config["broker"]["address"] = $("#sci-bat-conf-mailbox-addr").val();
    config["broker"]["port"] = parseInt($("#sci-bat-conf-mailbox-port").val());
    config["broker"]["username"] = $("#sci-bat-conf-mailbox-username").val();
    config["broker"]["password"] = $("#sci-bat-conf-mailbox-password").val();
    config["basetopic"] = $("#sci-bat-conf-mailbox-btopic").val();

    // Save settings
    SciBatSettings_A_Save("mailbox", config, SciBatSettings_Mailbox_OnSave);
});

// Event: Configuration saved
function SciBatSettings_Mailbox_OnSave(success)
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
    SciBatSettings_A_Load("mailbox", SciBatSettings_Mailbox_OnLoad);
}
