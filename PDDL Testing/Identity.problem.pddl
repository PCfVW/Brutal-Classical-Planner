;; -------------------------------------------------------------------------------------------- PDDL Problem file
;;
;;						 
;; 		 c                       c
;; 		 a  b                    a  b
;; 		------                  ------
;;   Initial State            Final State
;;
;; --------------------------------------------------------------------------------------------

(define (problem identity)
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
		(on a table)
		(on c a))))
		
;; -------------------------------------------------------------------------------------------- PDDL Problem file