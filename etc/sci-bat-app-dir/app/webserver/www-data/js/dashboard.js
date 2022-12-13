/*
 *  SCI-BAT Dashboard script
 *  (c) Copyright 2022 by Ludwig Fuechsl
 */

// Event: Page loaded
$(document).ready(function() {
    // Setup refresh button
    $("#sci-bat-dash-reload").click(function() {
        SciBatDashTick();
    });

    // Run first tick and setup interval
    SciBatDashTick();
    setInterval('SciBatDashTick()', 2500);
});

// Function/Event: Tick (Called every 5 seconds)
function SciBatDashTick()
{
    jsonPath = $("#sci-bat-dash-jsonlocation").attr("href");

    // Invoke request
    xmlhttp = new XMLHttpRequest();
    xmlhttp.open("GET", jsonPath, true);
    xmlhttp.onload = (e) => 
        {
            if (xmlhttp.readyState == 4 && xmlhttp.status == 200) 
            {
                SciBatDashUpdate(JSON.parse(xmlhttp.responseText));
            }
        };
    xmlhttp.onerror = (e) => 
    {
        console.error(xmlhttp.statusText);
    };
    xmlhttp.send(null);
}

// Function/Event: Update (Called when json data is ready)
function SciBatDashUpdate(json)
{
    // console.log(json);

    // Setup header
    batteryCapacityPercent = Math.trunc(Math.ceil(json["battery"]["charge"] * 100.0));
    setpoint = json["inverter"]["setpoint"];
    mode = 0;
    if(setpoint > 0)
    {
        mode = -1; // Discharging
    }
    else if(setpoint < 0)
    {
        mode = 1; // Charging
    }
    
    $("#sci-bat-dash-d-top-bar").html(batteryCapacityPercent + "%").css("width", batteryCapacityPercent + "%");
    switch(mode)
    {
        case -1:
            $("#sci-bat-dash-d-top-header").html("Discharging");
            $("#sci-bat-dash-d-top-bolt").removeClass("text-success").removeClass("text-primary").removeClass("text-warning").removeClass("fa-beat-fade")
                .addClass("text-success").addClass("fa-beat-fade");
            $("#sci-bat-dash-d-top-time").html(json["battery"]["remaining-discharging-time"]);
            $("#sci-bat-dash-d-top-bar").addClass("progress-bar-animated");
            break;
        case 0:
            $("#sci-bat-dash-d-top-header").html("Iydle");
            $("#sci-bat-dash-d-top-bolt").removeClass("text-success").removeClass("text-primary").removeClass("text-warning").removeClass("fa-beat-fade")
                .addClass("text-primary");
            $("#sci-bat-dash-d-top-bar").removeClass("progress-bar-animated");
            break;
        case 1:
            $("#sci-bat-dash-d-top-header").html("Charging");
            $("#sci-bat-dash-d-top-bolt").removeClass("text-success").removeClass("text-primary").removeClass("text-warning").removeClass("fa-beat-fade")
                .addClass("text-warning").addClass("fa-beat-fade");
            $("#sci-bat-dash-d-top-time").html(json["battery"]["remaining-charging-time"]);
            $("#sci-bat-dash-d-top-bar").addClass("progress-bar-animated");
            break;
    }

    // Setup relais
    if(json["relays"]["relay1"]) $("#sci-bat-dash-d-relay-r1i").removeClass("text-bg-dark").addClass("text-bg-success");
    else $("#sci-bat-dash-d-relay-r1i").removeClass("text-bg-success").addClass("text-bg-dark");
    if(json["relays"]["relay2"]) $("#sci-bat-dash-d-relay-r2i").removeClass("text-bg-dark").addClass("text-bg-success");
    else $("#sci-bat-dash-d-relay-r2i").removeClass("text-bg-success").addClass("text-bg-dark");
    if(json["relays"]["relay3"]) $("#sci-bat-dash-d-relay-r3i").removeClass("text-bg-dark").addClass("text-bg-success");
    else $("#sci-bat-dash-d-relay-r3i").removeClass("text-bg-success").addClass("text-bg-dark");
    if(json["relays"]["relay4"]) $("#sci-bat-dash-d-relay-r4i").removeClass("text-bg-dark").addClass("text-bg-success");
    else $("#sci-bat-dash-d-relay-r4i").removeClass("text-bg-success").addClass("text-bg-dark");

    // Cooloff delay
    if((json["tcontrol"]["mode"] == "Off" || json["tcontrol"]["mode"] == "Cooling") && json["relays"]["relay2"]) $("#sci-bat-dash-d-tcontrol-cooloff").removeClass("invisible");
    else $("#sci-bat-dash-d-tcontrol-cooloff").addClass("invisible");

    // Status
    $("#sci-bat-dash-d-tcontrol-status").html(json["tcontrol"]["mode"]);

    // Setup batter metadata
    $("#sci-bat-dash-d-battery-status").html(json["battery"]["status"]);
    $("#sci-bat-dash-d-battery-voltage").html(json["battery"]["voltage"]);
    $("#sci-bat-dash-d-battery-current").html(json["battery"]["current"]);
    $("#sci-bat-dash-d-battery-temperature").html(json["battery"]["temperature"]);

    // Setup inverter metadata
    $("#sci-bat-dash-d-inverter-status").html(json["inverter"]["status"]);
    $("#sci-bat-dash-d-inverter-power").html(json["inverter"]["power"]);
    $("#sci-bat-dash-d-inverter-setpoint").html(json["inverter"]["setpoint"]);

    // Setup grid metadata
    $("#sci-bat-dash-d-grid-voltage").html(json["grid"]["voltage"]);
    $("#sci-bat-dash-d-grid-frequency").html(json["grid"]["freqency"]);

    // Set updated text
    $('#sci-bat-dash-last-update').html(new Date().toLocaleString());

    // Hide placeholdedr & Make content visible
    $("#sci-bat-dash-placeholder").removeClass("d-block").addClass("d-none");
    $("#sci-bat-dash-content").removeClass("d-none").addClass("d-block");
}

