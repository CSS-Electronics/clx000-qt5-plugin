#include "CLX000Plugin.h"

#include "CLX000CanBus.h"
#include "CLX000CanBusUSB.h"

QList<QCanBusDeviceInfo> CLX000CanBusPlugin::availableDevices(QString *errorMessage) const {
    Q_UNUSED(errorMessage);
    return CLX000CanBus::interfaces();
}

QCanBusDevice *CLX000CanBusPlugin::createDevice(const QString &interfaceName, QString *errorMessage) const {
    Q_UNUSED(errorMessage);

    // Locate info.
    auto const info = CLX000CanBusPlugin::availableDevices(errorMessage);

    for(auto const& entry: info) {
        if(entry.name() == interfaceName) {
            return new CLX000CanBusUSB(interfaceName);
        }
    }

    return nullptr;
}
