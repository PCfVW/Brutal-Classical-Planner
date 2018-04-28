(define

	(problem bob-and-alice-case)

	(:domain Multi-Agent-Planner)

	(:objects
		bob	- Person
		alice - Person
		kitchen - Location
		den - Location
		SilverPlate - Locatable
		cabinetKey - Key
		cabinet - Container
	)

	(:init
		(in-room alice den)
		(in-room bob kitchen)
		(carrying alice cabineKey)
		(in-room cabinet den)
		(unlocks cabinetKey cabinet)
		(inside SilverPlate cabinet)
		(locked cabinet)
	)

	(:goal
		(carrying bob SilverPlate)
	)

)