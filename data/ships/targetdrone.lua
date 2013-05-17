-- Copyright © 2008-2013 Pioneer Developers. See AUTHORS.txt for details
-- Licensed under the terms of CC-BY-SA 3.0. See licenses/CC-BY-SA-3.0.txt

define_ship {
	name='Target drone',
	model='lunarshuttle',
	forward_thrust = 20e5,
	reverse_thrust = 5e5,
	up_thrust = 3e5,
	down_thrust = 3e5,
	left_thrust = 1e5,
	right_thrust = 1e5,
	angular_thrust = 40e5,
	camera_offset = v(0,4,-22),
	max_cargo = 0,
	capacity = 30,
	hull_mass = 30,
	fuel_tank_mass = 25,
	-- Exhaust velocity Vc [m/s] is equivalent of engine efficiency and depend on used technology. Higher Vc means lower fuel consumption.
	-- Smaller ships built for speed often mount engines with higher Vc. Another way to make faster ship is to increase fuel_tank_mass.
	effective_exhaust_velocity = 80000e3,
	price = 0,
	hyperdrive_class = 0,
	max_engines = 0,
}
