import sys
import re

def update_logplayer_cpp():
    path = r'LogApi\LogPlayer.cpp'
    with open(path, 'r', encoding='utf-8') as f:
        content = f.read()

    # 1. Connect
    content = re.sub(
        r'^([ \t]*this->m_Players\[Auth\]\.Deaths = 0;\n)$',
        r'\1\n\g<1>'.replace('Deaths = 0', 'Ping = 0'),
        content,
        flags=re.MULTILINE
    )

    # 2. Update
    #         this->m_Players[Auth].Deaths = Player->m_iDeaths;
    content = re.sub(
        r'^([ \t]*this->m_Players\[Auth\]\.Deaths = Player->m_iDeaths;\n)$',
        r'\1\n'
        r'\g<1>int ping = 0, loss = 0;\n'
        r'\g<1>g_engfuncs.pfnGetPlayerStats(pEdict, &ping, &loss);\n'
        r'\g<1>this->m_Players[Auth].Ping = ping;\n'.replace('this->m_Players[Auth].Deaths = Player->m_iDeaths;', ''),
        content,
        flags=re.MULTILINE
    )

    # 3. GetPlayerJson
    content = re.sub(
        r'^([ \t]*\{"Deaths", Player->Deaths\},\n)$',
        r'\1'
        r'\g<1>'.replace('"Deaths", Player->Deaths', '"Ping", Player->Ping'),
        content,
        flags=re.MULTILINE
    )

    with open(path, 'w', encoding='utf-8') as f:
        f.write(content)

def update_logapi_cpp():
    path = r'LogApi\LogApi.cpp'
    with open(path, 'r', encoding='utf-8') as f:
        content = f.read()

    # 1. GetServerInfo
    content = re.sub(
        r'^([ \t]*\{"Deaths", Player\.second\.Deaths\},\n)$',
        r'\1'
        r'\g<1>'.replace('"Deaths", Player.second.Deaths', '"Ping", Player.second.Ping'),
        content,
        flags=re.MULTILINE
    )

    with open(path, 'w', encoding='utf-8') as f:
        f.write(content)

if __name__ == "__main__":
    update_logplayer_cpp()
    update_logapi_cpp()
