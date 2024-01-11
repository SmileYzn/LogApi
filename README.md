<p align="center">
    <a href="https://github.com/SmileYzn/LogApi/issues"><img alt="GitHub Issues" src="https://img.shields.io/github/issues-raw/smileyzn/LogApi?style=flat-square"></a>
    <a href="https://github.com/SmileYzn/LogApi/actions"><img alt="GitHub Workflow Status" src="https://img.shields.io/github/actions/workflow/status/SmileYzn/LogApi/msbuild.yml?branch=main&label=Windows&style=flat-square"></a>
    <a href="https://github.com/SmileYzn/LogApi/actions"><img alt="GitHub Workflow Status" src="https://img.shields.io/github/actions/workflow/status/smileyzn/LogApi/makefile.yml?branch=main&label=Linux&style=flat-square"></a>
    <a href="https://github.com/SmileYzn/LogApi/releases/latest"><img src="https://img.shields.io/github/downloads/SmileYzn/LogApi/total?label=Download%40latest&style=flat-square&logo=github&logoColor=white" alt="Download"></a>
</p>

# HLDS Log API
MetaMod Plugin that uses JSON API to comunicate with WebServer using HTTP/S Protocol.

## What that plugin do
The Plugin sends requests to a webserver using JSON format.
And it can get responses from the webserver of what to do on the HLDS side using specific data models.

## Console Vriables
| Console Variable | Default | Required | Description                                                |
|------------------|:-------:|:--------:|------------------------------------------------------------|
| log_api_enable   |    0    |    Yes   | Enable Log API plugin 0 Disable 1 Enable                   |
| log_api_address  |    -    |    Yes   | Set Log API Address Ie. https://api.yoursite.com/          |
| log_api_timeout  |   5.0   |    Yes   | Timeout in seconds to wait for response from remote server |
| log_api_bearer   |    -    |    No    | Bearer Token to use with HTTP                              |
| log_api_delay    |   60.0  |    No    | Delay to update Server Info on webserver                   |

## Enable / Disable events
To enable what event will be sent to webserver, edit events.cfg file.
The settings will be work after server change level or restart.

```
ServerActivate 		0
ServerDeactivate 	0
ServerAlertMessage 	0
ServerInfo 		0
ClientConnect 		0
ClientPutInServer 	0
ClientDisconnect 	0
ClientKill 		0
ClientUserInfoChanged 	0
ClientCommand 		0
ClientSay 		0
```

## PHP Example
See how webserver can handle some of events using PHP.
In the example, when player join in server the webserver will return a chat message to all players, and do a sv_restart command.

1. Enable ClientPutInServer event on events.cfg
```
ServerActivate 		0
ServerDeactivate 	0
ServerAlertMessage 	0
ServerInfo 		0
ClientConnect 		0
ClientPutInServer 	1
ClientDisconnect 	0
ClientKill 		0
ClientUserInfoChanged 	0
ClientCommand 		0
ClientSay 		0
```

2. Configure variables on HLDS side (At logapi.cfg file)
```
// Enable Log API
//
// 0 Disable
// 1 Enable
//
// Default "0"
log_api_enable "1"

// Set Log API Address
// API HTTP Address Ie. http://api.yoursite.com/
//
// Default ""
log_api_address "https://yourwebsite.com/api.php"

// Set Log API Timeout
// Timeout in seconds to wait for response from remote server
//
// Default 5.0
log_api_timeout "5.0"

// Set Log API Bearer Token
// Authentication Token or leave empty to disable
//
// Default ""
log_api_bearer "YOUR_API_TOKEN_CVAR"

// Server Info Event Delay
// Delay to update Server Info on webserver
//
// Default "60.0"
log_api_delay "60.0"
```

3. Le's create the api.php file (log_api_addres must be something like: https://yourwebsite.com/api.php)
```PHP
<?php
// Include the LogAPI Class
include("LogAPI.php");

// Set PHP to return content type as json
header('Content-Type: application/json');

// The final default result
$result = null;

// Compare HTTP_AUTHORIZATION with bearer token of (log_api_bearer console variable on HLDS)
if(trim(str_replace('Bearer', '', $_SERVER['HTTP_AUTHORIZATION'])) == 'YOUR_API_TOKEN_CVAR')
{
	// Set the result that comes from LogAPI class
	$result = (new LogAPI)->OnEvent();  
}

// Return final result encoded as json
die(json_encode($result));
```

4. The LogAPI.php class to handle HLDS server events, you can create other classes to handle events later.
```PHP
<?php

class LogAPI
{
    /**
     * On Receive Event 
     */
    function OnEvent()
    {
        // Parse request as json event
        $request = json_decode(file_get_contents('php://input'), true);
        
        // If event is not empty
        if(!empty($request['Event']))
        {
            // If method exists
            if(method_exists($this, $request['Event']))
            {
                // Process paramemeters as array
                $parameters = array_values($request);

                // Return from function call
                return $this->{$request['Event']}(...$parameters);                
            }
        }
        
        // Return null
        return null;
    }

    /**
     * On Client Put In Server
     * 
     * @param string $Event             Event Name
     * @param array $Server		Server information data
     * @param array $Player		Player information data
     * 
     * @return mixed                    Array containing LogAPI commands or null
     */
    protected function ClientPutInServer($Event, $Server, $Player)
    {
	// Print Chat
	$result["PrintChat"] =
	[
		// Entity Index (0) send message to all players
		'EntityId' => 0,

		// The message string (You can use format sequence to get colors in chat)
		'Message' => "{$Player['Name']} entered in server. Game will restart in ^35^1 seconds..."
	];

	// Return server command to HLDS
	$result['ServerCommand'] = "sv_restart 5";

	// Return result to api.php, result can have multiple commands at once
        return $result;
    }
}
```

## Development Alpha Stage
Please, consider that plugin is experimental. Any feedback / request is always welcome.

## Finally
A Future documentation how to use it will be provided in this repository.
