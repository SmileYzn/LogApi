# HLDS Log API
MetaMod Plugin that uses JSON API to comunicate with WebServer using HTTP/s Protocol.

## What that do
The Plugin sends requests to a webserver using JSON format.
And it can get responses from the webserver of what to do on the HLDS side.

## Console Vriables
| Console Variable | Default | Required | Description                                                |
|------------------|:-------:|:--------:|------------------------------------------------------------|
| log_api_enable   |    0    |    Yes   | Enable Log API plugin 0 Disable 1 Enable                   |
| log_api_address  |    -    |    Yes   | Set Log API Address Ie. https://api.yoursite.com/          |
| log_api_timeout  |   5.0   |    Yes   | Timeout in seconds to wait for response from remote server |
| log_api_bearer   |    -    |    No    | Bearer Token to use with HTTP                              |

## Development Alpha Stage
