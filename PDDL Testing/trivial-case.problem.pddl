(define

	(problem trivial-case)

	(:domain Multi-Agent-Planner)

	(:objects
		bob	- Person
		kitchen - Location
		den - Location
		SilverPlate - Locatable
	)

	(:init
		(in-room bob kitchen)
		(in-room SilverPlate den)
	)

	(:goal
		(carrying bob SilverPlate)
	)

)