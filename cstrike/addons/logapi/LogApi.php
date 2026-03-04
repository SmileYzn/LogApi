<?php

class LogAPI
{
    /**
     * The log api token cvar value
     * 
     * @param string
     */
    private $logApiToken = "YOUR_API_TOKEN_CVAR";

    /**
     * The allowed events whitelist
     * 
     * @var array
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
        'ClientMenuHandle'
    ];

    /**
     * On Receive Event
     */
    public function OnEvent()
    {
        // Compare HTTP_AUTHORIZATION with Bearer Token of log_api_bearer using constant-time comparison
        if (hash_equals($this->logApiToken, trim(str_replace('Bearer', '', $_SERVER['HTTP_AUTHORIZATION'] ?? '')))) {
            // Parse request as json event
            $request = json_decode(file_get_contents('php://input'), true);

            // If event is not empty
            if (!empty($request['Event'])) {
                // If event is allowed in whitelist and method exists
                if (in_array($request['Event'], $this->allowedEvents, true) && method_exists($this, $request['Event'])) {
                    // Return from function call (pass request array directly)
                    $result = $this->{$request['Event']}($request);

                    // Set PHP to return content type as json
                    header('Content-Type: application/json');

                    // Return final result encoded as json
                    die(json_encode($result));
                }
            }
        } else {
            header("HTTP/1.1 401 Unauthorized");
        }
    }

    /**
     * On Server Activate Event
     * 
     * 
     * @param array $request    Full request data with keys: Event, Server, EdictCount, ClientMax
     * 
     * @return mixed                Array containing LogAPI commands to server or null
     */
    protected function ServerActivate($request)
    {
        return null;
    }

    /**
     * On Server Deactivate Event
     * 
     * @param array $request    Full request data with keys: Event, Server
     * 
     * @return mixed            Array containing LogAPI commands to server or null
     */
    protected function ServerDeactivate($request)
    {
        return null;
    }

    /**
     * On Server Alert Message
     * 
     * @param array $request    Full request data with keys: Event, Server, Type, Message
     * 
     * @return mixed            Array containing LogAPI commands to server or null
     */
    protected function ServerAlertMessage($request)
    {
        return null;
    }

    /**
     * On Server Update Information
     * 
     * @param array $request    Full request data with keys: Event, Server
     *
     * @return mixed            Array containing LogAPI commands to server or null
     */
    protected function ServerInfo($request)
    {
        return null;
    }

    /**
     * On Client Connect
     * 
     * @param array $request    Full request data with keys: Event, Server, Player
     * 
     * @return mixed            Array containing LogAPI commands to server or null
     */
    protected function ClientConnect($request)
    {
        return null;
    }

    /**
     * On Client Put In Server
     * 
     * @param array $request    Full request data with keys: Event, Server, Player
     * 
     * @return mixed            Array containing LogAPI commands or null
     */
    protected function ClientPutInServer($request)
    {
        return null;
    }

    /**
     * On Client Disconnect
     * 
     * @param array $request    Full request data with keys: Event, Server, Player, Crash, Reason
     * 
     * @return mixed            Array containing LogAPI commands to server or null
     */
    protected function ClientDisconnect($request)
    {
        return null;
    }

    /**
     * On Client Killed
     * 
     * @param array $request    Full request data with keys: Event, Server, Player
     * 
     * @return mixed            Array containing LogAPI commands or null
     */
    protected function ClientKill($request)
    {
        return null;
    }

    /**
     * On Client Information Changed
     * 
     * @param array $request    Full request data with keys: Event, Server, Player, InfoBuffer
     * 
     * @return mixed                Array containing LogAPI commands to server or null
     */
    protected function ClientUserInfoChanged($request)
    {
        return null;
    }

    /**
     * On Client Command
     * 
     * @param array $request    Full request data with keys: Event, Server, Player, Command, Args
     * 
     * @return mixed            Array containing LogAPI commands to server or null
     */
    protected function ClientCommand($request)
    {
        return null;
    }

    /**
     * On Client say or say_team commands
     * 
     * @param array $request    Full request data with keys: Event, Server, Player, Type, Message
     * 
     * @return mixed             Array containing LogAPI commands to server or null
     */
    protected function ClientSay($request)
    {
        return null;
    }

    /**
     * On Client menu handle command
     * 
     * @param array $request    Full request data with keys: Event, Server, Player, Item (Info, Text, Disabled, Extra)
     *
     * @return mixed            Array containing LogAPI commands to server or null
     */
    protected function ClientMenuHandle($request)
    {
        return null;
    }
}
