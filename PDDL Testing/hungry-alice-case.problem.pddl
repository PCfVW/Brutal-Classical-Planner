(define

	(problem hungry-alice-case)

	(:domain Multi-Agent-Planner)

	(:objects
		bob	- Person
		alice - Person
		kitchen - Location
		den - Location
		SilverPlate - Locatable
		cabinetKey - Key
		cabinet - Container
		lasagna - Food
		fridge - Container
	)

	(:init
		(in-room alice den)
		(in-room bob kitchen)
		(carrying alice cabineKey)
		(in-room cabinet den)
		(unlocks cabinetKey cabinet)
		(locked cabinet)
		(hungry alice)
		(inside SilverPlate cabinet)
		(inside lasagne fridge)

		(in-room fridge kitchen)
		(unlocked fridge)
	)
		
	(:goal
		(not (hungry alice))
	)

)