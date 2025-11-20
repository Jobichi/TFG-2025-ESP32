#include <Connectivity/mqtt/TopicParser.h>

TopicInfo TopicParser::parse(const String &topic) {
    TopicInfo info;
    info.valid = false;

    // Separamos por "/"
    int idx1 = topic.indexOf('/');
    int idx2 = topic.indexOf('/', idx1 + 1);
    int idx3 = topic.indexOf('/', idx2 + 2);

    if (idx1 < 0 || idx2 < 0 || idx3 <0) {
        return info;    // Tópico invalido
    }

    info.action = topic.substring(0, idx1);
    info.device = topic.substring(idx1 + 1, idx2);
    info.type = topic.substring(idx2 + 1, idx3);

    String idStr = topic.substring(idx3 + 1);
    info.id = idStr.toInt();    // Convertimos el id a entero

    // Verificación básica {type}
    if (info.type != "sensor" && info.type != "actuator"){
        return info;
    }

    if (info.id < 0){
        return info;
    }

    info.valid = true;
    return info;    // Si supera todas las condiciones -> valido
}