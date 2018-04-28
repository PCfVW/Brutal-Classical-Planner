;; ----------------------------------------------------------------------------------
;; PDDL Domain adapted from:
;;		Jamie Cheng & Finnegan Southey,
;;			Implementing Practical Planning, Game Programming Gems (Ed. Kim Pallister),
;;			Charles River Media (2005), pages 329--344.
;; ----------------------------------------------------------------------------------
(define (domain Multi-Agent-Planner)

	(:requirements :strips :typing :negative-preconditions)

	(:types
		Locatable	- object
		Location	- object
		Creature	- Loocatable
		Person		- Creature
		Dog			- Creature
		Container	- Locatable
		Item		- Locatable
		Key			- Item
		Plate		- Item
		Food		- Item
	)

	(:predicates
		(in-room ?a - Locatable ?b - Location)	;; Object ?a is in location ?b
		(carrying ?c - Creature ?i - Item)		;; Creature ?c carries item ?i
		(locked ?c - Container)					;; Container ?c is locked
		(unlocks ?k - Key ?c - Container)		;; Key ?k unlocks container ?c
		(hungry ?c - Creature)					;; Creature ?c is hungry
		(prepared ?f - Food)					;; Food ?f is prepared
		(inside ?i - Item ?c - Container)		;; Item ?i is inside container ?c
		(unlocked ?c - Containter)				;; Container ?c is unlocked
	)

	(:action take
		:parameters	(?p - Person ?i - Item ?r - Location)
		:precondition	(and
							(in-room ?p ?r)
							(in-room ?i ?r)
						)
		:effect			(and
							(not (in-room ?i ?r))
							(carrying ?p ?i)
						)
	)

	(:action goto
		:parameters	(?p - Person ?r1 ?r2 - Location)
		:precondition	(and
							(not (in-room ?p ?r2))
							(in-room ?p ?r1)
						)
		:effect			(and
							(in-room ?p ?r2)
							(not (in-room ?p ?r1))
						)
	)

	(:action drop
		:parameters	(?p - Person ?i - Item ?r - Location)
		:precondition	(and
							(in-room ?p ?r)
							(carrying ?p ?i)
						)
		:effect			(and
							(in-room ?i ?r)
							(not (carrying ?p ?i))
						)
	)

	(:action remove
		:parameters	(?p - Person ?i - Item ?v - Container ?r - Location)
		:precondition	(and
							(inside ?i ?v)
							(in-room ?p ?r)
							(in-room ?v ?r)
							(unlocked ?v)
						)
		:effect			(and
							(carrying ?p ?i)
							(not (inside ?i ?v))
						)
	)

	(:action unlock
		:parameters	(?p - Person ?v - Container ?k - Key)
		:precondition	(and
							(locked ?v)
							(unlocks ?k ?v)
							(carrying ?p ?k)
						)
		:effect			(and
							(not (locked ?v))
						)
	)

	(:action prepare
		:parameters	(?p - Person ?f - Food)
		:precondition	(and
							(carrying ?p ?f)
							(not (prepared ?f))
						)
		:effect			(and
							(prepared ?f)
						)
	)

	(:action eat
		:parameters	(?p - Person ?f - Food ?pl - Plate)
		:precondition	(and
							(carrying ?p ?f)
							(carrying ?p ?pl)
							(hungry ?p)
							(prepared ?f)
						)
		:effect			(and
							(not (hungry ?p))
							(not (carrying ?p ?f))
						)
	)
)