(define
   (problem test)
   (:domain far-cry-strips)
   (:objects Soldier        - player
             Stand          - posture
             Crouch         - posture
             Swim           - posture
             Enemy-1        - enemy
             Enemy-2    - enemy
             Hand-gun    - hand-gun
             Big-hand-gun  - light-weight-anti-tank-munition
             Normal-power   - power
             Strong-power   - power
             Card-key-1     - card-key
             North          - direction
             North-east     - direction
             North-west     - direction
             South          - direction
             South-east     - direction
             South-west     - direction
             East           - direction
             West           - direction
             Start-point    - location
             Hut-1          - location
             Hut-2          - location
             Hut-3          - location
             Hut-4          - location
             Hut-5          - location
             Hut-5-inside   - location
             Hut-6          - location
             Hut-6-inside   - location
             Mini-Island    - location
             Enemy-area-1     - location
             Enemy-area-2 - location
             Destination-point - location
             Storehouse    - location
             )

   (:init
            ;Directions
            (possible-direction North)
            (possible-direction North)
            (possible-direction North-east)
            (possible-direction North-west)
            (possible-direction South)
            (possible-direction South-east)
            (possible-direction South-west)
            (possible-direction East)
            (possible-direction West)
            
            ;Postures
            (possible-posture Stand)
            (possible-posture Crouch)
            (possible-posture Swim)
            
            ;Player
            (player-position Soldier Start-point)
            (player-direction Soldier North)
            (posture-of Soldier Crouch)
            ;;(not (already-mounted Soldier))
            ;;(not (is-in-hand Soldier Gun))
            ;;(not (mounted Soldier Gun))

            ;Enemy
            (entity-position Enemy-1 Enemy-area-1)
            (is-enemy-power Enemy-1 Normal-power)
            (is-enemy Enemy-1)
            ;;(not (is-killed-enemy Enemy-1))

            (entity-position Enemy-2 Enemy-area-2)
            (is-enemy-power Enemy-2 Strong-power)
            (is-enemy Enemy-2)
            ;;(not (is-killed-enemy Enemy-2))

            ;Weapon
            (entity-position Hand-gun Storehouse)
            (is-weapon-power Hand-gun Normal-power)
            ;;(not (is-searched-goods Hand-gun))
            ;;(not (is-observed-goods Hand-gun))
            ;;(not (is-checked-goods Hand-gun))

            (entity-position Big-hand-gun Hut-6-inside)
            (is-weapon-power Big-hand-gun Strong-power)
            ;;(not (is-searched-goods Big-hand-gun))
            ;;(not (is-observed-goods Big-hand-gun))
            ;;(not (is-checked-goods Big-hand-gun))

            ;Key
            (entity-position Card-key-1 Hut-5-inside)
            (is-key-for-the-arc Card-key-1 Hut-6 Hut-6-inside)
            ;;(not (is-searched-goods Card-key-1))
            ;;(not (is-observed-goods Card-key-1))
            ;;(not (is-checked-goods Card-key-1))


            ;Pass network
            (arc Start-point Hut-1 North-east Stand)
            (arc Hut-1 Start-point South-west Stand)
            (arc Hut-1 Hut-2 East Crouch)
            (arc Hut-2 Hut-1 West Crouch)
            (arc Hut-2 Storehouse East Crouch)
            (arc Storehouse Hut-2 West Crouch)
            (arc Hut-2 Hut-4 South Stand)
            (arc Hut-4 Hut-2 North Stand)
            (arc Start-point Hut-3 South-east Stand)
            (arc Hut-3 Start-point North-west Stand)
            (arc Hut-3 Hut-4 East Stand)
            (arc Hut-4 Hut-3 West Stand)
            (arc Hut-4 Enemy-area-1 South Stand)
            (arc Enemy-area-1 Hut-4 North Stand)
            (arc Enemy-area-1 Mini-Island East Swim)
            (arc Mini-Island Enemy-area-1 West Swim)
            (arc Mini-Island Hut-5 East Swim)
            (arc Hut-5 Mini-Island West Swim)
            (arc Hut-5 Hut-5-inside North Stand)
            (arc Hut-5-inside Hut-5 South Stand)
            (arc Hut-5 Hut-6 East Stand)
            (arc Hut-6 Hut-5 West Stand)
            (arc Hut-6 Hut-6-inside North Stand)
            (arc Hut-6-inside Hut-6 South Stand)
            (arc Hut-6 Enemy-area-2 East Stand)
            (arc Enemy-area-2 Hut-6 West Stand)
            (arc Enemy-area-2 Destination-point East Stand)
            (arc Destination-point Enemy-area-2 West Stand)

            ;Permission to go
            (have-permission Soldier Start-point Hut-1)
            (have-permission Soldier Hut-1 Start-point)
            (have-permission Soldier Hut-1 Hut-2)
            (have-permission Soldier Hut-2 Hut-1)
            (have-permission Soldier Hut-2 Storehouse)
            (have-permission Soldier Storehouse Hut-2)
            (have-permission Soldier Hut-2 Hut-4)
            (have-permission Soldier Hut-4 Hut-2)
            (have-permission Soldier Start-point Hut-3)
            (have-permission Soldier Hut-3 Start-point)
            (have-permission Soldier Hut-3 Hut-4)
            (have-permission Soldier Hut-4 Hut-3)
            (have-permission Soldier Hut-4 Enemy-area-1)
            (have-permission Soldier Enemy-area-1 Hut-4)
            (have-permission Soldier Enemy-area-1 Mini-Island)
            (have-permission Soldier Mini-Island Enemy-area-1)
            (have-permission Soldier Mini-Island Hut-5)
            (have-permission Soldier Hut-5 Mini-Island)
            (have-permission Soldier Hut-5 Hut-5-inside)
            (have-permission Soldier Hut-5-inside Hut-5)
            (have-permission Soldier Hut-5 Hut-6)
            (have-permission Soldier Hut-6 Hut-5)
           ; (not (have-permission Soldier Hut-6 Hut-6-inside))
            (have-permission Soldier Hut-6-inside Hut-6)
            (have-permission Soldier Hut-6 Enemy-area-2)
            (have-permission Soldier Enemy-area-2 Hut-6)
            (have-permission Soldier Enemy-area-2 Destination-point)
            (have-permission Soldier Destination-point Enemy-area-2)



            ;location condition
            (is-safe-location Start-point)
            (is-safe-location Hut-1)
            (is-safe-location Hut-2)
            (is-safe-location Hut-3)
            (is-safe-location Hut-4)
            (is-safe-location Hut-5)
            (is-safe-location Hut-5-inside)
            (is-safe-location Hut-6)
            (is-safe-location Hut-6-inside)
            (is-safe-location Mini-Island)
            (is-safe-location Storehouse)
            (is-safe-location Destination-point)
            ;;(not (is-safe-location Enemy-area-1))
            ;;(not (is-checked-enemy Enemy-1))
            ;;(not (is-safe-location Big-bos-house))
            ;;(not (is-checked-enemy Enemy-2))
   )

   (:goal
             (player-position Soldier Destination-point)
   )
)
