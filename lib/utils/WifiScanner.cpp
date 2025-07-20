#include <WifiScanner.h>

std::vector<WifiScanner::Network> WifiScanner::scanNetworks(){
    std::vector<Network> networks;

    int n = WiFi.scanNetworks();
    for (int i = 0; i < n; ++i){
        Network net;
        net.ssid = WiFi.SSID(i);
        net.rssi = WiFi.RSSI(i);
        net.encrypted = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;
        networks.push_back(net);
    }

    return networks;
}

void WifiScanner::printNetworks(){
    auto nets = scanNetworks();
    Serial.println("Redes WiFi encontradas:");
    for(const auto& net:nets){
        Serial.printf("SSID: %s | RSSI: %d dBm | Seguridad: %s\n",
            net.ssid.c_str(), 
            net.rssi, 
            net.encrypted ? "Si" : "No");
    }
}
