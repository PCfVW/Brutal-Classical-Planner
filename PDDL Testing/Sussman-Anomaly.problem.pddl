;; -------------------------------------------------------------------------------------------- PDDL Problem file
;;
;;								 a
;; 		 c                       b
;; 		 a  b                    c
;; 		------                  ---
;;   Initial State			Final State
;;
;; --------------------------------------------------------------------------------------------

(define (problem sussman-anomaly)
  (:domain chapman-blocks-world)
  (:objects a b c table)
  (:init
	(clear c)
	(on c a)
	(on a table)
	(clear b)
	(on b table))
	
  (:goal
	(and
		(on a b)
		(on b c))))
		
;; -------------------------------------------------------------------------------------------- PDDL Problem file
