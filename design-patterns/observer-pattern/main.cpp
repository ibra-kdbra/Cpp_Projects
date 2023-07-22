#include <iostream>
#include <memory>

#include "WeatherMeasurementService.h"
#include "WeatherStation1.h"
#include "WeatherStation2.h"

int main() {
    WeatherMeasurementService wms;
    wms.change(30);
    auto ws1 = std::make_shared<WeatherStation1>(1);
    auto ws2 = std::make_shared<WeatherStation2>(2);
    auto ws3 = std::make_shared<WeatherStation1>(3);
    wms.add(ws1);
    wms.add(ws2);
    wms.add(ws3);

    wms.addTemp(1.2);
    wms.remove(ws1);
    wms.change(28.8);

    return 0;
}
