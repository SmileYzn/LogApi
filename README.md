<p align="center">
    <a href="https://github.com/SmileYzn/LogApi/issues"><img alt="GitHub Issues" src="https://img.shields.io/github/issues-raw/smileyzn/LogApi?style=flat-square"></a>
    <a href="https://github.com/SmileYzn/LogApi/actions"><img alt="GitHub Workflow Status" src="https://img.shields.io/github/actions/workflow/status/SmileYzn/LogApi/build.yml?branch=main&label=C/C++&style=flat-square"></a>
</p>

# ReHLDS Log API
MetaMod Plugin that uses JSON API to comunicate with WebServer using HTTP/S Protocol.

## What that plugin do
The Plugin sends requests to a webserver using JSON format.
And it can get responses from the webserver of what to do on the ReHLDS side using specific data models.

The main idea is control ReHLDS server using server or player events, that will be handled by webserver.
Like create a ban system, register system, live server list or something that envolves webservers, databases, players and others.

## Visit our documentation
You can learn what LogAPI can do in HLDS server, control the events, create responses and more: [LogApi Wiki](https://github.com/SmileYzn/LogApi/wiki)

## Console Vriables
| Console Variable | Default | Required | Description                                                |
|------------------|:-------:|:--------:|------------------------------------------------------------|
| log_api_enable   |    0    |    Yes   | Enable Log API plugin 0 Disable 1 Enable                   |
| log_api_address  |    -    |    Yes   | Set Log API Address Ie. https://api.yoursite.com/          |
| log_api_timeout  |   5.0   |    Yes   | Timeout in seconds to wait for response from remote server |
| log_api_bearer   |    -    |    No    | Bearer Token to use with HTTP                              |
| log_api_delay    |   60.0  |    No    | Delay to update Server Info on webserver                   |

## Enable / Disable events
To enable what event will be sent to webserver, edit events.json file.
The settings will be work after server restart.

```
{
	"ServerActivate": 		    false,
	"ServerDeactivate": 		false,
	"ServerAlertMessage": 		false,
	"ServerInfo": 			    false,
	"ClientConnect":		    false,
	"ClientPutInServer":		false,
	"ClientDisconnect":		    false,
	"ClientKill": 			    false,
	"ClientUserInfoChanged": 	false,
	"ClientCommand": 		    false,
	"ClientSay": 			    false,
	"ClientMenuHandle":		    false
}
```

## There is a small PHP Example
See how webserver can handle some of events using PHP.
In the example, when player join in server the webserver will return a chat message to all players, and do a sv_restart command.

1. Enable ClientPutInServer event on events.json
```
{
	"ServerActivate": 		    false,
	"ServerDeactivate": 	    false,
	"ServerAlertMessage": 	    false,
	"ServerInfo": 			    false,
	"ClientConnect":		    false,
	"ClientPutInServer":	    true,
	"ClientDisconnect":		    false,
	"ClientKill": 			    false,
	"ClientUserInfoChanged":	false,
	"ClientCommand": 		    false,
	"ClientSay": 			    false,
	"ClientMenuHandle":		    false
}
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

// Execute Commands
// Allows the webserver to execute commands on the CS server via the `ServerCommand` or `ServerExecute` JSON response
// Warning: Only enable if your webserver communication is strictly secure!
//
// 0 Disable execution of commands from webserver
// 1 Enable execution of commands from webserver
//
// Default "0"
log_api_exec_commands "0"
```

3. Let's create the api.php file (log_api_addres must be something like: https://yourwebsite.com/api.php)
```PHP
<?php
// Include the LogAPI Class
include("LogApi.php");

// Execute LogAPI OnEvent
(new LogAPI)->OnEvent();

// Exit
exit;
```

4. The LogAPI.php class to handle HLDS server events, you can create other classes to handle events later.
```PHP
<?php

class LogAPI
{
    /**
     * YOUR_API_TOKEN_CVAR must be exactly the same as configured
     * in the "log_api_bearer" cvar in the server's logapi.cfg
     */
    private $logApiToken = "YOUR_API_TOKEN_CVAR";

    /**
     * Internal Whitelist for Security (PREVENTS RCE).
     * ONLY these methods are allowed to be executed by the plugin.
     * Add any custom menu callback names to this array!
     */
    private $allowedEvents = [
        'ServerActivate', 'ServerDeactivate', 'ServerAlertMessage', 'ServerInfo',
        'ClientConnect', 'ClientPutInServer', 'ClientDisconnect', 'ClientKill',
        'ClientUserInfoChanged', 'ClientCommand', 'ClientSay', 'ClientMenuHandle',
        // Example Custom Menu Callbacks:
        'PlayerMenuHandle', 'Menu1Handle', 'Menu2Handle'
    ];

    /**
     * On Receive Event Target
     */
    public function OnEvent()
    {
        // 1. Robust Header Extraction (Supports Apache & Nginx environments)
        $headers = '';
        if (isset($_SERVER['Authorization'])) { $headers = trim($_SERVER["Authorization"]); }
        else if (isset($_SERVER['HTTP_AUTHORIZATION'])) { $headers = trim($_SERVER["HTTP_AUTHORIZATION"]); }
        else if (isset($_SERVER['REDIRECT_HTTP_AUTHORIZATION'])) { $headers = trim($_SERVER["REDIRECT_HTTP_AUTHORIZATION"]); }
        elseif (function_exists('apache_request_headers')) {
            $requestHeaders = apache_request_headers();
            $requestHeaders = array_combine(array_map('ucwords', array_keys($requestHeaders)), array_values($requestHeaders));
            if (isset($requestHeaders['Authorization'])) { $headers = trim($requestHeaders['Authorization']); }
        }
        
        // 2. Extract Token Hash
        $receivedToken = '';
        if (!empty($headers) && preg_match('/Bearer\s(\S+)/', $headers, $matches)) {
            $receivedToken = $matches[1];
        }

        // 3. Secure Timing-Attack Safe Comparison
        if (!empty($receivedToken) && hash_equals($this->logApiToken, $receivedToken))
        {
            // Parse JSON request into PHP array
            $request = json_decode(file_get_contents('php://input'), true);
            
            // Check if Request and Event names are valid
            if (!empty($request['Event']))
            {
                // Strict RCE Check & Method Verification
                if (in_array($request['Event'], $this->allowedEvents, true) && method_exists($this, $request['Event']))
                {
                    // Pass the ENTIRE array to the event handler to prevent parameter reordering attacks
                    // The function returns the Array response directly!
                    return $this->{$request['Event']}($request);
                }
            }
            return null;
        }
        else
        {
            // Authentication Failed
            header("HTTP/1.1 401 Unauthorized");
            return [ "error" => "Unauthorized HTTP 401" ]; 
        }
    }

    /**
     * =========================================
     *  EXAMPLES OF HOW TO HANDLE EVENTS
     * =========================================
     */

    /**
     * Example 1: Debugging payload and sending Server Console messages on Connect
     */
    protected function ClientConnect($request)
    {
        $PlayerName = $request['Player']['Name'];
        $EntityId   = $request['Player']['EntityId'];

        // Convert the full array to JSON String to print it to the console
        $debugString = json_encode($request);

        // Print debug to HLDS Server Console
        $retorno["ServerCommand"] =
        [
            "echo \"[PHP DEBUG] Player $PlayerName connected!\"",
            "echo \"[PHP DEBUG REQUEST] $debugString\""
        ];

        return $retorno;
    }

    /**
     * Example 2: ClientPutInServer - Sending a Chat Message and executing a server command
     */
    protected function ClientPutInServer($request)
    {
        $PlayerName = $request['Player']['Name'];

        $result["PrintChat"] =
        [
            'EntityId' => 0, // Entity Index 0 sends the message to ALL players
            'Message' => "^4[LogAPI]^1 {$PlayerName} entered the game. Restarting match in ^35^1 seconds..."
        ];

        // Execute server command in HLDS
        $result['ServerCommand'] = "sv_restart 5";

        return $result;
    }

    /**
     * Example 3: ClientDisconnect - Broadcast a message when someone leaves
     */
    protected function ClientDisconnect($request)
    {
        $PlayerName = $request['Player']['Name'];

        $result["PrintChat"] =
        [
            'EntityId' => 0,
            'Message'  => "^4[LogAPI]^1 Player ^3{$PlayerName}^1 disconnected from the server."
        ];

        return $result;
    }

    /**
     * Example 4: ClientKill - Show a center screen message when a player gets a headshot
     */
    protected function ClientKill($request)
    {
        // $request['Player'] is the victim, $request['Attacker'] (if any) is the killer
        $VictimName   = $request['Player']['Name'];
        $AttackerName = $request['Attacker']['Name'];
        $Weapon       = $request['Weapon'];
        $Headshot     = $request['Headshot'];

        // If it was a headshot, print a center message to everyone!
        if ($Headshot) {
            $result["ClientPrint"] = [
                'EntityId'  => 0, // 0 = All players
                'PrintType' => 4, // 4 = print_center
                'Message'   => "Boom! {$AttackerName} headshotted {$VictimName} with {$Weapon}!"
            ];
            return $result;
        }
        return null;
    }

    /**
     * Example 5: Intercepting Chat messages (.menu) and displaying a Menu to the player
     */
    protected function ClientSay($request)
    {
        // Extract variables from the injected array
        $Server  = $request['Server'];
        $Player  = $request['Player'];
        $Message = $request['Message'];

        $EntityId = $Player['EntityId'];
        $UserId   = $Player['UserId'];

        // If player types ".menu"
        if (strpos($Message, '.menu') === 0)
        {
            // Build the Menu Array Structure
            $menu =
            [
                "EntityId" => $EntityId,
                "Title"    => "Player List:",
                "Exit"     => true,
                "Callback" => "PlayerMenuHandle", // The callback function to call!
            ];

            // Loop through all players currently parsed in the Server object
            foreach ($Server['Players'] as $Auth => $PlayerItem)
            {
                $menu["Items"][] =
                [
                    "Info"     => $PlayerItem['Auth'],
                    "Text"     => $PlayerItem['Name'],
                    "Disabled" => false,
                    "Extra"    => "{$PlayerItem['UserId']}"
                ];
            }

            // Tell LogAPI to show a menu AND execute a command simultaneously 
            $resultado =
            [
                "ShowMenu"      => $menu,
                "ServerCommand" => ["log_psay #$UserId [LOGAPI] Select a player to kick."]
            ];

            return $resultado;
        }

        return null; // Ignore messages that are not commands
    }

    /**
     * Example 6: Handling the Callback of the Custom Menu and Kicking a player
     */
    protected function PlayerMenuHandle($request)
    {
        // Get the chosen data injected into "Extra" during Menu Creation
        $Extra  = $request['Item']['Extra'];
        
        $result["PrintChat"] = [
            "EntityId" => 0,
            "Message" => "Executing punishment on UserID #{$Extra} via PHP LogAPI..."
        ];
        
        // Execute kick command inside the engine
        $result["ServerCommand"] = "kick #{$Extra} \"BYE\"";

        return $result;
    }
}
```

## Development Alpha Stage
Please, consider that plugin is experimental. Any feedback / request is always welcome..

## Finally
A Future documentation how to use it will be provided in this repository.
