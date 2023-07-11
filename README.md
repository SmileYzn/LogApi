<p align="center">
    <a href="https://github.com/SmileYzn/LogApi/issues"><img alt="GitHub Issues" src="https://img.shields.io/github/issues-raw/smileyzn/LogApi?style=flat-square"></a>
    <a href="https://github.com/SmileYzn/LogApi/actions"><img alt="GitHub Workflow Status" src="https://img.shields.io/github/actions/workflow/status/SmileYzn/LogApi/msbuild.yml?branch=main&label=Windows&style=flat-square"></a>
    <a href="https://github.com/SmileYzn/LogApi/actions"><img alt="GitHub Workflow Status" src="https://img.shields.io/github/actions/workflow/status/smileyzn/LogApi/makefile.yml?branch=main&label=Linux&style=flat-square"></a>
    <a href="https://github.com/SmileYzn/LogApi/releases/latest"><img src="https://img.shields.io/github/downloads/SmileYzn/LogApi/total?label=Download%40latest&style=flat-square&logo=github&logoColor=white" alt="Download"></a>
</p>

# HLDS Log API
MetaMod Plugin that uses JSON API to comunicate with WebServer using HTTP/s Protocol.

## What that plugin do
The Plugin sends requests to a webserver using JSON format.
And it can get responses from the webserver of what to do on the HLDS side.

## Console Vriables
| Console Variable | Default | Required | Description                                                |
|------------------|:-------:|:--------:|------------------------------------------------------------|
| log_api_enable   |    0    |    Yes   | Enable Log API plugin 0 Disable 1 Enable                   |
| log_api_address  |    -    |    Yes   | Set Log API Address Ie. https://api.yoursite.com/          |
| log_api_timeout  |   5.0   |    Yes   | Timeout in seconds to wait for response from remote server |
| log_api_bearer   |    -    |    No    | Bearer Token to use with HTTP                              |

## Enable / Disable events
To enable what event will be sent to webserver, edit events.json file.
The settings will be work after server change level or restart.

```
{
	"ServerActivate": 		false,
	"ServerDeactivate": 		false,
	"ServerAlertMessage": 		false,
	"ClientConnect":		false,
	"ClientPutInServer":		false,
	"ClientDisconnect":		false,
	"ClientKill": 			false,
	"ClientUserInfoChanged": 	false
}
```

## Development Alpha Stage
Please, consider that plugin is experimental. Any feedback / request is always welcome.

## Finally
A Future documentation how to use it will be provided in this repository.
