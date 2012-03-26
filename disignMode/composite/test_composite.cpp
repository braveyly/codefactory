#include "basic_vehicle.h"
#include "intelligent_vehicle.h"
#include "tire.h"
#include "steer_wheel.h"
#include "navigator.h"
#include "engine_motor.h"

int main()
{
	TIRE tire("one tire");
	STEER_WHEEL wheel("one wheel");
	NAVIGATOR navigator("one navigator");
	ENGINE_MOTOR engine("one engine");
	BASIC_VEHICLE bike("my bike");
	bike.add(&tire);
	bike.add(&wheel);
	cout<<"basic vehicle power is"<<bike.power()<<"name"<<bike.name()<<endl;

	INTELLIGENT_VEHICLE bus("my bus");
	bus.add(&tire);
	bus.add(&wheel);
	bus.add(&navigator);
	bus.add(&engine);
	cout<<"intelligent vehicle power is"<<bus.power()<<"name"<<bus.name()<<endl;

}
