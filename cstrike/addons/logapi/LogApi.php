<?php

class LogAPI
{
    /**
     * Called when a unknow function is received
     * 
     * @param string $name      Function name
     * @param type $arguments   Function argument list
     */
    function __call($name, $arguments)
    {
        // Do nothing here
    }
    
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
     * On Server Activate Event
     * 
     * 
     * @param string $Event             Event Name
     * @param array $Server		Server information data
     * @param string $EdictCount        Entity Count in Server (Number of edict)
     * @param string $ClientMax         Number of maximum allowed clients in server
     * 
     * @return mixed                    Array containing ServerExecute commands or null
     */
    private function ServerActivate($Event, $Server, $EdictCount, $ClientMax)
    {
        return null;
    }
    
    /**
     * On Server Deactivate Event
     * 
     * @param string $Event             Event Name
     * @param array $Server		Server information data
     * 
     * @return mixed                    Array containing ServerExecute commands or null
     */
    private function ServerDeactivate($Event, $Server)
    {
        return null;
    }
    
    /**
     * On Server Alert Message
     * 
     * @param string $Event             Event Name
     * @param array $Server		Server information data
     * @param string $Type              Alert Mesasge Type (Always 5 at_logged)
     * @param string $Message           Log string
     * 
     * @return mixed                    Array containing ServerExecute commands or null
     */
    private function ServerAlertMessage($Event, $Server, $Type, $Message)
    {
        return null;
    }

    /**
     * On Server Update Information
     * 
     * @param string $Event             Event Name
     * @param array $Server		Server information data
     *
     * @return mixed                    Array containing ServerExecute commands or null
     */
    private function ServerInfo($Event, $Server, $Address, $Hostname, $Map, $MaxPlayers, $Players)
    {
        return null;
    }
    
    /**
     * On Client Connect
     * 
     * @param string $Event             Event Name
     * @param array $Server		Server information data
     * @param string $UserId            User Index
     * @param string $Name              Client Name
     * @param string $AuthId            Client AuthId
     * @param string $Address           Client IP Address
     * 
     * @return mixed                    Array containing ServerExecute commands or null
     */
    private function ClientConnect($Event, $Server, $UserId, $Name, $AuthId, $Address)
    {
        return null;
    }
    
    /**
     * On Client Put In Server
     * 
     * @param string $Event             Event Name
     * @param array $Server		Server information data
     * @param string $UserId            User Index
     * @param string $Name              Client Name
     * @param string $AuthId            Client AuthId
     * 
     * @return mixed                    Array containing ServerExecute commands or null
     */
    private function ClientPutInServer($Event, $Server, $UserId, $Name, $AuthId)
    {
        return null;
    }
    
    /**
     * On Client Disconnect
     * 
     * @param string $Event             Event Name
     * @param array $Server		Server information data
     * @param string $UserId            User Index
     * @param string $Name              Client Name
     * @param string $AuthId            Client AuthId
     * 
     * @return mixed                    Array containing ServerExecute commands or null
     */
    private function ClientDisconnect($Event, $Server, $UserId, $Name, $AuthId)
    {
        return null;
    }
    
    /**
     * On Client Killed
     * 
     * @param string $Event             Event Name
     * @param array $Server		Server information data
     * @param string $UserId            User Index
     * @param string $Name              Client Name
     * @param string $AuthId            Client AuthId
     * 
     * @return mixed                    Array containing ServerExecute commands or null
     */
    private function ClientKill($Event, $Server, $UserId, $Name, $AuthId)
    {
        return null;
    }
    
    /**
     * On Client Information Changed
     * 
     * @param string $Event             Event Name
     * @param array $Server		Server information data
     * @param string $UserId            User Index
     * @param string $Name              Client Name
     * @param string $AuthId            Client AuthId
     * @param string $InfoBuffer        KeyInfoBuffer
     * 
     * @return mixed                    Array containing ServerExecute commands or null
     */
    private function ClientUserInfoChanged($Event, $Server, $UserId, $Name, $AuthId, $InfoBuffer)
    {
        return null;
    }
    
    /**
     * On Client Command
     * 
     * @param string $Event             Event Name
     * @param array $Server		Server information data
     * @param string $UserId            User Index
     * @param string $Name              Client Name
     * @param string $AuthId            Client AuthId
     * @param string $Command           Command
     * @param string $Args              Command Arguments
     * 
     * @return mixed                    Array containing ServerExecute commands or null
     */
    private function ClientCommand($Event, $Server, $UserId, $Name, $AuthId, $Command, $Args)
    {
        return null;
    }
    
    /**
     * On Client say or say_team commands
     * 
     * @param string $Event             Event Name
     * @param array $Server		Server information data
     * @param string $UserId            User Index
     * @param string $Name              Client Name
     * @param string $AuthId            Client AuthId
     * @param string $Type              Type (say or say_team)
     * @param string $Message           Message sent
     * 
     * @return mixed                    Array containing ServerExecute commands or null
     */
    private function ClientSay($Event, $Server, $UserId, $Name, $AuthId, $Type, $Message)
    {
        return null;
    }
}