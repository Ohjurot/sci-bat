/*
 *  SCI-BAT Dashboard script
 *  (c) Copyright 2023 by Ludwig Fuechsl
 */

// Load setting
function SciBatSettings_A_Load(setting, callback)
{
    jsonPath = "/api/setting/" + setting;
    SciBatSettings_A_LoadJSON(jsonPath, callback);
}

// Load json data
function SciBatSettings_A_LoadJSON(path, callback)
{
    // Build host based path
    jsonPath = $("#sci-bat-host").attr("href") + path;

    // Invoke request
    xmlhttp = new XMLHttpRequest();
    xmlhttp.open("GET", jsonPath, true);
    xmlhttp.onload = (e) => 
        {
            if (xmlhttp.readyState == 4 && xmlhttp.status == 200) 
            {
                callback(JSON.parse(xmlhttp.responseText));
            }
        };
    xmlhttp.onerror = (e) => 
    {
        console.error(xmlhttp.statusText);
    };
    xmlhttp.send(null);
}

// Save setting
function SciBatSettings_A_Save(setting, data, callback)
{
    jsonPath = "/api/setting/" + setting;
    SciBatSettings_A_SaveJSON(jsonPath, data, callback);
}

// Save json data
function SciBatSettings_A_SaveJSON(path, data, callback)
{
    SciBatSettings_A_SavePlain(path, JSON.stringify(data), callback);
}

// Save plane data
function SciBatSettings_A_SavePlain(path, plainData, callback)
{
    jsonPath = $("#sci-bat-host").attr("href") + path;

    // Invoke request
    xmlhttp = new XMLHttpRequest();
    xmlhttp.open("POST", jsonPath, true);
    xmlhttp.onload = (e) => 
        {
            if (xmlhttp.readyState == 4 && xmlhttp.status == 200) 
            {
                callback(true);
            }
            else
            {
                callback(false);
            }
        };
    xmlhttp.onerror = (e) => 
    {
        console.error(xmlhttp.statusText);
        callback(false);
    };
    xmlhttp.send(plainData);
}

