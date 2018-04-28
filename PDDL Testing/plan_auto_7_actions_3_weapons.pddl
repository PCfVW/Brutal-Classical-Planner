(define
(problem premier-plan)
(:domain pengi-strips)
(:objects   f-0 - flame  i-1 - coord-i  i-3 - coord-i  i-4 - coord-i  i-7 - coord-i  i-9 - coord-i  i-8 - coord-i  i-6 - coord-i  j-1 - coord-j  j-8 - coord-j  j-5 - coord-j  j-2 - coord-j  j-3 - coord-j  j-7 - coord-j  j-9 - coord-j  j-4 - coord-j )
(:init
(guard f-0 i-8 j-5 i-9 j-5)

(blocked-by-cell i-8 j-2 i-4 j-8)
(protected-cell i-8 j-2)
(weapon i-8 j-2 i-8 j-3 i-8 j-7 i-8 j-8)

(blocked-by-cell i-6 j-5 i-4 j-8)
(protected-cell i-6 j-5)
(weapon i-6 j-5 i-7 j-5 i-8 j-5 i-9 j-5)

(blocked-by-cell i-8 j-9 i-4 j-8)
(protected-cell i-8 j-9)
(weapon i-8 j-9 i-8 j-8 i-8 j-4 i-8 j-3)

(position i-1 j-1)
(protected-cell i-1 j-1)
(reachable-cell i-1 j-1)

(iced-coin i-9 j-5)
(reachable-cell i-9 j-5 )

(reachable-cell i-3 j-8 )
(protected-cell i-3 j-8 )
(crushable-cell i-3 j-8 i-4 j-8 )

(protected-cell i-4 j-8 )
(pushable-cell i-4 j-8 i-7 j-8)

(blocked-by-cell i-4 j-8 i-3 j-8)

)
(:goal
;(protected-cell i-9 j-5 )
	 (extracted i-9 j-5)
)
)
