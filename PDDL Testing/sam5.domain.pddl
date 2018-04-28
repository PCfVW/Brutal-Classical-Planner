; Philippe Morignot - philippe.morignot@cea.fr
; Domain : the mobile robot SAM (gripper, arm and mobile base).
; Created on July 12, 2010.

(define (domain sam)
   ;;(:requirements :strips)
   (:types object location point3D point2D rotation3D)
   (:predicates

	(mobile-base ?pt ?a)

	(initialization-arm) (not-initialization-arm)
	(initialization-SAM) (not-initialization-SAM)
	(initialization-object-selection) (not-initialization-object-selection)
	(initialization-grasping) (not-initialization-grasping)
	(initialization-dropping) (not-initialization-dropping)

	(arm-position           ?pt3D ?rot)

	(arm-initial-position   ?pti ?roti)
	(arm-reference-position ?ptr ?rotr)
	(arm-safe-position      ?pts ?rots)
	(arm-transport-position ?ptt ?rott)
	(arm-deliver-position   ?ptd ?rotd)

	(maps-loaded) (not-maps-loaded)

	(gripper-open) (not-gripper-open)

	(station ?l ?pt ?rot)
	(at-robot ?l) 

	(approach-position ?o ?pt ?rot)
	(ontology1 ?o ?pt ?rot)
	(ontology2 ?o ?pt ?rot)
	(optical-barrier-crossed ?o ?pt ?rot)	; Position of the arm when the gripper has its optical barrier crossed around object ?o

	(at ?o ?pto)

	(holding ?o)

	(goal-object ?o)
	(click ?o)          ; object ?o selected by the user
   )

; ------------------------------------------------------------------------------------------------------
; Initializations
; ------------------------------------------------------------------------------------------------------

(:action initialize-arm ; InitStateInitArm
   :parameters ()
   :precondition                          (not-initialization-arm)
   :effect (and (initialization-arm) (not (not-initialization-arm))))

(:action initialize-SAM ; InitStateSAM
   :parameters (?pt - point3D ?rot - rotation3D)
   :precondition (and                     (not-initialization-SAM) (initialization-arm) (arm-position ?pt ?rot) (arm-reference-position ?pt ?rot) (not-gripper-open))
   :effect (and (initialization-SAM) (not (not-initialization-SAM))))

(:action initialize-object-selection ; InitStateSelectObjectForGrasping
   :parameters ()
   :precondition (and                                  (not-initialization-object-selection) (maps-loaded))
   :effect (and (initialization-object-selection) (not (not-initialization-object-selection))))

(:action initialize-grasping ; InitStateArmGrasp
   :parameters (?o - object)
   :precondition (and                          (not-initialization-grasping) (goal-object ?o))
   :effect (and (initialization-grasping) (not (not-initialization-grasping))))

(:action initialize-dropping ; InitStateArmDrop
   :parameters (?o - object ?pt - point3D ?rot - rotation3D)
   :precondition (and  (not-initialization-dropping)  (holding ?o) (arm-position ?pt ?rot) (arm-transport-position ?pt ?rot))
   :effect (and   (not (not-initialization-dropping)) (initialization-dropping)))

; ------------------------------------------------------------------------------------------------------
; Opening / closing the gripper, with or without holding an object.
; ------------------------------------------------------------------------------------------------------

(:action open-gripper ; OpenTheGripper
   :parameters ()
   :precondition (and                (not-gripper-open) (initialization-object-selection))
   :effect  (and (gripper-open) (not (not-gripper-open))))

(:action open-gripper-with-object ; OpenGripperState
   :parameters (?o - object ?pt - point3D ?rot - rotation3D)
   :precondition (and      (not-gripper-open)       (holding ?o)  (arm-position ?pt ?rot) (arm-deliver-position ?pt ?rot))
   :effect  (and      (not (not-gripper-open)) (not (holding ?o)) (at ?o ?pt)))

(:action close-gripper ; CloseGripper
   :parameters (?pt - point3D ?rot - rotation3D)
   :precondition (and (gripper-open) (arm-position ?pt ?rot) (arm-reference-position ?pt ?rot))
   :effect  (and (not (gripper-open)) (not-gripper-open)))

(:action close-gripper-on-object ; CloseGripperState
   :parameters (?o - object ?l - location ?pt - point3D ?rot - rotation3D)
   :precondition (and (gripper-open) (goal-object ?o) (at ?o ?l) (arm-position ?pt ?rot) (optical-barrier-crossed ?o ?pt ?rot))
   :effect  (and (not (gripper-open)) (not-gripper-open) (holding ?o) (not (at ?o ?l))))

; ------------------------------------------------------------------------------------------------------
; Arm motion
; ------------------------------------------------------------------------------------------------------

(:action load-maps ; SAMStartLoadMap
   :parameters ()
   :precondition (and             (not-maps-loaded) (initialization-SAM))
   :effect (and (maps-loaded) (not (not-maps-loaded))))

; Move the arm to the position of the station (i.e., the room where the robot is).
(:action position-arm-for-grasping ; PositArmForGrasp
   :parameters (?pt - point3D ?rot - rotation3D ?l - location ?pt2 - point3D ?rot2 - rotation3D)
   :precondition (and (arm-position ?pt ?rot) (at-robot ?l) (station ?l ?pt2 ?rot2) (initialization-object-selection) (gripper-open))
   :effect (and (arm-position ?pt2 ?rot2) (not (arm-position ?pt ?rot))))

; If the gripper is open, the arm is at the station position, and the user clicks on the object, this is the goal object.
(:action wait-for-object-designation ; WaitDesignationState
   :parameters (?o - object ?l - location ?pt - point3D ?rot - rotation3D)
   :precondition (and (gripper-open) (click ?o) (at-robot ?l) (station ?l ?pt ?rot) (arm-position ?pt ?rot))
   :effect (and (goal-object ?o)))

(:action move-towards-object ; ObjectSelectedState
   :parameters (?pt - point3D ?rot - rotation3D ?o - object ?pt2 - point3D ?rot2 - rotation3D)
   :precondition (and (arm-position ?pt ?rot) (goal-object ?o) (approach-position ?o ?pt2 ?rot2) (initialization-grasping))
   :effect (and (arm-position ?pt2 ?rot2) (not (arm-position ?pt ?rot))))

(:action avoid-obstacle-before-object1 ; AvoidObstacleGripStrategy
   :parameters (?pt - point3D ?rot - rotation3D ?o - object ?pt2 - point3D ?rot2 - rotation3D)
   :precondition (and (arm-position ?pt ?rot) (goal-object ?o) (approach-position ?o ?pt ?rot) (ontology1 ?o ?pt2 ?rot2))
   :effect (and (arm-position ?pt2 ?rot2) (not (arm-position ?pt ?rot))))

(:action avoid-obstacle-before-object2 ; GripStrategy
   :parameters (?pt - point3D ?rot - rotation3D ?o - object ?pt2 - point3D ?rot2 - rotation3D)
   :precondition (and (arm-position ?pt ?rot) (ontology1 ?o ?pt ?rot) (goal-object ?o) (ontology2 ?o ?pt2 ?rot2))
   :effect (and (arm-position ?pt2 ?rot2) (not (arm-position ?pt ?rot))))

(:action blind-grasp ; BlindGrasp
   :parameters (?pt - point3D ?rot - rotation3D ?o - object ?pt2 - point3D ?rot2 - rotation3D)
   :precondition (and (arm-position ?pt ?rot) (ontology2 ?o ?pt ?rot) (goal-object ?o) (optical-barrier-crossed ?o ?pt2 ?rot2))
   :effect (and (arm-position ?pt2 ?rot2) (not (arm-position ?pt ?rot))))

; ------------------------------------------------------------------------------------------------------
; Move the arm to pre-defined positions.
; ------------------------------------------------------------------------------------------------------

(:action move-arm-to-reference-position ; MoveToArmReferencePositionState
   :parameters (?pt - point3D ?rot - rotation3D ?pt2 - point3D ?rot2 - rotation3D)
   :precondition (and (arm-position ?pt ?rot) (arm-reference-position ?pt2 ?rot2) (initialization-arm))
   :effect (and  (not (arm-position ?pt ?rot)) (arm-position ?pt2 ?rot2)))

(:action move-arm-to-safe-position ; MoveToSafePose
   :parameters (?o - object ?pt - point3D ?rot - rotation3D ?pt2 - point3D ?rot2 - rotation3D)
   :precondition (and (arm-position ?pt ?rot) (arm-safe-position ?pt2 ?rot2) (holding ?o))
   :effect (and (not (arm-position ?pt ?rot)) (arm-position ?pt2 ?rot2)))

(:action move-arm-to-transport-position ; MoveToTransportPosition
   :parameters (?pt - point3D ?rot - rotation3D ?pt2 - point3D ?rot2 - rotation3D)
   :precondition (and (arm-position ?pt ?rot) (arm-safe-position ?pt ?rot) (arm-transport-position ?pt2 ?rot2))
   :effect (and (not (arm-position ?pt ?rot)) (arm-position ?pt2 ?rot2)))

(:action move-arm-to-deliver-position ; DeliverState
   :parameters (?pt - point3D ?rot - rotation3D ?o - object ?pt2 - point3D ?rot2 - rotation3D)
   :precondition (and (arm-position ?pt ?rot) (arm-transport-position ?pt ?rot) (arm-deliver-position ?pt2 ?rot2) (holding ?o) (initialization-dropping))
   :effect (and (not (arm-position ?pt ?rot)) (arm-position ?pt2 ?rot2)))
)

; =================================================================================================
