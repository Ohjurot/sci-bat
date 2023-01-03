/*
 *  SCI-BAT Dashboard script
 *  (c) Copyright 2023 by Ludwig Fuechsl
 */

__SciBatSettings_Accounts_ActiveUser = "";

// Event: Page loaded
$(document).ready(function() {
    // Reset selection
    $("#sci-bat-conf-accounts-select").val("").change();
});

// Event: change of selection of user account
$("#sci-bat-conf-accounts-select").change(function() {
    newac = $("#sci-bat-conf-accounts-select").val();

    // Hide current account and show new one
    $("#sci-bat-conf-accounts-actype-" + __SciBatSettings_Accounts_ActiveUser).hide();
    $("#sci-bat-conf-accounts-actype-" + newac).show();

    // Warning for superadmin
    if(newac == "superadmin")
        $("#sci-bat-conf-accounts-sawarn").show();
    else
        $("#sci-bat-conf-accounts-sawarn").hide();

    // Hide controles
    SciBatSettings_Accounts_HideCtrls();

    // Clear password entrys
    $("#sci-bat-account-action-enable-pw").val("");
    $("#sci-bat-account-action-enable-rpw").val("");
    $("#sci-bat-account-action-chpw-pw").val("");
    $("#sci-bat-account-action-chpw-rpw").val("");

    // Kickoff loading
    if(newac != "")
    {
        $("#sci-bat-conf-accounts-loader").show();
        SciBatSettings_A_LoadJSON("/api/usermod/status/" + newac, SciBatSettings_Accounts_OnLoad)
    }

    // Save new account
    __SciBatSettings_Accounts_ActiveUser = newac;
});

// Hides all controles
function SciBatSettings_Accounts_HideCtrls()
{
    $("#sci-bat-conf-accounts-st-enabled").hide();
    $("#sci-bat-conf-accounts-st-disabled").hide();
    $("#sci-bat-conf-account-card-enable").hide();
    $("#sci-bat-conf-account-card-disable").hide();
    $("#sci-bat-conf-account-card-chpw").hide();
}

// Event: Status for account loaded
function SciBatSettings_Accounts_OnLoad(status)
{
    // Show account status
    if(status["active"])
        $("#sci-bat-conf-accounts-st-enabled").show();
    else
        $("#sci-bat-conf-accounts-st-disabled").show();

    // Show availible controles
    if(status["active"])
    {
        if(__SciBatSettings_Accounts_ActiveUser != "superadmin")
            $("#sci-bat-conf-account-card-disable").show();
        $("#sci-bat-conf-account-card-chpw").show();
    }
    else
    {
        $("#sci-bat-conf-account-card-enable").show();
    }

    // Hide loader
    $("#sci-bat-conf-accounts-loader").hide();
}

// Event: User account updated
function SciBatSettings_Accounts_OnUpdate(success)
{
    // Show toast
    if(success)
    {
        const toast = new bootstrap.Toast($("#sci-bat-conf-accounts-fb-toast-ok"));
        toast.show()
    }
    else
    {
        const toast = new bootstrap.Toast($("#sci-bat-conf-accounts-fb-toast-err"));
        toast.show()
    }

    // Reload information
    $("#sci-bat-conf-accounts-select").change();
}

// Action: Disable the currently selected account
$("#sci-bat-conf-account-action-disable").click(function() {
    // Go to loading state
    $("#sci-bat-conf-accounts-loader").show();
    SciBatSettings_Accounts_HideCtrls();

    // Kickoff request
    SciBatSettings_A_SavePlain("/api/usermod/deactivate/" + __SciBatSettings_Accounts_ActiveUser, "", SciBatSettings_Accounts_OnUpdate);
});

// Dispatches set password and activate
function SciBatSettings_Accounts_SetPasswordAndActivete(pw1, pw2)
{
    if(pw1 == pw2)
    {
        $("#sci-bat-conf-accounts-loader").show();
        SciBatSettings_Accounts_HideCtrls();
        SciBatSettings_A_SavePlain("/api/usermod/activate/" + __SciBatSettings_Accounts_ActiveUser, pw1, SciBatSettings_Accounts_OnUpdate);
    }
    else
    {
        const toast = new bootstrap.Toast($("#sci-bat-conf-accounts-fb-toast-err"));
        toast.show()
    }
}

// Action: Enables the currently selected account
$("#sci-bat-conf-account-action-enable").click(function() {
    pw1 = $("#sci-bat-account-action-enable-pw").val();
    pw2 = $("#sci-bat-account-action-enable-rpw").val();
    SciBatSettings_Accounts_SetPasswordAndActivete(pw1, pw2);
});

// Action: Enables the currently selected account
$("#sci-bat-conf-account-action-chpw").click(function() {
    pw1 = $("#sci-bat-account-action-chpw-pw").val();
    pw2 = $("#sci-bat-account-action-chpw-rpw").val();
    SciBatSettings_Accounts_SetPasswordAndActivete(pw1, pw2);
});
