;;; ten steps
(define (problem att-log3)
  (:domain logistics-strips)
  (:objects package1 package2 package3
	    pgh-truck bos-truck la-truck airplane1 airplane2
	    bos-po pgh-po la-po bos-airport pgh-airport la-airport
	    pgh bos la)
  (:init (OBJ package1)		; statis predicates
         (OBJ package2)
         (OBJ package3)
	 (TRUCK pgh-truck)
	 (TRUCK bos-truck)
	 (AIRPLANE airplane1)
	 (AIRPLANE airplane2)
	 (LOCATION bos-po)
	 (LOCATION pgh-po)
	 (LOCATION bos-airport)
	 (LOCATION pgh-airport)
	 (AIRPORT bos-airport)
	 (AIRPORT pgh-airport)
	 (CITY pgh)
	 (CITY bos)
	 (IN-CITY pgh-po pgh)
	 (IN-CITY pgh-airport pgh)
	 (IN-CITY bos-po bos)
	 (IN-CITY bos-airport bos)
	 (at package1 pgh-po);; dynamic predicates
	 (at package2 pgh-po)
	 (at package3 pgh-po)
	 (at airplane1 pgh-airport)
	 (at airplane2 pgh-airport)
	 (at bos-truck bos-po)
	 (at pgh-truck pgh-po))
  (:goal (and (at package1 bos-po))))

