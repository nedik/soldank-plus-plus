#include <steam/steamnetworkingsockets.h>

#include <iostream>

int main()
{
    std::cout << "Server started!" << std::endl;

    SteamNetworkingIdentity identity{};

    identity.ParseString("str:peer-server");
    return 0;
}
