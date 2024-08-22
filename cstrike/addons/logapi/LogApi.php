<?php

class LogAPI
{
    /**
     * On Receive Event
     *
     * @return mixed                    Array containing LogAPI commands or null
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
     * @return mixed                    Array containing LogAPI commands to server or null
     */
    protected function ServerActivate($Event, $Server, $EdictCount, $ClientMax)
    {
        return null;
    }
    
    /**
     * On Server Deactivate Event
     * 
     * @param string $Event             Event Name
     * @param array $Server		Server information data
     * 
     * @return mixed                    Array containing LogAPI commands to server or null
     */
    protected function ServerDeactivate($Event, $Server)
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
     * @return mixed                    Array containing LogAPI commands to server or null
     */
    protected function ServerAlertMessage($Event, $Server, $Type, $Message)
    {
        return null;
    }

    /**
     * On Server Update Information
     * 
     * @param string $Event             Event Name
     * @param array $Server		Server information data
     *
     * @return mixed                    Array containing LogAPI commands to server or null
     */
    protected function ServerInfo($Event, $Server)
    {
        return null;
    }
    
    /**
     * On Client Connect
     * 
     * @param string $Event             Event Name
     * @param array $Server		Server information data
     * @param array $Player		Player information data
     * 
     * @return mixed                    Array containing LogAPI commands to server or null
     */
    protected function ClientConnect($Event, $Server, $Player)
    {
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
        return null;
    }
    
    /**
     * On Client Disconnect
     * 
     * @param string $Event             Event Name
     * @param array $Server		Server information data
     * @param array $Player		Player information data
     * 
     * @return mixed                    Array containing LogAPI commands to server or null
     */
    protected function ClientDisconnect($Event, $Server, $Player)
    {
        return null;
    }
    
    /**
     * On Client Killed
     * 
     * @param string $Event             Event Name
     * @param array $Server		Server information data
     * @param array $Player		Player information data
     * 
     * @return mixed                    Array containing LogAPI commands or null
     */
    protected function ClientKill($Event, $Server, $Player)
    {
        return null;
    }
    
    /**
     * On Client Information Changed
     * 
     * @param string $Event             Event Name
     * @param array $Server		Server information data
     * @param array $Player		Player information data
     * @param string $InfoBuffer        KeyInfoBuffer
     * 
     * @return mixed                    Array containing LogAPI commands to server or null
     */
    protected function ClientUserInfoChanged($Event, $Server, $Player, $InfoBuffer)
    {
        return null;
    }
    
    /**
     * On Client Command
     * 
     * @param string $Event             Event Name
     * @param array $Server		Server information data
     * @param array $Player		Player information data
     * @param string $Command           Command
     * @param string $Args              Command Arguments
     * 
     * @return mixed                    Array containing LogAPI commands to server or null
     */
    protected function ClientCommand($Event, $Server, $Player, $Command, $Args)
    {
        return null;
    }
    
    /**
     * On Client say or say_team commands
     * 
     * @param string $Event             Event Name
     * @param array $Server		Server information data
     * @param array $Player		Player information data
     * @param string $Type              Type (say or say_team)
     * @param string $Message           Message sent
     * 
     * @return mixed                    Array containing LogAPI commands to server or null
     */
    protected function ClientSay($Event, $Server, $Player, $Type, $Message)
    {
        return null;
    }

    /**
     * On Client menu handle command
     * 
     * @param string $Event     Event Name (Menu callback name)
     * @param array $Server     Server information data
     * @param array $Player     Player information data
     * @param string $Info     Info string passed to menu item option
     * @param string $Text     Text of option string passed to menu item
     * @param boolean $Disabled     This option is disabled or not
     * @param string $Extra     Extra information string passed to menu item option
     *
     * @return mixed                    Array containing LogAPI commands to server or null
     */
    protected function ClientMenuHandle($Event, $Server, $Player, $Info, $Text, $Disabled, $Extra)
    {
        return null;
    }
}
