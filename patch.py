import os

def patch_logplayer_h():
    path = r'LogApi\LogPlayer.h'
    with open(path, 'r', encoding='utf-8') as f:
        content = f.read()

    # Look for 'int Deaths;\n\tfloat GameTime;' and insert 'int Ping;\n'
    target = '\tint Deaths;\n\tfloat GameTime;'
    replacement = '\tint Deaths;\n\tint Ping;\n\tfloat GameTime;'
    content = content.replace(target, replacement)
    with open(path, 'w', encoding='utf-8') as f:
        f.write(content)

def patch_logplayer_cpp():
    path = r'LogApi\LogPlayer.cpp'
    with open(path, 'r', encoding='utf-8') as f:
        content = f.read()

    # 1. Connect
    # \t\t\tthis->m_Players[Auth].Deaths = 0;\n\n\t\t\tthis->m_Players[Auth].GameTime
    target1 = '\t\t\tthis->m_Players[Auth].Deaths = 0;\n\n\t\t\tthis->m_Players[Auth].GameTime'
    replacement1 = '\t\t\tthis->m_Players[Auth].Deaths = 0;\n\n\t\t\tthis->m_Players[Auth].Ping = 0;\n\n\t\t\tthis->m_Players[Auth].GameTime'
    content = content.replace(target1, replacement1)

    # 2. Update
    # \t\t\t\tthis->m_Players[Auth].Deaths = Player->m_iDeaths;\n\n\t\t\t\tif (this->m_Players[Auth].GameTime
    target2 = '\t\t\t\tthis->m_Players[Auth].Deaths = Player->m_iDeaths;\n\n\t\t\t\tif (this->m_Players[Auth].GameTime'
    replacement2 = '\t\t\t\tthis->m_Players[Auth].Deaths = Player->m_iDeaths;\n\n\t\t\t\tint ping = 0, loss = 0;\n\t\t\t\tg_engfuncs.pfnGetPlayerStats(pEdict, &ping, &loss);\n\t\t\t\tthis->m_Players[Auth].Ping = ping;\n\n\t\t\t\tif (this->m_Players[Auth].GameTime'
    content = content.replace(target2, replacement2)

    # 3. GetPlayerJson
    # \t\t\t\t\t\t\t\t\t\t\t\t\t\t{"Deaths", Player->Deaths},\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t{"GameTime"
    target3_alt = '{"Deaths", Player->Deaths},'
    replacement3 = '{"Deaths", Player->Deaths},\n\t\t\t\t\t\t\t\t\t\t\t\t\t\t{"Ping", Player->Ping},'
    content = content.replace(target3_alt, replacement3)

    with open(path, 'w', encoding='utf-8') as f:
        f.write(content)

def patch_logapi_cpp():
    path = r'LogApi\LogApi.cpp'
    with open(path, 'r', encoding='utf-8') as f:
        content = f.read()

    # 1. GetServerInfo
    # {"Deaths", Player.second.Deaths},
    target1 = '{"Deaths", Player.second.Deaths},'
    replacement1 = '{"Deaths", Player.second.Deaths},\n\t\t\t\t\t{"Ping", Player.second.Ping},'
    content = content.replace(target1, replacement1)

    with open(path, 'w', encoding='utf-8') as f:
        f.write(content)

if __name__ == '__main__':
    patch_logplayer_h()
    patch_logplayer_cpp()
    patch_logapi_cpp()
