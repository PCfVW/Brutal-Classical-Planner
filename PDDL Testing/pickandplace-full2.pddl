(define (problem pickandplace-full)
  (:domain sam)
  (:objects
		coca-can - object
		pt-random pt-init pt-ref pt-safe pt-transport pt-deliver pt-coca-approach pt-coca-obstacle pt-coca-grasping pt-coca-barrier pt-coca-ontology1 pt-coca-ontology2 pt-kitchen - point3D
	    rot-random rot-init rot-ref rot-safe rot-transport rot-deliver rot-coca-approach rot-coca-obstacle rot-coca-grasping rot-coca-barrier rot-coca-ontology1 rot-coca-ontology2 rot-kitchen - rotation3D
	    kitchen - location
	)
  (:init

	(arm-position           pt-random rot-random)

	(arm-initial-position   pt-init rot-init)
	(arm-reference-position pt-ref rot-ref)
	(arm-safe-position      pt-safe rot-safe)
	(arm-transport-position pt-transport rot-transport)
	(arm-deliver-position   pt-deliver rot-deliver)
	
	(not-initialization-arm)
	(not-initialization-SAM)
	(not-initialization-object-selection)
	(not-initialization-grasping)
	(not-initialization-dropping)

	(not-maps-loaded)

	(gripper-open)

	(at coca-can kitchen)
	(station kitchen pt-kitchen rot-kitchen)
	(at-robot kitchen)

	(approach-position coca-can pt-coca-approach rot-coca-approach)
	(ontology1 coca-can pt-coca-ontology1 rot-coca-ontology1)
	(ontology2 coca-can pt-coca-ontology2 rot-coca-ontology2)
	(optical-barrier-crossed coca-can pt-coca-barrier rot-coca-barrier)

	(click coca-can)

	;(goal-object coca-can)

	;(holding coca-can)
	;(arm-position pt-transport rot-transport)
	)

  (:goal (and
    ; To generate the first sequence (initialization)
	;(initialization-arm)
	;(arm-position pt-ref rot-ref)
	;(not-gripper-open)
	
    ; To generate the second sequence (loading maps)
	;(maps-loaded)
	
	; To generate the third sequence (object selection).
	;(goal-object coca-can)
	
	; To generate the fourth sequence (grasping)
	; put (goal-object coca-can) dans l'état initial (résultat de la 3e séquence).
	;(arm-position pt-transport rot-transport)

	; To generate the fifth sequence (dropping)
	; put in initial state : (not-gripper-open) (holding coca-can) (arm-position pt-transport rot-transport)
	(at coca-can pt-deliver)

	)))

