;
;       29/09/09
;       --------
;
;       introducing the crush-iceblock operator
;       and the push-iceblock operator
;       modify (or restore) the kick-to-kill-guard operator
;


(define (domain pengi-strips)

   (:requirements :typing)
   (:types   coord-i coord-j flame - object )

   (:predicates

		(guard ?f - flame ?x  - coord-i ?y  - coord-j ?z  - coord-i ?t  - coord-j)
		(position ?x  - coord-i ?y - coord-j)  ; validate the pengi locattion near the weapon
		(iced-coin ?x  - coord-i ?y - coord-j)  ; the location of the coin
		(crushable-cell ?x  - coord-i ?y - coord-j ?z  - coord-i ?t - coord-j)  ; valid if the path is safe
		(pushable-cell ?x  - coord-i ?y - coord-j ?z  - coord-i ?t - coord-j)  ; valid if the path is safe
		(protected-cell ?x  - coord-i ?y - coord-j)  ; valid if the path is safe
		(blocked-by-cell  ?x  - coord-i ?y - coord-j ?z  - coord-i ?t  - coord-j)
		(blocked-by-weapon  ?x  - coord-i ?y - coord-j ?z  - coord-i ?t  - coord-j)
		(connected-with-cell  ?x  - coord-i ?y - coord-j ?z  - coord-i ?t  - coord-j)
		(blocked-path  ?x  - coord-i ?y - coord-j)
		(reachable-cell ?x  - coord-i ?y - coord-j)  ; valid if the path is free
		(extracted ?x  - coord-i ?y - coord-j)  ; valid if the coin is extracted
                   ; expresses the gaur effect on a given coin
		(weapon ?x  - coord-i ?y  - coord-j ?z  - coord-i ?t  - coord-j ?u  - coord-i ?v  - coord-j ?w  - coord-i ?s  - coord-j)
		  ; identifies a weapon with new and old coordinates on a certain flame
                )


; action move to cell
; -------------------
   (:action move-to-cell
       :parameters  (?pengix  - coord-i ?pengiy  - coord-j ?coinx  - coord-i ?coiny - coord-j )
       :precondition (and  (reachable-cell ?coinx ?coiny)
                           (position ?pengix ?pengiy) )
       :effect (and
                     (position ?coinx ?coiny)
                    (not (position ?pengix ?pengiy))
;		     (not (protected-cell ?coinx ?coiny))
                     ))

; action extract
; --------------
   (:action extract
       :parameters (?coinx  - coord-i ?coiny - coord-j )
       :precondition  (and  (position ?coinx ?coiny)
                            (iced-coin ?coinx ?coiny)
                            (protected-cell ?coinx ?coiny)
                            (reachable-cell ?coinx ?coiny))
       :effect (and (extracted ?coinx ?coiny)
		    (not (iced-coin ?coinx ?coiny))
                    (not (protected-cell ?coinx ?coiny))
                    (not (reachable-cell ?coinx ?coiny))))


; action push iceblock
; ---------------------
   (:action push-iceblock
       :parameters (?iceblockx  - coord-i ?iceblocky  - coord-j
                    ?newiceblockx  - coord-i ?newiceblocky  - coord-j  ?blockedx  - coord-i ?blockedy - coord-j )
       :precondition  (and
                           (position ?iceblockx ?iceblocky)
                           (reachable-cell  ?iceblockx ?iceblocky)
                           (protected-cell ?iceblockx ?iceblocky)
                           (pushable-cell ?iceblockx ?iceblocky ?newiceblockx  ?newiceblocky)
                           (blocked-by-cell ?blockedx ?blockedy ?iceblockx ?iceblocky)
       ; the weapon blocks
       ; the pengi is located next to the weapon
;                             (position ?weaponx ?weapony))
                             )
       :effect (and
;                    (protected-cell ?blockedx ?blockedy)   ; the path to the cell is safe
                    ;(reachable-cell  ?newiceblockx ?newiceblocky) ; the new weapon location is free
                    ;(protected-cell  ?newiceblockx ?newiceblocky) ; the new weapon location is safe
                    ;(position ?iceblockx ?iceblocky)
                    (reachable-cell ?blockedx ?blockedy)   ; the path to the cell is not blocked
                    ;(position ?blockedx ?blockedy)        ; the pengi is next to the blocked cell
;                    (position ?blockedx ?blockedy)        ; the pengi is next to the blocked cell
		    ;(not (position ?iceblockx ?iceblocky))  ; the pengi is no longer ar the iceblock
		    (not (reachable-cell ?iceblockx ?iceblocky))  ; the pengi is no longer ar the iceblock
		    (not (pushable-cell ?iceblockx ?iceblocky ?newiceblockx  ?newiceblocky))  ; the pengi is no longer ar the iceblock
                    (not (blocked-by-cell ?blockedx ?blockedy ?iceblockx ?iceblocky))))
                    ; the cell is no longer blocked



; action crush iceblock
; ---------------------
   (:action crush-iceblock
       :parameters (?iceblockx  - coord-i ?iceblocky  - coord-j  ?newiceblockx  - coord-i ?newiceblocky  - coord-j
                        ?blockedx  - coord-i ?blockedy - coord-j )
       :precondition  (and
                           (position ?iceblockx ?iceblocky)
                           (reachable-cell  ?iceblockx ?iceblocky)
                           (protected-cell ?iceblockx ?iceblocky)
                           (crushable-cell ?iceblockx ?iceblocky ?newiceblockx  ?newiceblocky)
                           (blocked-by-cell ?blockedx ?blockedy ?iceblockx ?iceblocky)
                             )
       :effect (and
                    (reachable-cell ?blockedx ?blockedy)   ; the path to the cell is not blocked
                    (position ?newiceblockx ?newiceblocky)        ; the pengi is next to the blocked cell
		    (not (position ?iceblockx ?iceblocky))  ; the pengi is no longer ar the iceblock
		    (not (reachable-cell ?iceblockx ?iceblocky))  ; the pengi is no longer ar the iceblock
		    (not (crushable-cell ?iceblockx ?iceblocky ?newiceblockx  ?newiceblocky))  ; the pengi is no longer ar the iceblock
                    (not (blocked-by-cell ?blockedx ?blockedy ?iceblockx ?iceblocky))))




; action destroy weapon
; ---------------------
   (:action destroy-weapon
       :parameters (?weaponx  - coord-i ?weapony  - coord-j  ?blockedx  - coord-i ?blockedy - coord-j )
       :precondition  (and
                           (reachable-cell  ?weaponx ?weapony)
                           (protected-cell ?weaponx ?weapony)
                           (blocked-by-weapon ?blockedx ?blockedy ?weaponx ?weapony)
       ; the weapon blocks
       ; the pengi is located next to the weapon
;                             (position ?weaponx ?weapony))
                             (position ?weaponx ?weapony))
       :effect (and
;                    (protected-cell ?blockedx ?blockedy)   ; the path to the cell is safe
                    (reachable-cell ?blockedx ?blockedy)   ; the path to the cell is not blocked
                    (position ?blockedx ?blockedy)        ; the pengi is next to the blocked cell
;                    (position ?blockedx ?blockedy)        ; the pengi is next to the blocked cell
		    (not (position ?weaponx ?weapony))  ; the pengi is no longer ar the weapon
                    (not (blocked-by-weapon ?blockedx ?blockedy ?weaponx ?weapony))))
                    ; the cell is no longer blocked



; action set reachable
; --------------------
   (:action set-reachable
       :parameters  (?cellx  - coord-i ?celly  - coord-j ?othercellx  - coord-i ?othercelly - coord-j )
       :precondition (and  (connected-with-cell ?othercellx ?othercelly ?cellx ?celly) (reachable-cell ?cellx ?celly) )
       :effect (and
                     (reachable-cell ?othercellx ?othercelly)
                     ))


; action kick to kill guard
; -------------------------
   (:action kick-to-kill-guard
       :parameters (?reachablewx  - coord-i ?reachablewy  - coord-j     ; coordinates of the movable cell to reach the weapon
                    ?weaponx  - coord-i ?weapony  - coord-j     ; coordinates of the weapon
                    ?newweaponx  - coord-i ?newweapony  - coord-j  ; new coordinates of the weapon
                    ?flame - flame ; storing the flame number
                    ?guardx  - coord-i ?guardy  - coord-j ; coordinates of the guard
                    ?coinx  - coord-i ?coiny  - coord-j  ; coordinates of the coin
                    ?blockedx  - coord-i ?blockedy - coord-j ) ; coordinates of the blocked cell
       :precondition  (and   (position ?reachablewx ?reachablewy) ; the pengi is at the movable cell to reach the weapon
                             (iced-coin ?coinx ?coiny)     ; the iced coin exists
                             (guard ?flame ?guardx ?guardy ?coinx ?coiny) ; the iced coin is guarded
                             (weapon ?reachablewx ?reachablewy ?weaponx ?weapony  ?newweaponx ?newweapony ?blockedx ?blockedy)
                             (reachable-cell ?reachablewx ?reachablewy)
                             (protected-cell ?reachablewx ?reachablewy)
                             )
                             ; the weapon has a linear trajectory and blocks a cell
       :effect (and (position ?weaponx ?weapony) ; the pengi is at the weapon
                    (protected-cell ?coinx ?coiny)  ; the path to the coin is safe
                    (blocked-path ?blockedx ?blockedy)  ; the path is blocked for the blocked cell
                    (blocked-by-weapon ?blockedx ?blockedy ?newweaponx ?newweapony) ; the path is blocked by the weapon
                    (reachable-cell  ?newweaponx ?newweapony) ; the new weapon location is free
                    (protected-cell  ?newweaponx ?newweapony) ; the new weapon location is safe
                    (not (position ?reachablewx ?reachablewy)) ; the Pengi is not longer at the movable cell to reach the weapon
                    (not (reachable-cell ?reachablewx ?reachablewy))  ; the path to the weapon is not free
                    (not (protected-cell ?reachablewx ?reachablewy))  ; the path to the weapon is nor safe
                    (not (reachable-cell ?blockedx ?blockedy))      ; the path to the stop is not free
		    (not (guard ?flame ?guardx ?guardy ?coinx ?coiny))   ; the guard is killed
                    (not (weapon ?reachablewx ?reachablewy ?weaponx ?weapony  ?newweaponx ?newweapony ?blockedx ?blockedy))))
)