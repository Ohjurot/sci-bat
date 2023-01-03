/*
 *  SCI-BAT Dashboard script
 *  (c) Copyright 2023 by Ludwig Fuechsl
 */

__SciBatSettings_TControle_Config = {};

// Event: Page loaded
$(document).ready(function() {
    // Load availible devices (First because: 1. we can't have two async request running at a time 2. this is only required once and will trigger the next load)
    SciBatSettings_A_LoadJSON("/api/serial/list", SciBatSettings_TControle_OnSerialLoad);
});

// Event: Current configuration loaded
function SciBatSettings_TControle_OnLoad(config)
{
    // Store active configuration
    __SciBatSettings_TControle_Config = config;

    // Set current values
    $("#sci-bat-conf-tcontrole-current").val(config["serial"]["device"]);
    $("#sci-bat-conf-tcontrole-cooloff").val(config["cooloff-time"]);
    $("#sci-bat-conf-tcontrole-select").val("").change();

    // Enable button
    $("#sci-bat-conf-tcontrole-save").prop("disabled", false);
    $("#sci-bat-conf-tcontrole-save").html("Save Changes");
}

// Event: List of serial ports received
function SciBatSettings_TControle_OnSerialLoad(serial)
{
    // Add devices to selection
    serial["devices"].forEach((item) => {
        $('#sci-bat-conf-tcontrole-select').append($('<option>', {value: item, text: item}));
    });

    // We will now load the actual data
    SciBatSettings_A_Load("tcontrole", SciBatSettings_TControle_OnLoad);
}

// Event: Save clicked
$("#sci-bat-conf-tcontrole-save").click(function() {
    // Disable button
    $("#sci-bat-conf-tcontrole-save").prop("disabled", true);
    $("#sci-bat-conf-tcontrole-save").html('<i class="fa-regular fa-floppy-disk fa-beat-fade"></i> Saving Changes...');

    // Build json information
    config = __SciBatSettings_TControle_Config;
    if($("#sci-bat-conf-tcontrole-select").val() != "")
        config["serial"]["device"] = $("#sci-bat-conf-tcontrole-select").val();
    config["cooloff-time"] = parseInt($("#sci-bat-conf-tcontrole-cooloff").val());

    // Save settings
    SciBatSettings_A_Save("tcontrole", config, SciBatSettings_TControle_OnSave);
});

// Event: Configuration saved
function SciBatSettings_TControle_OnSave(success)
{
    // Show toast
    if(success)
    {
        const toast = new bootstrap.Toast($("#sci-bat-conf-tcontrole-fb-toast-ok"));
        toast.show()
    }
    else
    {
        const toast = new bootstrap.Toast($("#sci-bat-conf-tcontrole-fb-toast-err"));
        toast.show()
    }

    // Reload new data
    SciBatSettings_A_Load("tcontrole", SciBatSettings_TControle_OnLoad);
}
