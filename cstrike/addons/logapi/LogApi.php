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
     * ONLY these methods and your custom menu callbacks
     * are allowed to be executed by the plugin!
     */
    private $allowedEvents = [
        'ServerActivate',
        'ServerDeactivate',
        'ServerAlertMessage',
        'ServerInfo',
        'ClientConnect',
        'ClientPutInServer',
        'ClientDisconnect',
        'ClientKill',
        'ClientUserInfoChanged',
        'ClientCommand',
        'ClientSay',
        'ClientMenuHandle', // Original Plugin method for generic menus

        // Your Custom Menu Callbacks:
        'PlayerMenuHandle',
        'Menu1Handle',
        'Menu2Handle'
    ];

    /**
     * On Receive Event
     */
    public function OnEvent()
    {
        $headers = '';
        if (isset($_SERVER['Authorization'])) {
            $headers = trim($_SERVER["Authorization"]);
        } else if (isset($_SERVER['HTTP_AUTHORIZATION'])) {
            $headers = trim($_SERVER["HTTP_AUTHORIZATION"]);
        } else if (isset($_SERVER['REDIRECT_HTTP_AUTHORIZATION'])) {
            $headers = trim($_SERVER["REDIRECT_HTTP_AUTHORIZATION"]);
        } elseif (function_exists('apache_request_headers')) {
            $requestHeaders = apache_request_headers();
            $requestHeaders = array_combine(array_map('ucwords', array_keys($requestHeaders)), array_values($requestHeaders));
            if (isset($requestHeaders['Authorization'])) {
                $headers = trim($requestHeaders['Authorization']);
            }
        }

        // Extract token
        $receivedToken = '';
        if (!empty($headers) && preg_match('/Bearer\s(\S+)/', $headers, $matches)) {
            $receivedToken = $matches[1];
        }

        if (!empty($receivedToken) && hash_equals($this->logApiToken, $receivedToken)) {

            $request = json_decode(file_get_contents('php://input'), true);

            if (!empty($request['Event'])) {
                if (in_array($request['Event'], $this->allowedEvents, true) && method_exists($this, $request['Event'])) {
                    return $this->{$request['Event']}($request);
                }
            }

            return null;
        } else {
            // Authentication Failed
            header("HTTP/1.1 401 Unauthorized");
            return ["error" => "Unauthorized HTTP 401", "received" => $receivedToken]; // Return JSON payload for debugging if needed
        }
    }

    /**
     * On Server Activate Event
     * Fired when the map starts or server boots up.
     * 
     * @param array $request {
     *     @type string "Event"      "ServerActivate"
     *     @type array  "Server"     Hostname, Game, Address, Map, MaxPlayers, Time, EngineTime
     *     @type int    "EdictCount" Number of entities currently in the server
     *     @type int    "ClientMax"  Maximum number of clients
     * }
     * 
     * @return mixed Array containing LogAPI commands to server or null
     */
    protected function ServerActivate($request)
    {
        return null;
    }

    /**
     * On Server Deactivate Event
     * Fired when the map ends or server shuts down.
     * 
     * @param array $request {
     *     @type string "Event"  "ServerDeactivate"
     *     @type array  "Server" Server data
     * }
     * 
     * @return mixed Array containing LogAPI commands to server or null
     */
    protected function ServerDeactivate($request)
    {
        return null;
    }

    /**
     * On Server Alert Message
     * Fired when the engine prints an alert message.
     * 
     * @param array $request {
     *     @type string "Event"   "ServerAlertMessage"
     *     @type array  "Server"  Server data
     *     @type int    "Type"    The type of alert (usually 5 for at_logged)
     *     @type string "Message" The text printed to the engine log
     * }
     * 
     * @return mixed Array containing LogAPI commands to server or null
     */
    protected function ServerAlertMessage($request)
    {
        return null;
    }

    /**
     * On Server Update Information
     * Fired periodically based on `log_api_delay`
     * 
     * @param array $request {
     *     @type string "Event"  "ServerInfo"
     *     @type array  "Server" Server data (includes inner 'Players' array with Auth, Name, UserId, Ip, Team, Ping, etc)
     * }
     *
     * @return mixed Array containing LogAPI commands to server or null
     */
    protected function ServerInfo($request)
    {
        return null;
    }

    /**
     * On Client Connect
     * Fired immediately when a client connects to the server (before entering).
     * 
     * @param array $request {
     *     @type string "Event"  "ClientConnect"
     *     @type array  "Server" Server data
     *     @type array  "Player" Player data (Auth, Name, UserId, EntityId, Ip, Team, Flags)
     * }
     * 
     * @return mixed Array containing LogAPI commands to server or null
     */
    protected function ClientConnect($request)
    {
        return null;
    }

    /**
     * On Client Put In Server
     * Fired when the player enters the game world for the first time.
     * 
     * @param array $request {
     *     @type string "Event"  "ClientPutInServer"
     *     @type array  "Server" Server data
     *     @type array  "Player" Player data
     * }
     * 
     * @return mixed Array containing LogAPI commands or null
     */
    protected function ClientPutInServer($request)
    {
        return null;
    }

    /**
     * On Client Disconnect
     * Fired when a player leaves the server.
     * 
     * @param array $request {
     *     @type string "Event"   "ClientDisconnect"
     *     @type array  "Server"  Server data
     *     @type array  "Player"  Player data
     *     @type bool   "Crash"   True if player dropped due to timeout/crash
     *     @type string "Reason"  The drop reason string
     * }
     * 
     * @return mixed Array containing LogAPI commands to server or null
     */
    protected function ClientDisconnect($request)
    {
        return null;
    }

    /**
     * On Client Killed
     * Fired when a player dies.
     * 
     * @param array $request {
     *     @type string "Event"    "ClientKill"
     *     @type array  "Server"   Server data
     *     @type array  "Player"   The player who DIED (Victim)
     *     @type array  "Attacker" The player who KILLED (optional, empty if world kill/suicide)
     *     @type string "Weapon"   The weapon name used (e.g. "ak47")
     *     @type bool   "Headshot" True if the kill was a headshot
     *     @type bool   "Suicide"  True if the player killed themselves
     * }
     * 
     * @return mixed Array containing LogAPI commands or null
     */
    protected function ClientKill($request)
    {
        return null;
    }

    /**
     * On Client Information Changed
     * Fired when a client changes their setinfo/keyinfobuffer.
     * 
     * @param array $request {
     *     @type string "Event"      "ClientUserInfoChanged"
     *     @type array  "Server"     Server data
     *     @type array  "Player"     Player data
     *     @type string "InfoBuffer" The full new InfoBuffer string
     * }
     * 
     * @return mixed Array containing LogAPI commands to server or null
     */
    protected function ClientUserInfoChanged($request)
    {
        return null;
    }

    /**
     * On Client Command
     * Fired when a client executes ANY command in their client console.
     * 
     * @param array $request {
     *     @type string "Event"   "ClientCommand"
     *     @type array  "Server"  Server data
     *     @type array  "Player"  Player data
     *     @type string "Command" The main command string executed
     *     @type string "Args"    Arguments sent along with the command
     * }
     * 
     * @return mixed Array containing LogAPI commands to server or null
     */
    protected function ClientCommand($request)
    {
        return null;
    }

    /**
     * On Client say or say_team commands
     * Fired when a player talks in chat.
     * 
     * @param array $request {
     *     @type string "Event"   "ClientSay"
     *     @type array  "Server"  Server data
     *     @type array  "Player"  Player data
     *     @type string "Type"    Chat type ("say" or "say_team")
     *     @type string "Message" The actual message typed
     * }
     * 
     * @return mixed Array containing LogAPI commands to server or null
     */
    protected function ClientSay($request)
    {
        return null;
    }

    /**
     * On Client menu handle command
     * Triggered when a player presses a key on a general LogAPI menu.
     * 
     * @param array $request {
     *     @type string "Event"  Depends on the callback string (e.g. "ClientMenuHandle")
     *     @type array  "Server" Server data
     *     @type array  "Player" Player data
     *     @type array  "Item" {
     *         @type int    "Info"     The Info key of the item
     *         @type string "Text"     The display text
     *         @type bool   "Disabled" If the item was disabled
     *         @type string "Extra"    Any extra string passed
     *     }
     * }
     *
     * @return mixed Array containing LogAPI commands to server or null
     */
    protected function ClientMenuHandle($request)
    {
        return null;
    }
}
