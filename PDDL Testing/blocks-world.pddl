;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;  Blocks world domains
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

(define (domain chapman-blocks-world)
	(:constants table)
	(:predicates
				(on ?x ?y)		;; Block ?x is on the top of Block ?y
				(clear ?x))		;; Nothing is on the top of Block ?x

	;; Block ?x is clear at the top of a tower and it is moved onto the table
	(:action NewTower
		:parameters	(?x ?z)
		:precondition	(and	(on ?x ?z)
								(clear ?x))
		:effect	(and	(on ?x table)
						(clear ?z)
						(not (on ?x ?z))))
						
	;; Block ?x is clear at the top of a tower and it is moved onto the top of another tower
	(:action PutOn
		:parameters (?x ?y ?z)
		:precondition	(and	(on ?x ?z)
								(clear ?x)
								(clear ?y))
		:effect	(and	(on ?x ?y)
						(not (on ?x ?z))
						(clear ?z)
						(not (clear ?y))))

)