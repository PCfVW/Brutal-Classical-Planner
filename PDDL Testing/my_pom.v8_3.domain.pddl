(define (domain far-cry-strips)

   (:requirements :typing :negative-preconditions)
   (:types

          entity
          direction
          posture

          location        - entity
          person          - entity
          goods           - entity
          power           - entity
          door            - entity

          weapon          - goods
          key             - goods

          player          - person
          enemy           - person
          
          enemy-power     - enemy

          hand-gun                        - weapon
          light-weight-anti-tank-munition - weapon
          weapon-power                    - weapon
          
          card-key                        - key
   )

   (:predicates
         (arc ?start-location - location ?end-location - location ?direction - direction ?posture-required - posture)
         (possible-direction ?direction - direction)
         (player-position ?player - player ?location - location)
         (player-direction ?player - player ?direction - direction)
         (entity-position ?entity - entity ?location - location)
         (is-safe-location ?location - location)
         (is-enemy ?enemy - enemy)
         (is-killed-enemy ?enemy - enemy)
         (mounted ?player - player ?entity - goods)
         (already-mounted ?player - player)
         (is-founded-entity ?entity)
         (is-spotted-entity ?entity)
         (possible-posture ?posture)
         (posture-of ?person - person ?posture - posture)
         (is-checked-goods ?goods -goods)
         (is-in-hand ?player - player ?goods -goods)
         (have-permission ?player - player ?start-location -location ?end-location - location)
         (is-enemy-power ?enemy - enemy ?power - power)
         (is-weapon-power ?weapon - weapon ?power - power)
         (is-key-for-the-arc ?key - key ?start-location -location ?end-location - location)
   )

   (:action orient
         :parameters   (?player - player ?old-direction - direction ?new-direction - direction)
         :precondition (and (player-direction ?player ?old-direction)
                            (possible-direction ?new-direction))
         :effect       (and (player-direction ?player ?new-direction)
                            (not (player-direction ?player ?old-direction))
                       )
   )

   (:action move
         :parameters   (?player - player ?start-location - location ?end-location - location ?player-direction -direction ?posture-required - posture)
         :precondition (and (player-position ?player ?start-location)
                            (player-direction ?player ?player-direction)
                            (arc ?start-location ?end-location ?player-direction ?posture-required)
                            (is-safe-location ?start-location)
                            (posture-of ?player ?posture-required)
                            (have-permission ?player ?start-location ?end-location)
                       )
         :effect       (and (player-position ?player ?end-location)
                            (not (player-position ?player ?start-location))
                       )
   )

   (:action search
         :parameters   (?player - player ?entity - entity ?location - location)
         :precondition (and (entity-position ?entity ?location)
                            (player-position ?player ?location)
                            (not (is-founded-entity ?entity))
                            (not (is-spotted-entity ?entity))
                       )
         :effect       (and (is-founded-entity ?entity)
                            (not (is-spotted-entity ?entity))
                       )
   )

   (:action observe
         :parameters   (?player - player ?entity - entity ?location - location)
         :precondition (and (entity-position ?entity ?location)
                            (player-position ?player ?location)
                            (is-founded-entity ?entity)
                            (not (is-spotted-entity ?entity))
                       )
         :effect       (is-spotted-entity ?entity)
   )

   (:action pick-up
         :parameters   (?player - player ?goods - goods ?location - location)
         :precondition (and (player-position ?player ?location)
                            (entity-position ?goods ?location)
                            (is-safe-location ?location)
                            (is-founded-entity ?goods)
                            (not (is-checked-goods ?goods))
                            (not (is-in-hand ?player ?goods))
                       )
         :effect       (and (is-in-hand ?player ?goods)
                            (is-checked-goods ?goods)
                       )
   )

   (:action fire
         :parameters   (?player - player ?weapon - weapon ?power - power ?enemy - enemy ?location - location)
         :precondition (and (entity-position ?enemy ?location)
                            (player-position ?player ?location)
                            (is-founded-entity ?enemy)
                            (is-spotted-entity ?enemy)
                            (is-enemy ?enemy)
                            (mounted ?player ?weapon)
                            (is-weapon-power ?weapon ?power)
                            (is-enemy-power ?enemy ?power)
                            (not (is-killed-enemy ?enemy))
                            (not (is-safe-location ?location))
                       )
         :effect       (and (is-safe-location ?location)
                            (not (entity-position ?enemy ?location))
                            (is-killed-enemy ?enemy)
                       )
   )

   (:action mount
         :parameters   (?player - player ?goods - goods)
         :precondition (and (is-in-hand ?player ?goods)
                            (not (mounted ?player ?goods))
                            (not (already-mounted ?player))
                       )
         :effect       (and (already-mounted ?player)
                            (mounted ?player ?goods)
                       )
   )
   
   (:action dismount
         :parameters   (?player - player ?goods - goods)
         :precondition (and (is-in-hand ?player ?goods)
                            (already-mounted ?player)
                            (mounted ?player ?goods)
                       )
         :effect       (and (not (already-mounted ?player))
                            (not (mounted ?player ?goods))
                       )
   )

   
   (:action Use-key
         :parameters   (?player - player ?key - key ?start-location - location ?end-location - location ?player-direction - direction ?posture-required - posture)
         :precondition (and (is-in-hand ?player ?key)
                            (is-key-for-the-arc ?key ?start-location ?end-location)
                            (player-position ?player ?start-location)
                            (player-direction ?player ?player-direction)
                            (arc ?start-location ?end-location ?player-direction ?posture-required)
                            (not (have-permission ?player ?start-location ?end-location))
                       )
         :effect       (and (have-permission ?player ?start-location ?end-location)
                       )
   )


   (:action set-posture
         :parameters   (?person - person ?old-posture - posture ?new-posture - posture)
         :precondition (and (posture-of ?person ?old-posture)
                            (possible-posture ?new-posture)
                       )
         :effect       (and (posture-of ?person ?new-posture)
                            (not (posture-of ?person ?old-posture))
                       )
   )

)